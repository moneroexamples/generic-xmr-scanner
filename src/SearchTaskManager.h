#ifndef SEARCHTASKMGR_H
#define SEARCHTASKMGR_H

#include "searchtasks/OutputSearchTask.h"
#include "searchtasks/TestSearchTask.h"
#include "utility/traits.hpp"

#include "FiberPool.hpp"

#include <boost/type_index.hpp>

#include <any>
#include <numeric>
#include <unordered_map> 
#include <string>

namespace xmreg
{

using namespace std::chrono_literals;


class SearchTaskManager
{

public:

using task_t = std::shared_ptr<ISearchTask>;  

using task_future_t = boost::fibers::future<void>;

using task_map_t = std::unordered_map<
                            std::string, 
                            std::unique_ptr<ISearchObserver>>;

explicit SearchTaskManager(MicroCore* _mcore)
    : m_core {_mcore}
{}


/**
 * Checks if tasks already exists. If not creates
 * Task instance, submits it to fiberpool 
 * and registers it with the task manager's m_tasks
 */
auto push(std::shared_ptr<ISearchTask>&& task, 
          WebSocketConnectionPtr const& ws_conn)
{
    assert(m_core != nullptr);

    task->set_microcore(m_core);
    
    auto key = task->key();

    // set message aggragate for the task to use
    
    std::lock_guard lk {m_tasks_mtx};
    
    // before submitting the task to fiberpool
    // and registring it with task manager,
    // check if it already exists

    if (auto existing_it = m_tasks.find(key);
        existing_it != m_tasks.end())
    {
        cerr << "Tasks already exists\n";
        
        // task already exists, so return it iterator
        // and set bool flag as false to indicate that 
        // it has not been just created.
        
        auto existing_task_ptr = static_cast<Task*>(
                existing_it->second.get());

        existing_task_ptr->add_conn(ws_conn); 
        
        return make_tuple(existing_it, false);
    }
    
    // tasks does not exist, so we submit it to fiberpool

    // submit our job to the the pool
    auto opt_future = DefaultFiberPool::submit_job(
            [task = task]() 
            {
                // capture shared_ptr<task_variant_t> by copy
                // the fiber should participate in the tasks
                // lifespan
                
                // run the task
                (*task)();
            });

    // if submiting task to fiberpool failed, returned
    if (!opt_future)
    {
        return make_tuple(m_tasks.end(), false);
    }

    // create Task object which will represent the task
    // in the m_tasks map
    
    unique_ptr<ISearchObserver> tsk 
        = make_unique<Task>(std::move(task),
                            std::move(*opt_future),
                            ws_conn);

    // create entry in the task map
    auto [it, success] = m_tasks.insert(
               {key, std::move(tsk)}); 
    
    return make_tuple(it, success);
}

/*
 * Removes disconnected connections
 * and tasks from m_tasks if they are done
 * or there are no connections to them
 */
void clean_up_tasks() 
{
    std::lock_guard lk {m_tasks_mtx};
    
    std::cout << "running " << m_tasks.size() 
              << " tasks"   << std::endl;

    for (auto it = m_tasks.cbegin(); it != m_tasks.end();)
    {
       auto const& [key, task] = *it;
        
       auto task_ptr = static_cast<Task*>(task.get());

       task_ptr->remove_disconnected();

       if (task_ptr->conns.empty())
       {
           // if all connection lost to the given task
           // ask it to finish
           task_ptr->task->finish();
       }

        if (auto status = task_ptr->task_future.wait_for(0ms);
            status == boost::fibers::future_status::ready)
        {
            it = m_tasks.erase(it);
            cout << "deleted key " << key << "from m_tasks" << endl;
        }
        else
        {
            ++it;
        }
    }
}


void managment_loop()
{
    for(;;)
    {
        clean_up_tasks();
        boost::this_fiber::sleep_for(5s);
    }
}


private:

/*
 * Representation of a task in our task_map
 * It will also listen for any search results
 * found
 */
struct Task : public ISearchObserver
{
    task_t task;
    task_future_t task_future;
    set<WebSocketConnectionPtr> conns;

    explicit Task(task_t&& _task,
                  task_future_t&& _task_future,
                  WebSocketConnectionPtr _conn)
        : task {std::move(_task)},
          task_future {std::move(_task_future)},
          conns {std::move(_conn)}
    {
        task->subscribe(this);
    }

    void add_conn(WebSocketConnectionPtr _conn)
    {
        conns.insert(std::move(_conn));
    }

    void remove_disconnected() 
    {
        for (auto it = conns.cbegin();
                it != conns.end();)
        {
            if ((*it)->disconnected())
            {
                it = conns.erase(it);
                cout << "removing disconneced conn" << endl;
            }
            else
            {
                ++it;
            }
        }
    }

    /**
     * Method class each time the task, i.e., source,
     * finds something. This way we are obtaining
     * current search results in real time and 
     * can send them immediently to the client
     *
     */
    void new_results(ISearchTask* source) override
    {
        static size_t counter {0};

        auto results = source->get_results();

        // sending found results to the client
        // this is async call, we dont care about 
        // its outcome/future, exceptions throw. 
        for (auto& conn: conns)
        {
             boost::fibers::fiber(
                   [conn = std::weak_ptr(conn),
                    jmsgs = results,
                    counter = counter++]() 
                   {
                       auto conn_ptr = conn.lock();

                       if (conn_ptr && conn_ptr->connected())
                       {
                           nl::json joined {{"msgs", jmsgs}};

                           conn_ptr->send(joined.dump());
                        }
                   }).detach();
        }
    } 
    
};

mutable boost::fibers::mutex m_tasks_mtx;

task_map_t m_tasks;

MicroCore const* m_core {nullptr}; 

};
}

#endif

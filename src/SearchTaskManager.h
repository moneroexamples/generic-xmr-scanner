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

using task_t = std::shared_ptr<ISearchTask>;  

using task_future_t = boost::fibers::future<void>;

using task_map_t = std::unordered_map<
                            std::string, 
                            std::unique_ptr<ISearchObserver>>;


/*
 * Representation of a task in our task_map
 * It will also act as observer for any search results
 * found by the task
 */
struct Task : public ISearchObserver
{
    task_t task;
    task_future_t task_future;
    
    list<std::pair<WebSocketConnectionPtr, 
                  size_t /* last index of message sent */
                      >> conns;

    explicit Task(task_t&& _task,
                  task_future_t&& _task_future,
                  WebSocketConnectionPtr _conn)
        : task {std::move(_task)},
          task_future {std::move(_task_future)},
          conns {make_pair(std::move(_conn), 0)}
    {
        task->subscribe(this);
    }

    void add_conn(WebSocketConnectionPtr _conn)
    {
        conns.push_back(make_pair(std::move(_conn),0));
    }

    void remove_disconnected() 
    {
        for (auto it = conns.cbegin();
                it != conns.end();)
        {
            auto& [conn, msg_idx] = *it;

            if (conn->disconnected())
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
     * This method is called each time the task
     * finds something. This way we are obtaining
     * current search results in real time and 
     * can send them immediently to the client
     */
    void new_results(ISearchTask* source) override
    {
        static size_t counter {0};

        // sending found results to the client
        for (auto& [conn, msg_idx]: conns)
        {
            // get copy of the current progress results starting
            // from the given msg_idx
            auto [progress, results] = source->get_results(msg_idx);
             
            // update msg index
            msg_idx += results.size(); 

            // append progress to results. results could be 
            // empty, thus we are just going to report 
            // current progress of the search
            results.push_back(std::move(progress));

            // send out the results/progress to the clients
            // using fiber. We dont check if the send was successful
            // or thrown exception. Just dispatch the fiber
            // hopeing that the results/progress messages
            // will get delivered to the clients 
            boost::fibers::fiber(
               [conn = std::weak_ptr(conn),
                jmsgs = std::move(results),
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



class SearchTaskManager
{

public:

explicit SearchTaskManager
    (MicroCore* _mcore, 
     FiberPool::FiberPoolSharing<>* _fpool)
    : m_core {_mcore}, m_fpool {_fpool}
{}


/**
 * Checks if tasks already exists. If not creates
 * Task instance, submits it to fiberpool 
 * and registers it with the task manager's m_tasks
 */
auto push(std::shared_ptr<ISearchTask>&& task, 
          WebSocketConnectionPtr const& ws_conn)
{

    static size_t task_no {0};

    assert(m_core != nullptr);
    
    task->t_no = ++task_no;

    task->set_microcore(m_core);
    
    auto key = task->key();

    // set message aggragate for the task to use
    
    std::lock_guard lk {m_tasks_mtx};
    
    // before submitting the task to fiberpool
    // and registring it with the task manager,
    // check if it already exists

    if (auto existing_it = m_tasks.find(key);
        existing_it != m_tasks.end())
    {
        LOG_INFO << "Tasks already exists\n";
        
        // task already exists, so return its iterator
        // and set bool flag as false to indicate that 
        // it has not been just created.
        // also ws_conn is added to the tasks's
        // connections lists, so that the resutls
        // from the task are also send to the new
        // connection
        
        auto existing_task_ptr = static_cast<Task*>(
                existing_it->second.get());

        existing_task_ptr->add_conn(ws_conn); 
        
        return make_tuple(existing_it, false);
    }
    
    // tasks does not exist, so we submit it to fiberpool

    // submit our job to the the pool
    auto opt_future = m_fpool->submit(
            [task = task]() 
            {
                // capture shared_ptr of the task by copy
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

    assert(success);
    
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
    
    { 
        ostringstream os;

        os << "running " << m_tasks.size() 
           << " tasks\n";

        cout << os.str() << flush; 
    }

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

        // check if the task is finished. it can be finished
        // either because it naturally ended, or an exception
        // was thrown inside of it 
        // in either case, we dont care how the task finished
        // we just remove it from the task map
        if (auto status = task_ptr->task_future.wait_for(0ms);
            status == boost::fibers::future_status::ready)
        {
            it = m_tasks.erase(it);

            //ostringstream os;

            //os << "deleted key " << key 
            //   << "from m_tasks\n";

            //cout << os.str() << flush; 
        }
        else
        {
            ++it;
        }
    }
}


void managment_loop()
{
    std::thread::id my_thread = std::this_thread::get_id(); 

    {
        std::ostringstream buffer;

        buffer << "managment_loop is on thread " 
               << my_thread << '\n';

        std::cout << buffer.str() << std::flush;
    }

    for(;;)
    {
        clean_up_tasks();
        boost::this_fiber::sleep_for(5s);
        std::thread::id new_thread 
            = std::this_thread::get_id();

         if (new_thread != my_thread) 
         {
            my_thread = new_thread;

            std::ostringstream buffer;

            buffer << "managment_loop switched to thread " 
                   << my_thread << '\n';

            std::cout << buffer.str() << std::flush;
         }

    }
}

auto status() const
{
    return jb().success(
        {
            {"network", m_core->get_nettype()},
            {"current_height", m_core->get_current_blockchain_height()}
        });
}


private:

mutable boost::fibers::mutex m_tasks_mtx;

task_map_t m_tasks;

MicroCore const* m_core {nullptr}; 
     
//FiberPool::FiberPoolStealing<>* m_fpool {nullptr};
FiberPool::FiberPoolSharing<>* m_fpool {nullptr};

};
}

#endif


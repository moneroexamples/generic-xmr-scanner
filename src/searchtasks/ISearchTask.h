#pragma once

//#include "nlohmann/json.hpp"

#include "src/MicroCore.h"
#include "src/Account.h"

#include "nlohmann/json.hpp"

#include <boost/fiber/all.hpp>

#include <atomic>
#include <list>
#include <string>


namespace xmreg
{


namespace nl = nlohmann; 

class ISearchTask;
/**
 * Basic interface for any class intrested
 * in being notified when SearchTasks find
 * something
 */
struct ISearchObserver
{
    virtual void new_results(ISearchTask* source) = 0;
};



/*
 * All search tasks need to have this interface
 * It also notifies all subscribed objects
 * about found results
 */
class ISearchTask 
{                                                                       
public:                                                                 
    uint64_t t_no {0};

    virtual std::string key() const = 0;
    virtual void operator()() = 0;                                      
    
    virtual void finish() 
    {
        std::lock_guard {m_mtx};
        m_should_finish = true;
    } 

    void notify(ISearchTask* source) 
    {
        if (results.empty())
            return;

        for (auto& obs: observers)
        {
            obs->new_results(source);
        }

        results.clear();
    }

    void subscribe(ISearchObserver* observer)
    {
        observers.push_back(observer);
    }

    void ubsubscribe(ISearchObserver* observer)
    {
        observers.remove(observer);
    }

    virtual std::vector<nl::json> get_results() 
    {
        return results;
    }

    void set_microcore(MicroCore const* _mcore) 
    {
        m_core = _mcore;
    }

    inline auto current_height() const
    {
        return m_core->get_current_blockchain_height() - 1;
    }
    
    virtual ~ISearchTask() = default;                                   

protected:
    boost::fibers::mutex m_mtx;
    bool m_should_finish {false};

    std::vector<nl::json> results;

    // holds pointers to any objects which wants
    // to be notified each time the search task
    // finds a new result
    //boost::fibers::mutex m_mtx2;
    std::list<ISearchObserver*> observers;

    MicroCore const* m_core {nullptr};
};

}

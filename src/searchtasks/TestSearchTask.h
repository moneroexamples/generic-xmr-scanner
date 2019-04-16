#pragma once

#include "ISearchTask.h"

#include "drogon/WebSocketController.h"

#include <boost/fiber/all.hpp>

#include <atomic>
#include <string>
#include <thread>

namespace xmreg
{

using namespace drogon;
using namespace std;

class TestSearchTask:  public ISearchTask 
{
public:

TestSearchTask(string address, string viewkey)
    : m_address {std::move(address)},
      m_viewkey {std::move(viewkey)}
{} 

std::string key() const override 
{
    return m_address;
}

void operator()() override
{

    auto counter {100u};
    //auto sleep_time = std::rand() % 5 + 1;
    auto sleep_time = 1;

    std::thread::id my_thread = std::this_thread::get_id(); 
    {
        std::ostringstream buffer;
        buffer << "fiber " << m_address << " started on thread " 
            << my_thread << '\n';
        std::cout << buffer.str() << std::flush;
    }

    while(counter --> 0)
    {
        if (m_should_finish)
        {
            std::cout << "finishing task" << std::endl;
            break;
        }

        //boost::this_fiber::yield();
        boost::this_fiber::sleep_for(chrono::seconds(sleep_time));
        std::thread::id new_thread = std::this_thread::get_id(); 
                
        if (new_thread != my_thread) 
        { /*< test if fiber was migrated to another thread >*/
            my_thread = new_thread;

            std::ostringstream buffer;
            buffer << "fiber " << m_address 
                   << " switched to thread " << my_thread << '\n';
            //std::cout << buffer.str() << std::flush;

			nl::json msg {{"found: ", buffer.str()}};
			//m_msg_aggr->push(key(), std::move(msg));
            results.push_back(std::move(msg));
        }
        {
            std::ostringstream buffer;
            buffer<< m_address << " counter: " << counter << '\n';
            //std::cout << buffer.str() << std::flush;
			
			nl::json msg {{"found: ", buffer.str()}};
            results.push_back(std::move(msg));
		//	m_msg_aggr->push(key(), std::move(msg));
        }

        notify(this);
    }

    {
        std::ostringstream buffer;
        buffer<< m_address << " search_task finished\n";
        std::cout << buffer.str() << std::flush;
		
		nl::json msg {{"found: ", buffer.str()}};
        results.push_back(std::move(msg));
        notify(this);
    }
}

protected:
string m_address {};
string m_viewkey {};

};


}

#pragma once

#include "../config/DefaultAddresses.h"

#include "drogon/HttpController.h"

namespace xmreg
{

using namespace drogon;
using namespace std;


class SearchTaskManager;

class InfoCtrl: public HttpController<InfoCtrl, false>
{
public:

	METHOD_LIST_BEGIN
	ADD_METHOD_TO(InfoCtrl::default_addresses, "/addresses", Get);
	ADD_METHOD_TO(InfoCtrl::status, "/status", Get);
	METHOD_LIST_END

	InfoCtrl(
		SearchTaskManager* task_manager,
        DefaultAddresses* default_addresses);

	virtual void
	status(HttpRequestPtr const& req,
		std::function<void (const HttpResponsePtr &)>&& 
			callback);
	
    virtual void
	default_addresses(HttpRequestPtr const& req,
		std::function<void (const HttpResponsePtr &)>&& 
			callback);

protected:

    virtual void 
    make_json_response(nl::json&& out_data,
       std::function<void (const HttpResponsePtr &)>&& 
           callback);

	SearchTaskManager* m_task_manager {nullptr};
    DefaultAddresses* m_default_addresses {nullptr};
};


}



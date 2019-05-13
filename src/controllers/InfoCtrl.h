#pragma once

#include "drogon/HttpController.h"

namespace xmreg
{

using namespace drogon;
using namespace std;


class SearchTaskManager;

class InfoCtrl: public HttpController<InfoCtrl, false>
{
public:

  //static void
  //initPathRouting() 
  //{
       //registerMethod(&InfoCtrl::status, 
                      //"/status", {Get}, false);
  //}

	METHOD_LIST_BEGIN
	ADD_METHOD_TO(InfoCtrl::status, "/status", Get);
	METHOD_LIST_END


	InfoCtrl(
		SearchTaskManager * const task_manager);

	virtual void
	status(HttpRequestPtr const& req,
		std::function<void (const HttpResponsePtr &)>&& 
			callback);

protected:

	SearchTaskManager* m_task_manager {nullptr};

};


}



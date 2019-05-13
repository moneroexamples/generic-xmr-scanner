
#include "InfoCtrl.h"

#include "../SearchTaskManager.h"

//#include "HttpResponse.h"


namespace xmreg
{

InfoCtrl::InfoCtrl(
    SearchTaskManager * const task_manager)
  : m_task_manager {task_manager}
{
	cout << "InfoCtrl" << endl;
}

void
InfoCtrl::status(HttpRequestPtr const& req,
   std::function<void (const HttpResponsePtr &)>&& callback)
{
    auto jstatus = m_task_manager->status();

	cout << "status" << jstatus.dump() << endl;
    
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
    resp->addHeader("Access-Control-Allow-Origin", "*");
    resp->setBody(jstatus.dump());

    callback(resp);
}

}

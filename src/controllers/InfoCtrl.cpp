
#include "InfoCtrl.h"

#include "../SearchTaskManager.h"

//#include "HttpResponse.h"


namespace xmreg
{

InfoCtrl::InfoCtrl(
    SearchTaskManager* task_manager,
    DefaultAddresses* default_addresses)
  : m_task_manager {task_manager},
    m_default_addresses {default_addresses}
{
}

void
InfoCtrl::status(HttpRequestPtr const& req,
   std::function<void (const HttpResponsePtr &)>&& callback)
{
    auto jstatus = m_task_manager->status();
    
    size_t network = jstatus.j["data"]["network"]; 

    jstatus.j["data"]["addresses"] 
        = m_default_addresses->get(network);

    make_json_response(std::move(jstatus), 
                       std::move(callback));
}

void
InfoCtrl::default_addresses(HttpRequestPtr const& req,
    std::function<void (const HttpResponsePtr &)>&& 
        callback)
{
    auto status = m_task_manager->status();
    size_t network = status.j["data"]["network"]; 
    auto addresses = m_default_addresses->get(network);
    
    if (addresses.empty())
    {
        make_json_response(
                jb().error("Cant read default addresses"), 
                std::move(callback));
        return;
    }

    make_json_response(jb().success(addresses), 
                       std::move(callback));
}

void 
InfoCtrl::make_json_response(nl::json&& out_data,
   std::function<void (const HttpResponsePtr &)>&& 
       callback)
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
    resp->addHeader("Access-Control-Allow-Origin", "*");
    resp->setBody(out_data.dump());

    callback(resp);
}

}

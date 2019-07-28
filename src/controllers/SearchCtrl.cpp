
#include "SearchCtrl.h"

#include "../SearchTaskManager.h"

namespace xmreg
{

using namespace drogon;
using namespace std;

namespace nl = nlohmann;

/**
 * Basic check of the submmited message
 */
std::optional<nl::json>
msg_to_json(string_view message)
{
     nl::json jmessage;

     try 
     {
        jmessage = nl::json::parse(message); 
     }
     catch (std::exception const& e)
     {
         LOG_DEBUG << e.what();
         return {};
     }

     if (jmessage.count("scanner") == 0)
     {
         LOG_DEBUG << "scanner field not given";
         return {};
     }

     if (!jmessage["scanner"].is_string())
     {
         LOG_DEBUG << "scanner value is not string";
         return {};
     }

     return std::move(jmessage);
}


void
send_and_close(WebSocketConnectionPtr const& ws_conn,
               string msg)
{
     LOG_ERROR << msg;
     ws_conn->send(jb().error(msg).dump());
     ws_conn->forceClose();
}


SearchWebSocketCtrl::SearchWebSocketCtrl(
        SearchTaskManager * const task_manager)
        : m_task_manager {task_manager}
{}

void 
SearchWebSocketCtrl::handleNewMessage(
        const WebSocketConnectionPtr & ws_conn,
        std::string&& message,
        WebSocketMessageType const& ws_msg_type) 
{ 
    switch (ws_msg_type)
    {
     case WebSocketMessageType::Text:
     {
         auto jmessage = msg_to_json(message);

         if (!jmessage) 
         {
             send_and_close(ws_conn, "Cant parse the request");
             return;
         }

         if ((*jmessage)["scanner"] != "outputs")
         {
             send_and_close(ws_conn, "Unknown scanner");
             return;
         }

         //cout << jmessage->dump() << endl;

         unique_ptr<ISearchTask> search_task;

         if (jmessage->contains("ringmembers")
                 && (*jmessage)["ringmembers"].get<bool>() == true)
         {
             cout << "Create InputSearchTask task\n";

             //search_task = make_task<InputSearchTask>(
                     //*jmessage, m_task_manager->config());
             
             search_task = make_task<InputSearchLiteTask>(
                     *jmessage, m_task_manager->config());
         }
         else
         {
             cout << "Create OutputSearchTask task\n";

             search_task = make_task<OutputSearchTask>(
                     *jmessage, m_task_manager->config());
         }

         if (!search_task)
         {
             send_and_close(ws_conn,
                            "SearchTask creation failed");
             return;
         }
         
         auto [it, success] = m_task_manager->push(
                                std::move(search_task),
                                ws_conn);

         if (!success) 
         {
             send_and_close(ws_conn, "Search task submittion failed");
         }

         break;
     }
     case WebSocketMessageType::Ping:
         break;
     default:
     {
         send_and_close(ws_conn, "Unexpected message");
     }
    }
}

void 
SearchWebSocketCtrl::handleConnectionClosed(
        const WebSocketConnectionPtr&) 
{
     LOG_DEBUG << "new websocket connection";
}

void 
SearchWebSocketCtrl::handleNewConnection(
        const HttpRequestPtr &, 
        const WebSocketConnectionPtr & ws_conn) 
{
    LOG_DEBUG << "new connection";
}
}

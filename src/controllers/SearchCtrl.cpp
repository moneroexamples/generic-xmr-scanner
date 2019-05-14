
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
     LOG_DEBUG << msg;
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
             send_and_close(ws_conn, "Unknonw scanner");
             return;
         }

         //cout << jmessage->dump() << endl;
             
         auto search_task = OutputSearchTask::create(*jmessage);

         if (!search_task)
         {
             send_and_close(ws_conn,
                            "OutputSearchTask creation failed");
             return;
         }
         
         auto [it, success] = m_task_manager->push(
                                std::move(search_task),
                                ws_conn);

         if (!success) 
         {
             LOG_ERROR << "Failed search task submission"; 
             ws_conn->send("Failed Search task submitttion");
             ws_conn->forceClose();
         }

         break;
     }
     case WebSocketMessageType::Ping:
     {
         //cout << "Got ping: ->" << message << "<-" << endl;
         ws_conn->send(message, WebSocketMessageType::Pong);
         break;
     }
     default:
     {
         cout << "Got unexpcted message: ->" 
              << message << "<-" << endl;
         ws_conn->send(message, WebSocketMessageType::Pong);
         ws_conn->forceClose();
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

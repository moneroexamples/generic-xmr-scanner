#ifndef SEARCHCTRL_H
#define SEARCHCTRL_H

#include "../searchtasks/OutputSearchTask.h"

namespace xmreg
{

using namespace drogon;
using namespace std;

namespace nl = nlohmann;

class SearchTaskManager;


class SearchWebSocketCtrl
    : public WebSocketController<SearchWebSocketCtrl, false>
{
public:

    inline static std::atomic<size_t> no_conn {0};

    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/search");
    WS_PATH_LIST_END

    explicit SearchWebSocketCtrl(
        SearchTaskManager * const task_manager);

    virtual void 
    handleNewMessage(const WebSocketConnectionPtr & ws_conn,
                     std::string&& message,
                     WebSocketMessageType const& ws_msg_type) override;

    virtual void 
    handleConnectionClosed(const WebSocketConnectionPtr&) override;

    virtual void 
    handleNewConnection(const HttpRequestPtr &, 
                        const WebSocketConnectionPtr & ws_conn) override;

protected:

    SearchTaskManager* m_task_manager {nullptr};
};


}
#endif

//
// Created by xiehaitao on 2022/1/15.
//

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include "workflow/Workflow.h"
#include "workflow/WFTaskFactory.h"
#include "workflow/WFServer.h"
#include "workflow/WFFacilities.h"
#include "K2Message.h"

using WFTutorialTask = WFNetworkTask<protocol::K2MessageRequest,
        protocol::K2MessageResponse>;
using WFTutorialServer = WFServer<protocol::K2MessageRequest,
        protocol::K2MessageResponse>;

using namespace protocol;


void process(WFTutorialTask *task)
{
    K2MessageRequest *req = task->get_req();
    K2MessageResponse *resp = task->get_resp();
    void *body;
    size_t size;
    size_t i;

    req->get_message_body_nocopy(&body, &size);
    for (i = 0; i < size; i++)
        ((char *)body)[i] = toupper(((char *)body)[i]);

    resp->set_message_body(body, size);
}


static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
    wait_group.done();
}

int main(int argc, char *argv[])
{
    unsigned short port;

    if (argc != 2)
    {
        fprintf(stderr, "USAGE %s <port>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);
    signal(SIGINT, sig_handler);

    struct WFServerParams params = SERVER_PARAMS_DEFAULT;
    params.request_size_limit = 4 * 1024;

    fprintf(stdout,"start server\n");
    WFTutorialServer server(&params, process);
    if (server.start(AF_INET6, port) == 0 ||
        server.start(AF_INET, port) == 0)
    {
        wait_group.wait();
        server.stop();
    }
    else
    {
        perror("server.start");
        exit(1);
    }

    return 0;
}

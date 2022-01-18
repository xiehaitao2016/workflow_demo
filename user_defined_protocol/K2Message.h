//
// Created by xiehaitao on 2022/1/15.
//

#ifndef CSROS_PERIPHERAL_K2MESSAGE_H
#define CSROS_PERIPHERAL_K2MESSAGE_H


#include <stdlib.h>
#include "workflow/ProtocolMessage.h"

namespace protocol {
    class K2Message : public ProtocolMessage
    {
    private:
        int encode(struct iovec vectors[], int max) override;
        int append(const void *buf, size_t size) override;

    public:
        int set_message_body(const void *body, size_t size);

        void get_message_body_nocopy(void **body, size_t *size)
        {
            *body = this->body;
            *size = this->body_size;
        }

    protected:
        char head[8];
        size_t head_received;
        char *body;
        size_t body_received;
        size_t body_size;

    public:
        K2Message()
        {
            this->head_received = 0;
            this->body = NULL;
            this->body_size = 0;
        }

        K2Message(K2Message&& msg) noexcept ;
        K2Message& operator = (K2Message&& msg);

        virtual ~K2Message()
        {
            free(this->body);
        }
    };


    using K2MessageRequest = K2Message;
    using K2MessageResponse = K2Message;
}


#endif //CSROS_PERIPHERAL_K2MESSAGE_H

//
// Created by xiehaitao on 2022/1/15.
//
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <utility>
#include "K2Message.h"

namespace protocol {

    int K2Message::encode(struct iovec *vectors, int max) {
        uint32_t n = htonl(this->body_size);

        memcpy(this->head, &n, 8);
        vectors[0].iov_base = this->head;
        vectors[0].iov_len = 8;
        vectors[1].iov_base = this->body;
        vectors[1].iov_len = this->body_size;

        return 2;	/* return the number of vectors used, no more then max. */
    }

    int K2Message::append(const void *buf, size_t size) {
        if (this->head_received < 8)
        {
            size_t head_left;
            void *p;

            p = &this->head[head_received];
            head_left = 8 - this->head_received;
            if (size < 8 - this->head_received)
            {
                memcpy(p, buf, size);
                this->head_received += size;
                return 0;
            }

            memcpy(p, buf, head_left);
            size -= head_left;
            buf = (const char *)buf + head_left;

            p = this->head;
            this->body_size = ntohl(*(uint32_t *)p);
            if (this->body_size > this->size_limit)
            {
                errno = EMSGSIZE;
                return -1;
            }

            this->body = (char *)malloc(this->body_size);
            if (!this->body)
                return -1;

            this->body_received = 0;
        }

        size_t body_left = this->body_size - this->body_received;

        if (size > body_left)
        {
            errno = EBADMSG;
            return -1;
        }

        memcpy(this->body, buf, size);
        if (size < body_left)
            return 0;

        return 1;
    }

    int K2Message::set_message_body(const void *body, size_t size) {
        void *p = malloc(size);

        if (!p)
            return -1;

        memcpy(p, body, size);
        free(this->body);
        this->body = (char *)p;
        this->body_size = size;

        this->head_received = 8;
        this->body_received = size;
        return 0;
    }

    K2Message::K2Message(K2Message &&msg) noexcept : ProtocolMessage(std::move(msg))
    {
        memcpy(this->head, msg.head, 8);
        this->head_received = msg.head_received;
        this->body = msg.body;
        this->body_received = msg.body_received;
        this->body_size = msg.body_size;

        msg.head_received = 0;
        msg.body = NULL;
        msg.body_size = 0;
    }

    K2Message &K2Message::operator=(K2Message &&msg) {
        if (&msg != this)
        {
            *(ProtocolMessage *)this = std::move(msg);

            memcpy(this->head, msg.head, 8);
            this->head_received = msg.head_received;
            this->body = msg.body;
            this->body_received = msg.body_received;
            this->body_size = msg.body_size;

            msg.head_received = 0;
            msg.body = NULL;
            msg.body_size = 0;
        }

        return *this;
    }
}


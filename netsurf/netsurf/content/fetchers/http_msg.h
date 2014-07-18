#ifndef HTTP_MSG_H
#define HTTP_MSG_H

struct http_msg {
// internal used by library, dont mess with them.
unsigned int socket;
unsigned int flags;
unsigned int write_ptr;
unsigned int buffer_length;
unsigned int chunk_ptr;
unsigned int timestamp;

// available for use.
unsigned int status;
unsigned int header_length;
char *content_ptr;
unsigned int content_length;
unsigned int content_received;
char header; //unknown size (actually, it's size is defined in header_length)
};

#endif

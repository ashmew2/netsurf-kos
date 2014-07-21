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

/* For the flags in the http_msg struct above  */
int FLAG_HTTP11             = 1 << 0
int FLAG_GOT_HEADER         = 1 << 1
int FLAG_GOT_ALL_DATA       = 1 << 2
int FLAG_CONTENT_LENGTH     = 1 << 3
int FLAG_CHUNKED            = 1 << 4
int FLAG_CONNECTED          = 1 << 5

/* ERROR flags go into the upper word */
int FLAG_INVALID_HEADER     = 1 << 16
int FLAG_NO_RAM             = 1 << 17
int FLAG_SOCKET_ERROR       = 1 << 18
int FLAG_TIMEOUT_ERROR      = 1 << 19
int FLAG_TRANSFER_FAILED    = 1 << 20

#endif

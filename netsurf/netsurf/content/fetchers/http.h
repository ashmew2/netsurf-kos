#include "http_msg.h"
#define __stdcall __attribute__((stdcall))

extern int (* __stdcall http_init)(void);
extern unsigned int (* __stdcall http_get) (char * url, char * add_head); 
extern int (* __stdcall http_receive) (unsigned int identifier);
extern void (* __stdcall http_free) (unsigned int identifier);
extern char * (* __stdcall http_find_header_field) (struct http_msg *http_ahoy, char *field_name);
extern char * (* __stdcall http_unescape_url) (char * url_asciiz);
extern char * (* __stdcall http_post) (char *url, char *headers, char *content_type, char *content_length);
extern int (* __stdcall http_send) (struct http_msg *handle, char *data, unsigned int length);

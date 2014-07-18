#define __stdcall __attribute__((stdcall))

extern struct http_msg;

extern int (* __stdcall http_init)();
extern unsigned int (* __stdcall http_get) (char * url, char * add_head); 
extern int (* __stdcall http_process) (unsigned int identifier);
extern void (* __stdcall http_free) (unsigned int identifier);
extern char * (* __stdcall http_find_header_field) (struct http_msg *http_ahoy, char *field_name);
extern unsigned int (* __stdcall http_unescape_url) (char *url_asciiz);

#include <menuet/os.h>
#define NULL 0
#define __stdcall __attribute__((stdcall))

extern int dll_load();
extern int mem_Free();
extern int mem_Alloc();
extern int mem_ReAlloc();



int kol_exit(){
	__menuet__sys_exit();
}

struct http_msg {
// internal used by library, dont mess with these.
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


int (* __stdcall http_init)();
// On the next line, we should tell the C compiler that this procedure actually returns a pointer. (to the http_msg struct)
unsigned int (* __stdcall http_get) (char * url, char * add_head); //yay, it's NOT uint, but hey, C is stubborn, and I'm dumb
int (* __stdcall http_process) (unsigned int identifier);
void (* __stdcall http_free) (unsigned int identifier);
char * (* __stdcall http_find_header_field) (char *field_name, struct http_msg *http_ahoy); //This is crazzzzzzyyyyyy
char * (* __stdcall http_unescape_url) (char *url_asciiz);

int HTTP_YAY(){
	asm volatile ("pusha\n\
			   movl $mem_Alloc, %eax\n\
			   movl $mem_Free, %ebx\n\
			   movl $mem_ReAlloc, %ecx\n\
			   movl $dll_load, %edx\n\
			   movl http_init, %esi\n\
			   call *%esi\n\
			   popa");
}

///===========================

/*This utility function should go to it's proper location once some progress is made : 
Puts a string from src when a newline/cl/rf/clrf/space/tab is encountered into dest
and null terminates dest
*/
// Assumes the dest is big enough to hold the string.

char *return_null_terminated_string(char *dest, char *src)
{
  int i = 0;
  int break_loop = 0;

  if(src == 0)
    return NULL;

  while(*src)
    {
      switch(*src)
	{
	case '\n':
	case '\r':
	case '\t':
	case ' ':
	  break_loop = 1;
	  break;

	default:
	  *(dest+i) = *src;
	  src++;
	  i++;
	  break;
	}

      if(break_loop)
	break;
    }
  *(dest+i) = '\0';

  return dest;
}

void HTTP_INIT()
{
IMP_ENTRY *imp;

imp = __kolibri__cofflib_load("/sys/lib/http.obj");
if (imp == NULL)
	kol_exit();

http_init = ( __stdcall  int(*)()) 
		__kolibri__cofflib_getproc (imp, "lib_init");
if (http_init == NULL)
  {
  __menuet__debug_out("http_init() is NULL. Exiting.\n");
  kol_exit();
  }

http_get = ( __stdcall  unsigned int (*)(char*)) 
		__kolibri__cofflib_getproc  (imp, "get");
if (http_get == NULL)
  {
    __menuet__debug_out("http_get() is NULL. Exiting.\n");
    kol_exit();
  }
http_free = ( __stdcall  void (*)(unsigned int)) 
		__kolibri__cofflib_getproc  (imp, "free");
if (http_free == NULL)
  {
    __menuet__debug_out("http_free() is NULL. Exiting.\n");
    kol_exit();
  }	
http_process = ( __stdcall  int (*)(unsigned int)) 
		__kolibri__cofflib_getproc  (imp, "process");
if (http_process == NULL)
  {
    __menuet__debug_out("http_process() is NULL. Exiting.\n");
    kol_exit();
  }

http_find_header_field = ( __stdcall  char *(*)(struct http_msg*, char *)) 
		__kolibri__cofflib_getproc  (imp, "find_header_field");
if (http_find_header_field == NULL)
  {
    __menuet__debug_out("http_find_header_field() is NULL. Exiting.\n");
    kol_exit();
  }

http_unescape_url = ( __stdcall char *(*)(char *))
		__kolibri__cofflib_getproc  (imp, "HTTP_unescape");
if(http_unescape_url == NULL)
  {
    __menuet__debug_out("http_unescape_url() is NULL. Exiting.\n");
    kol_exit();
  }

__menuet__debug_out("HTTP init...\n");
HTTP_YAY();

__menuet__debug_out("ok...\n");

}

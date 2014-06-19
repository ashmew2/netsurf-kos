/*
 * Copyright 2006 Daniel Silverstone <dsilvers@digital-scurf.org>
 * Copyright 2007 James Bursa <bursa@users.sourceforge.net>
 * Copyright 2003 Phil Mellor <monkeyson@users.sourceforge.net>
 *
 * This file is part of NetSurf.
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \file
 * Fetching of data from a URL (implementation).
 *
 * This implementation uses libcurl's 'multi' interface.
 *
 *
 * The CURL handles are cached in the curl_handle_ring. There are at most
 * ::max_cached_fetch_handles in this ring.
 */


#include "http.c"

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/stat.h>

#include <libwapcaplet/libwapcaplet.h>

#include "utils/config.h"
//#include <openssl/ssl.h>
#include "content/fetch.h"
#include "content/fetchers/curl.h"
#include "content/urldb.h"
#include "desktop/netsurf.h"
#include "desktop/options.h"
#include "utils/log.h"
#include "utils/messages.h"
#include "utils/schedule.h"
#include "utils/utils.h"
#include "utils/ring.h"
#include "utils/useragent.h"

/* BIG FAT WARNING: This is here because curl doesn't give you an FD to
 * poll on, until it has processed a bit of the handle.	 So we need schedules
 * in order to make this work.
 */
#include <desktop/browser.h>

#ifdef DBG
#undef DBG
#endif
//#define DBG(s) __menuet__debug_out(s) /* For the debug messages in BOARD */
#define DBG(s) LOG((s))            /* So that we see debug in Netsurf's LOG files */

#define MAX_REDIRECTIONS_ALLOWED 5
/* uncomment this to use scheduler based calling
#define FETCHER_CURLL_SCHEDULED 1
*/


struct fetch_curl_context {
	struct fetch_curl_context *r_next, *r_prev;

	struct fetch *fetchh; /**< Handle for this fetch */

	bool aborted; /**< Flag indicating fetch has been aborted */
	bool locked; /**< Flag indicating entry is already entered */

	nsurl *url; /**< The full url the fetch refers to */
	char *path; /**< The actual path to be used with open() */

	time_t file_etag; /**< Request etag for file (previous st.m_time) */
};

static struct fetch_curl_context *ring = NULL;


static bool fetch_curl_initialise(lwc_string *scheme); //here
static void fetch_curl_finalise(lwc_string *scheme); //here
static bool fetch_curl_can_fetch(const nsurl *url); //here
static void * fetch_curl_setup(struct fetch *parent_fetch, nsurl *url,
		 bool only_2xx, bool downgrade_tls, const char *post_urlenc,
		 const struct fetch_multipart_data *post_multipart,
		 const char **headers); //here
static bool fetch_curl_start(void *vfetch); //here 

static void fetch_curl_abort(void *vf); //here
static void fetch_curl_free(void *f); //here
static void fetch_curl_poll(lwc_string *scheme_ignored); //here


/**
 * Initialise the fetcher.
 *
 * Must be called once before any other function.
 */

void fetch_curl_register(void)
{

lwc_string *scheme;


LOG(("curl register\n"));

lwc_intern_string("http", SLEN("http"), &scheme);

	if (!fetch_add_fetcher(scheme,
				fetch_curl_initialise, //here
				fetch_curl_can_fetch, //here
				fetch_curl_setup,
				fetch_curl_start,
				fetch_curl_abort, //here
				fetch_curl_free, //here
#ifdef FETCHER_CURLL_SCHEDULED
				       NULL,
#else
				fetch_curl_poll, //here
#endif
				fetch_curl_finalise)) {  //here
			LOG(("Unable to register cURL fetcher for HTTP"));
		}
		
		
		lwc_intern_string("https", SLEN("https"), &scheme);

	if (!fetch_add_fetcher(scheme,
				fetch_curl_initialise,
				fetch_curl_can_fetch,
				fetch_curl_setup,
				fetch_curl_start,
				fetch_curl_abort,
				fetch_curl_free,
#ifdef FETCHER_CURLL_SCHEDULED
				       NULL,
#else
				fetch_curl_poll,
#endif
				fetch_curl_finalise)) {
			LOG(("Unable to register cURL fetcher for HTTPS"));
		}

}


/**
 * Initialise a cURL fetcher.
 */

bool fetch_curl_initialise(lwc_string *scheme)
{
LOG(("curl initi lwc\n"));
	return true; /* Always succeeds */
}


/**
 * Finalise a cURL fetcher
 */

void fetch_curl_finalise(lwc_string *scheme)
{
LOG(("curl finali\n"));
}

static bool fetch_curl_can_fetch(const nsurl *url)
{
	LOG(("curl can fetch\n"));
	return true; //let's lie a bit
}

/**
 * Start fetching data for the given URL.
 *
 * The function returns immediately. The fetch may be queued for later
 * processing.
 *
 * A pointer to an opaque struct curl_fetch_info is returned, which can be 
 * passed to fetch_abort() to abort the fetch at any time. Returns 0 if memory 
 * is exhausted (or some other fatal error occurred).
 *
 * The caller must supply a callback function which is called when anything
 * interesting happens. The callback function is first called with msg
 * FETCH_HEADER, with the header in data, then one or more times
 * with FETCH_DATA with some data for the url, and finally with
 * FETCH_FINISHED. Alternatively, FETCH_ERROR indicates an error occurred:
 * data contains an error message. FETCH_REDIRECT may replace the FETCH_HEADER,
 * FETCH_DATA, FETCH_FINISHED sequence if the server sends a replacement URL.
 *
 * Some private data can be passed as the last parameter to fetch_start, and
 * callbacks will contain this.
 */

void * fetch_curl_setup (struct fetch *fetchh,
		 nsurl *url,
		 bool only_2xx,
		 bool downgrade_tls,
		 const char *post_urlenc,
		 const struct fetch_multipart_data *post_multipart,
		 const char **headers)
{

	LOG(("curl setup\n"));
	
	struct fetch_curl_context *ctx;
	int i;

	ctx = calloc(1, sizeof(*ctx));
	if (ctx == NULL)
		return NULL;

	//ctx->path = url_to_path(nsurl_access(url));
	char *zz;
	int pr;
	nsurl_get(url, NSURL_WITH_FRAGMENT, &zz, &pr);
	
	ctx->path = zz;
	if (ctx->path == NULL) {
		free(ctx);
		return NULL;
	}

	ctx->url = nsurl_ref(url);


	ctx->fetchh = fetchh;

	RING_INSERT(ring, ctx);

	return ctx;
}


/**
 * Dispatch a single job
 */
bool fetch_curl_start(void *vfetch)
{
LOG(("curl start\n"));
	return true;
}

/**
 * Abort a fetch.
 */

void fetch_curl_abort(void *ctx)
{
	struct fetch_curl_context *c = ctx;

	/* To avoid the poll loop having to deal with the fetch context
	 * disappearing from under it, we simply flag the abort here.
	 * The poll loop itself will perform the appropriate cleanup.
	 */
	c->aborted = true;
}

/**
 * Free a fetch structure and associated resources.
 */

void fetch_curl_free(void *ctx)
{
	struct fetch_curl_context *c = ctx;
	nsurl_unref(c->url);
	free(c->path);
	RING_REMOVE(ring, c);
	free(ctx);
}

static inline bool fetch_curl_send_callback(const fetch_msg *msg,
		struct fetch_curl_context *ctx)
{
	ctx->locked = true;
	DBG("Inside fetch_curl_send_cb, Calling fetch_send_cb()\n");

	fetch_send_callback(msg, ctx->fetchh);
	ctx->locked = false;
	DBG("Returning from fetch_curl_send_cb.\n");

	return ctx->aborted;
}

/* fetch_curl_send_header function returns true if aborted. => true is for FAIL */

static bool fetch_curl_send_header(struct fetch_curl_context *ctx,
		const char *fmt, ...)
{
	fetch_msg msg;
	char header[64];
	va_list ap;
	DBG("Inside fetch_curl_send_header\n");
	va_start(ap, fmt);

	vsnprintf(header, sizeof header, fmt, ap);

	va_end(ap);
	DBG("Header is : ");
	DBG(header);

	msg.type = FETCH_HEADER;
	msg.data.header_or_data.buf = (const uint8_t *) header;
	msg.data.header_or_data.len = strlen(header);
	DBG("\nCalling fetch_curl_send_callback\n");

	fetch_curl_send_callback(&msg, ctx);

	DBG("Returning from fetch_curl_send_header\n");
	return ctx->aborted;
}

static void fetch_curl_process_error(struct fetch_curl_context *ctx, int code)
{
	fetch_msg msg;
	char buffer[1024];
	const char *title;
	char key[8];

	/* content is going to return error code */
	fetch_set_http_code(ctx->fetchh, code);

	/* content type */
	if (fetch_curl_send_header(ctx, "Content-Type: text/html"))
	  return;

	snprintf(key, sizeof key, "HTTP%03d", code);
	title = messages_get(key);

	snprintf(buffer, sizeof buffer, "<html><head><title>%s</title></head>"
			"<body><h1>%s</h1>"
			"<p>Error %d while fetching file %s</p></body></html>",
			title, title, code, nsurl_access(ctx->url));

	msg.type = FETCH_DATA;
	msg.data.header_or_data.buf = (const uint8_t *) buffer;
	msg.data.header_or_data.len = strlen(buffer);
	if (fetch_curl_send_callback(&msg, ctx))
	  return;

	msg.type = FETCH_FINISHED;
	fetch_curl_send_callback(&msg, ctx);

	return;
}

/**
 * Find the status code and content type and inform the caller.
 *
 * Return true if the fetch is being aborted.
 */
bool fetch_curl_process_headers(struct fetch_curl_context *ctx, struct http_msg *http_ahoy)

{
	long http_code;
	long error_code;
	fetch_msg msg;
	char *header_location_field = (char *)malloc(200);
	char result_str[12];
	
	//header_location_field = return_null_terminated_string(header_location_field, http_find_header_field(http_ahoy, "location"));
	
	/* f->had_headers = true; */

	/* if (!f->http_code) */
	/* { */
	/* 	code = curl_easy_getinfo(f->curl_handle, CURLINFO_HTTP_CODE, */
	/* 				 &f->http_code); */
	/* 	fetch_set_http_code(f->fetch_handle, f->http_code); */
	/* 	assert(code == CURL_OK); */
	/* } */

	http_code = http_ahoy->status;

	/*Print out http_code*/
	sprintf(result_str, "%u", http_ahoy->status);
	DBG("Process_headers : http_code = ");
	DBG(result_str);
	DBG("\n");
	/*                   */

	if (http_code == 304) /* && !f->post_urlenc && !f->post_multipart) */ {
		/* Not Modified && GET request */
	  DBG("FETCH_NOTMODIFIED\n");
	  msg.type = FETCH_NOTMODIFIED;
		fetch_send_callback(&msg, ctx->fetchh);
		return true;
	}

	/* handle HTTP redirects (3xx response codes) */

	if (300 <= http_code && http_code < 400) {
                header_location_field = 
                          return_null_terminated_string(header_location_field, 
                                                        http_find_header_field(http_ahoy, "location"));
		if(!header_location_field)
		  DBG("Found a 3xx, but location field is NULL. Error.\n");

	        DBG("FETCH_REDIRECT\n");
		msg.type = FETCH_REDIRECT;
		msg.data.redirect = header_location_field;
		fetch_send_callback(&msg, ctx->fetchh);
		return true;
	}

	/* handle HTTP 401 (Authentication errors) */
	if (http_code == 401) {
 	        DBG("FETCH_AUTH\n");	
		msg.type = FETCH_AUTH;
		/* TODO : FIX this line. We don't have a realm field in context right now*/
		//		msg.data.auth.realm = f->realm;
		fetch_send_callback(&msg, ctx->fetchh);
		return true;
	}

	/* handle HTTP errors (non 2xx response codes) */
	if (http_code < 200 || 299 < http_code) {
		DBG("FETCH_ERROR\n"); 
		msg.type = FETCH_ERROR;
		msg.data.error = messages_get("Not2xx");
		fetch_send_callback(&msg, ctx->fetchh);
		return true;
	}

	if (ctx->aborted)
		return true;

	DBG("Returning from process headers function\n");
	return false;
}

char *curl2_unescape( char * url , int length ) {
	
	DBG("CURL:unescape inside content--fetchers\n");
	return http_unescape_url(url);
}


static void fetch_curl_process(struct fetch_curl_context *ctx) {
	char ps[96], str[128];
	sprintf(ps, "Yay! Path is %s\n", ctx->path); /*TODO : Remove these notify calls soon. Floods screen otherwise*/
	execl ("/sys/@notify", ps, 0);
	
	DBG(ps);
	
	fetch_msg msg;

	DBG("AHOY!\n");
	struct http_msg *http_ahoy;

	unsigned int wererat = 0;
	char *pa=ctx->path;

	wererat = http_get(pa, NULL);	// TODO: a pointer to additional headers (for cookies etc) can be placed here in the future.

	if(wererat == 0) /* Error condition : http_get returned 0 */
	  {
		DBG("http_get() failed. [ Return Value 0 ]\n");
		ctx->aborted = true;
		return;
	  }
	else
		DBG("http_get() Succeeded!. [ Return Value Non zero ]\n");
	
	DBG("HTTP GOT!\n");
	int result = 1337;
	char result_str[12];
	char wererat_str[13];
	
	http_ahoy = (struct http_msg *)wererat;
	
	sprintf (str, "Header %u bytes, content %u bytes, received %u bytes\n", http_ahoy->header_length, http_ahoy->content_length, http_ahoy->content_received);
	DBG(str);	
        
	do  
	  result = http_process(wererat);
	while (result != 0);
	
	sprintf (str, "curl.c After http_process loop: Header %u bytes, content %u bytes, received %u bytes\n", http_ahoy->header_length, http_ahoy->content_length, http_ahoy->content_received);
	DBG(str);
	
	/* fetch is going to be successful */
	
	fetch_set_http_code(ctx->fetchh, http_ahoy->status);		


	
	/* Any callback can result in the fetch being aborted.
	 * Therefore, we _must_ check for this after _every_ call to
	 * fetch_file_send_callback().
	 */

	DBG("STATUS CODE: ");
	sprintf(result_str, "%u", http_ahoy->status);
	DBG(result_str);
	DBG("\n");

	DBG("Calling fetch_curl_process_headers\n");
	fetch_curl_process_headers(ctx, http_ahoy);

	if (fetch_curl_send_header(ctx, "Content-Type: %s",
			fetch_filetype(ctx->path)))
	  {
	DBG("Inside fetch file_process_aborted label\n");
	return;
	/* goto fetch_file_process_aborted; */
	  }

	/* main data loop */
	DBG("Starting of main data loop\n");

	msg.type = FETCH_DATA;
	msg.data.header_or_data.buf = http_ahoy->content_ptr; 	// lets pray this works..x2	
	msg.data.header_or_data.len = http_ahoy->content_received;

	DBG("Calling fetch_curl_send_callback\n");
		fetch_curl_send_callback(&msg, ctx);

	/* DBG("Content : "); */
	/* DBG(http_ahoy->content_ptr); */
	/* DBG("\n"); */

	DBG("Calling http_free with wererat = ");
	sprintf(wererat_str, "%u", wererat);
	DBG(wererat_str);
	DBG("\n");
	
	http_free(wererat);		
	wererat = 0;
	
	if (ctx->aborted == false) {
	DBG("ctx->aborted = false\n");
		msg.type = FETCH_FINISHED;
	DBG("Calling fetch_curl_send_callback\n");
		fetch_curl_send_callback(&msg, ctx);
	DBG("After Calling fetch_curl_send_callback\n");
	}
	
fetch_file_process_aborted:
	DBG("Inside fetch file_process_aborted label\n");
return;

}

/**
 * Do some work on current fetches.
 *
 * Must be called regularly to make progress on fetches.
 */

void fetch_curl_poll(lwc_string *scheme_ignored)
{
	LOG(("curl poll\n"));
	
	struct fetch_curl_context *c, *next;

	if (ring == NULL) return;

	/* Iterate over ring, processing each pending fetch */
	c = ring;
	do {
		/* Ignore fetches that have been flagged as locked.
		 * This allows safe re-entrant calls to this function.
		 * Re-entrancy can occur if, as a result of a callback,
		 * the interested party causes fetch_poll() to be called
		 * again.
		 */
		if (c->locked == true) {
			next = c->r_next;
			continue;
		}

		/* Only process non-aborted fetches */
		if (c->aborted == false) {
			/* file fetches can be processed in one go */
		  DBG("Calling fetch_curl_process()");
			fetch_curl_process(c);
		}

		/* Compute next fetch item at the last possible moment as
		 * processing this item may have added to the ring.
		 */
		DBG("Moving next to c->r_next\n");
		next = c->r_next;
		DBG("Calling fetch_remove_from_queue\n");
		fetch_remove_from_queues(c->fetchh);
		DBG("Calling fetch_free\n");
		fetch_free(c->fetchh);

		/* Advance to next ring entry, exiting if we've reached
		 * the start of the ring or the ring has become empty
		 */
	} while ( (c = next) != ring && ring != NULL);

}





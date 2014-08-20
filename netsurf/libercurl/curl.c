#include <curl/curl.h>
#include <menuet/os.h>
/*
char *curl_unescape( char * url , int length ) {
	
	DBG("CURL:unescape\n");
	return url;
	
}
*/
void curl_free( char * ptr ){
	DBG("CURL:free?\n");
}

char *curl_easy_unescape( CURL * curl , char * url , int inlength , int * outlength ){
	DBG("CURL:easyunescape!\n");
	*outlength=inlength;
	return url;
	
}

void curl_easy_cleanup(CURL * handle ) {
	DBG("CURL:cleanup\n");
}

CURL *curl_easy_init( ){
	DBG("CURL:init\n");
}

time_t curl_getdate(char * datestring , time_t *now ){
	DBG("CURL:getdate\n");
	return 0;
	
}



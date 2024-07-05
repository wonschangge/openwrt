#ifndef _MY_HTTP_H
#define _MY_HTTP_H
 
#define MY_HTTPS_DEFAULT_PORT 443
 
char * http_get(const char *url);
char * https_post(const char *url,const char * post_str);
void send_https_post_request(const char *url, const char *data);
char * curl_https_post(const char *url,const char *post_str);


#endif
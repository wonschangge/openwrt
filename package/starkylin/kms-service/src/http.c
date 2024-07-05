#include "kms-service.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <curl/curl.h>
#include "http.h"

#define BUFFER_SIZE 4096 // Connection: Keep-Alive\r\n
#define HTTP_POST "POST /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: C Client\r\nAccept: application/json\r\n" \
                  "Content-Type:application/json\r\nContent-Length: %ld\r\n\r\n%s"
#define HTTP_GET "GET /%s HTTP/1.1\r\nHost: %s:%d\r\nAccept: */*\r\n\r\n"

#define FAIL -1

SSL_CTX *InitCTX(void)
{
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms(); /* Load cryptos, et.al. */
    SSL_load_error_strings();     /* Bring in and register error messages */
    // ctx = SSL_CTX_new(TLSv1_2_client_method());   /* Create new context */
    ctx = SSL_CTX_new(TLS_client_method()); /* Create new context */
    if (ctx == NULL)
    {
        // ERR_print_errors_fp(stderr);
        printf("SSL_CTX_new:%d %s\n", errno, strerror(errno));
        // abort();
    }

    // SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
    // SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

    return ctx;
}

void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != NULL)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line); /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);      /* free the malloc'ed string */
        X509_free(cert); /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}

static int https_tcpclient_create(const char *host, int port)
{
    struct hostent *he;
    struct sockaddr_in server_addr;
    int socket_fd;

    if ((he = gethostbyname(host)) == NULL)
    {
        printf("gethostbyname failed\n");
        return -1;
    }

    char **pptr;
    char str[INET_ADDRSTRLEN];
    switch (he->h_addrtype)
    {
    case AF_INET:
        pptr = he->h_addr_list;
        for (; *pptr != NULL; pptr++)
        {
            printf("\taddress: %s\n",
                   inet_ntop(he->h_addrtype, he->h_addr_list[0], str, sizeof(str)));
        }
        break;
    default:
        printf("unknown address type\n");
        break;
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("creat socket failed\n");
        return -1;
    }

    bzero(&server_addr, sizeof(server_addr));

    /*
    if(inet_pton(AF_INET, "172.21.28.133", &server_addr.sin_addr) <= 0)
    {
        printf("convert server addr failed. errno=%d, errMsg=%s\n", errno, strerror(errno));
    }
    */

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]); // *(long*)(he->h_addr)

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("connect server failed. errno=%d, errMsg=%s\n", errno, strerror(errno));
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}

static void https_tcpclient_close(int socket)
{
    close(socket);
}

static int https_parse_url(const char *url, char *host, char *file, int *port)
{
    char *ptr1, *ptr2;
    int len = 0;
    if (!url || !host || !file || !port)
    {
        return -1;
    }

    ptr1 = (char *)url;

    if (!strncmp(ptr1, "https://", strlen("https://")))
    {
        ptr1 += strlen("https://");
    }
    else
    {
        return -1;
    }

    ptr2 = strchr(ptr1, '/');
    if (ptr2)
    {
        len = strlen(ptr1) - strlen(ptr2);
        memcpy(host, ptr1, len);
        host[len] = '\0';
        if (*(ptr2 + 1))
        {
            memcpy(file, ptr2 + 1, strlen(ptr2) - 1);
            file[strlen(ptr2) - 1] = '\0';
        }
    }
    else
    {
        memcpy(host, ptr1, strlen(ptr1));
        host[strlen(ptr1)] = '\0';
    }
    // get host and ip
    ptr1 = strchr(host, ':');
    if (ptr1)
    {
        *ptr1++ = '\0';
        *port = atoi(ptr1);
    }
    else
    {
        *port = MY_HTTPS_DEFAULT_PORT;
    }

    return 0;
}

static int https_tcpclient_recv(int socket, char *lpbuff)
{
    int recvnum = 0;

    recvnum = recv(socket, lpbuff, BUFFER_SIZE, 0);

    if (recvnum == 0)
    {
        printf("connection closed\n");
    }
    else if (recvnum < 0)
    {
        printf("recv failed:%d %s\n", errno, strerror(errno));
    }
    else
    {
        printf("Bytes received: %d\n", recvnum);
    }

    return recvnum;
}

static int https_tcpclient_send(int socket, char *buff, int size)
{
    int sent = 0, tmpres = 0;

    while (sent < size)
    {
        tmpres = send(socket, buff + sent, size - sent, 0);
        if (tmpres == -1)
        {
            return -1;
        }
        sent += tmpres;
    }
    printf("Bytes send: %d\n", sent);
    return sent;
}

static char *https_parse_result(const char *lpbuf)
{
    char *ptmp = NULL;
    char *response = NULL;
    ptmp = (char *)strstr(lpbuf, "HTTP/1.1");
    if (!ptmp)
    {
        printf("http/1.1 not faind\n");
        return NULL;
    }
    if (atoi(ptmp + 9) != 200)
    {
        printf("result:\n%s\n", lpbuf);
        return NULL;
    }

    ptmp = (char *)strstr(lpbuf, "\r\n\r\n");
    if (!ptmp)
    {
        printf("ptmp is NULL\n");
        return NULL;
    }
    response = (char *)malloc(strlen(ptmp) + 1);
    if (!response)
    {
        printf("malloc failed \n");
        return NULL;
    }
    strcpy(response, ptmp + 4);
    return response;
}

char *https_post(const char *url, const char *post_str)
{
    // char post[BUFFER_SIZE] = {'\0'};
    int socket_fd = -1;
    char lpbuf[BUFFER_SIZE] = {'\0'};
    // char *ptmp;
    char host_addr[1024] = {'\0'};
    char file[1024] = {'\0'};
    int port = 0;
    // int len=0;
    // char *response = NULL;
    SSL *ssl;
    SSL_CTX *ctx;
    int bytes;
    int ret = 0;

    if (!url || !post_str)
    {
        printf("input param invalid.url=%p, post_str=%p!\n", url, post_str);
        return NULL;
    }

    if (https_parse_url(url, host_addr, file, &port))
    {
        printf("http_parse_url failed!\n");
        return NULL;
    }
    printf("host_addr : %s\tfile:%s\t,%d\n", host_addr, file, port);
    sprintf(lpbuf, HTTP_POST, file, host_addr, strlen(post_str), post_str);
    printf("lpbuf %s\n", lpbuf);

    SSL_library_init(); /*load encryption and hash algo's in ssl*/
    ctx = InitCTX();
    ssl = SSL_new(ctx); /* create new SSL connection state */
    socket_fd = https_tcpclient_create(host_addr, port);

    if (socket_fd < 0)
    {
        printf("https_tcpclient_create failed\n");
        // https_tcpclient_close(socket_fd);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return NULL;
    }

    SSL_set_fd(ssl, socket_fd); /* attach the socket descriptor */
    ret = SSL_connect(ssl);
    if (ret != 1) /* perform the connection */
    {
        int error = SSL_get_error(ssl, ret);
        printf("SSL_connect:%d %d %s %d\n", ret, errno, strerror(errno), error);
        printf("%s\n", ERR_error_string(ERR_get_error(), 0));
        printf("%d\n", SSL_load_error_strings());

        switch (error)
        {
        case SSL_ERROR_SSL:
            printf("SSL ERROR SSL\n");
            break;
        case SSL_ERROR_SYSCALL:
            printf("SSL ERROR SYSCALL\n");
            break;
        case SSL_ERROR_WANT_ASYNC_JOB:
            printf("SSL ERROR WANT ASYNC_LOOKUP\n");
            break;
        case SSL_ERROR_WANT_ASYNC:
            printf("SSL ERROR WANT X509_LOOKUP\n");
            break;
        case SSL_ERROR_WANT_X509_LOOKUP:
            printf("SSL ERROR WANT X509_LOOKUP\n");
            break;
        case SSL_ERROR_WANT_WRITE:
            printf("SSL ERROR WANT WRITE\n");
            break;
        case SSL_ERROR_WANT_READ:
            printf("SSL ERROR WANT READ\n");
            break;
        case SSL_ERROR_ZERO_RETURN:
            printf("SSL ERROR SSL_ERROR_ZERO_RETURN\n");
            break;
        case SSL_ERROR_NONE:
            break;

        default:
            break;
        }
        SSL_free(ssl);
        https_tcpclient_close(socket_fd);
        SSL_CTX_free(ctx);
        return NULL;
    }
    else
    {
        printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl); /* get any certs */
        sprintf(lpbuf, HTTP_POST, file, host_addr, strlen(post_str), post_str);
        // printf("lpbuf %s\n", lpbuf);
        if (SSL_write(ssl, lpbuf, strlen(lpbuf)) < 0)
        {
            printf("SSL_write failed..\n");
            SSL_free(ssl);
            https_tcpclient_close(socket_fd);
            SSL_CTX_free(ctx);
            return NULL;
        }
        printf("send request:\n%s\n", lpbuf);

        bytes = SSL_read(ssl, lpbuf, sizeof(lpbuf));

        /*it's time to recv from server*/
        if (bytes <= 0)
        {
            printf("SSL_read failed\n");
            SSL_free(ssl);
            https_tcpclient_close(socket_fd);
            SSL_CTX_free(ctx);
            return NULL;
        }
    }

    lpbuf[bytes] = 0;

    SSL_shutdown(ssl);
    SSL_free(ssl);
    https_tcpclient_close(socket_fd);
    SSL_CTX_free(ctx);

    return https_parse_result(lpbuf);
}

char *http_get(const char *url)
{

    // char post[BUFFER_SIZE] = {'\0'};
    int socket_fd = -1;
    char lpbuf[BUFFER_SIZE] = {'\0'};
    // char *ptmp;
    char host_addr[1024] = {'\0'};
    char file[1024] = {'\0'};
    int port = 0;
    // int len=0;

    if (!url)
    {
        printf("      failed!\n");
        return NULL;
    }

    if (https_parse_url(url, host_addr, file, &port))
    {
        printf("http_parse_url failed!\n");
        return NULL;
    }
    // printf("host_addr : %s\tfile:%s\t,%d\n",host_addr,file,port);

    socket_fd = https_tcpclient_create(host_addr, port);
    if (socket_fd < 0)
    {
        printf("http_tcpclient_create failed\n");
        return NULL;
    }

    sprintf(lpbuf, HTTP_GET, file, host_addr, port);

    if (https_tcpclient_send(socket_fd, lpbuf, strlen(lpbuf)) < 0)
    {
        printf("http_tcpclient_send failed..\n");
        return NULL;
    }
    //	printf("发送请求:\n%s\n",lpbuf);

    if (https_tcpclient_recv(socket_fd, lpbuf) <= 0)
    {
        printf("http_tcpclient_recv failed\n");
        return NULL;
    }
    https_tcpclient_close(socket_fd);

    return https_parse_result(lpbuf);
}

struct MemoryStruct
{
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}

char *curl_https_post(const char *url, const char *post_str)
{
    CURL *curl;
    CURLcode res;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
    chunk.size = 0;           /* no data at this point */

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // 设置为 POST 方法
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // 准备要发送的数据
        // const char *post_data = "key1=value1&key2=value2";

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_str);

        // 跳过证书验证
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        // 设置 Content-Type 为 json
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else
        {
            log_debug("CURL Result: %s", chunk.memory);
        }

        // 释放内存
        // free(chunk.memory); //still need to use chunk.memory,free later

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return chunk.memory;
}
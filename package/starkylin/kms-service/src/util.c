#include "kms-service.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void remove_spaces_from_str(char *string)
{
    int non_space_count = 0;

    for (int i = 0; string[i] != '\0'; i++)
    {
        if (string[i] != ' ' && string[i] != '\t')
        {
            string[non_space_count] = string[i];
            non_space_count++;
        }
    }

    string[non_space_count] = '\0';
}

void strcpy_remove_spaces(char *dststr, const char *srcstr)
{
    int non_space_count = 0;

    for (int i = 0; srcstr[i] != '\0'; i++)
    {
        if (srcstr[i] != ' ' && srcstr[i] != '\t')
        {
            dststr[non_space_count] = srcstr[i];
            non_space_count++;
        }
    }

    dststr[non_space_count] = '\0';
}

int ifequal_skip_blank_space(const char *s1, const char *s2)
{
    skip_blank_space(s1);
    skip_blank_space(s2);
    while (*s1++ == *s2++)
    {
        skip_blank_space(s1);
        skip_blank_space(s2);
        if (*s1 == '\0' && *s2 == '\0')
            return (0);
    }
    return (1);
}

void init_logging(int level)
{
    log_set_level(level);

    FILE *logfp = fopen(LOG_PATH, "wb");
    if (NULL == logfp)
    {
        log_warn("open %s failed, please check!", LOG_PATH);
    }
    log_add_fp(logfp, level);
}

int handle_etc_hosts()
{
    FILE *fp = fopen(HOSTS_PATH, "r");
    if (NULL == fp)
    {
        log_error("%s not exist, please check!", HOSTS_PATH);
        return ENOENT;
    }

    int is_set_dapi = 0;
    int is_set_mqtt = 0;
    char buff[128] = {0};
    while (fgets(buff, 128, fp) != NULL)
    {
        if (is_set_dapi && is_set_mqtt)
            break;

        if (ifequal_skip_blank_space(buff, DAPI_HOST) == 0)
        {
            is_set_dapi = 1;
            continue;
        }
        if (ifequal_skip_blank_space(buff, MQTT_HOST) == 0)
        {
            is_set_mqtt = 1;
            continue;
        }
    }

    if (!is_set_dapi || !is_set_mqtt)
    {
        fp = fopen("/etc/hosts", "a+");
        if (NULL == fp)
        {
            log_error("Open /etc/hosts has no permissions, please check!");
            return EACCES;
        }
        fseek(fp, 0x0, SEEK_END);

        if (!is_set_dapi)
            fputs(DAPI_HOST, fp);
        if (!is_set_mqtt)
            fputs(MQTT_HOST, fp);
    }

    fclose(fp);
    return 0;
}

int parse_url(const char *url, char *host, int *port)
{
    char *ptr1, *ptr2;
    int len = 0;
    if (!url || !host || !port)
    {
        log_error("parameters error, %s: error occured! %d", __func__, __LINE__);
        return EINVAL;
    }

    ptr1 = (char *)url;
    char *tcp_addr_head = "tcp://";

    if (!strncmp(ptr1, tcp_addr_head, strlen(tcp_addr_head)))
        ptr1 += strlen(tcp_addr_head);
    else
    {
        log_error("no tcp address head found, %s: error occured! %d\n", __func__, __LINE__);
        return EIO;
    }

    ptr2 = strchr(ptr1, '/');
    if (ptr2)
    {
        len = strlen(ptr1) - strlen(ptr2);
        memcpy(host, ptr1, len);
        host[len] = '\0';
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
        *port = 1883;

    return 0;
}

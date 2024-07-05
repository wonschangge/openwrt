#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mosquitto.h>


typedef struct Device
{
	char device_id[64];
	char url[64];
	char topic[64];
	char username[64];
	char password[64];
} DeviceInfo;

extern char model_id[64];

extern DeviceInfo device_config;

typedef struct mosquitto mqttclient;

extern mqttclient *mosq;

/*
 * macros
 */
#define LOG_PATH "/tmp/kms-service.log"
#define HOSTS_PATH "/etc/hosts"
#define DAPI_HOST "172.21.28.133	dapi.starkylin.xyz\n"
#define MQTT_HOST "172.21.28.135	mqtt.starkylin.xyz\n"
#define REST_API_MODEL "https://dapi.starkylin.xyz/model"
#define REST_API_DEVICE_ACTIVATE "https://dapi.starkylin.xyz/device/activate"

#define skip_blank_space(str)                                                                           \
	while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\v' || *str == '\f' || *str == '\r') \
		str++;

/* 设备注册 - 认证 */
int device_auth();

char *curl_https_post(const char *url, const char *post_str);

/********************** mqtt *************************/
int connect_to_mqtt_server(char *server_ip, int port);
/* mosquitto错误码对应状态 */
char *stremosq_err(int __errnum);


/********************** 工具函数 **********************/
/*
 * Function: init_logging
 *
 * 初始化日志
 *
 * Parameters:
 *	level - 0TRACE/1DEBUG/2INFO/3WARN/4ERROR/5FATAL
 */
void init_logging(int level);
/**
 * 解析 /etc/hosts 并在缺省时添加 kms service backend domain
 *
 * 正常返回 0
 */
int handle_etc_hosts();

int parse_url(const char *url, char *host, int *port);


/* 去除字符串中的 空格/制表符 */
void remove_spaces_from_str(char *string);

/* 复制去除了空格/制表符的字符串 */
void strcpy_remove_spaces(char *dststr, const char *srcstr);

/* 检查两个字符串在忽略空白字符后是否相等 */
int ifequal_skip_blank_space(const char *s1, const char *s2);


#include "kms-service.h"
#include "data_parse.h"
#include <mosquitto.h>

void mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    log_info("connect to mqtt server ok");
    if (MOSQ_ERR_SUCCESS != mosquitto_subscribe(mosq, NULL, "terminals/10000000af8ce224", 0))
        log_warn("sub topic terminals/10000000af8ce224 failed...");
    else
        log_info("sub topic terminals/10000000af8ce224 successed...");
}

void mqtt_msg_callback(struct mosquitto *mosq,
                       void *userdata,
                       const struct mosquitto_message *message)
{
    int errno_code = 0;
    log_info("callback recv mqtt msg, topic = %s", message->topic);

    if ((errno_code = decode_msg((const char *)message->payload, strlen((const char *)message->payload))))
        log_warn("errno:%2d, %s", errno_code, strerror(errno_code));
}

void mqtt_sub_callback(struct mosquitto *mosq,
                       void *userdata,
                       int mid,
                       int qos_count,
                       const int *granted_qos)
{
    log_info("sub callback");
}

void mqtt_disconnect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    if (result)
        log_warn("disconnect %s\n", mosquitto_connack_string(result));
    else
        log_warn("disconnect from mqtt server.\n");
}

int connect_to_mqtt_server(char *server_ip, int port)
{
    int rc = 0;
    // char mqtt_user[128] = {0};
    // char mqtt_pwd[128] = {0};
    char client_id[128] = {0};
    // struct timeval tv;
    // gettimeofday(&tv, NULL);
    mosquitto_lib_init();
    // snprintf(client_id, sizeof(client_id), "test_%d", tv.tv_sec);
    snprintf(client_id, sizeof(client_id), "%s", "10000000af8ce224");
    log_info("connect to mqtt server..client_id=%s", client_id);
    mosq = mosquitto_new(client_id, true, NULL);

    if (mosq == NULL)
    {
        log_error("mosquitto_new error occured!");
        return ENODATA;
    }

#if 1
    rc = mosquitto_username_pw_set(mosq, device_config.username, (const char *)device_config.password);
    if (rc)
    {
        mosquitto_destroy(mosq);
        log_error("mosquitto_username_pw_set error occured!");
        return ENODATA;
    }
#endif

    mosquitto_connect_callback_set(mosq, mqtt_connect_callback);
    mosquitto_message_callback_set(mosq, mqtt_msg_callback);
    mosquitto_subscribe_callback_set(mosq, mqtt_sub_callback);
    mosquitto_disconnect_callback_set(mosq, mqtt_disconnect_callback);

    rc = mosquitto_connect(mosq, server_ip, port, 30);
    if (rc)
    {
        mosquitto_destroy(mosq);
        log_error("Unable to connect mqtt server rc=%d", rc);
        return ENOTCONN;
    }

    return 0;
}

char *stremosq_err(int __errnum)
{
    switch (__errnum)
    {
    case -4:
        return "MOSQ_ERR_AUTH_CONTINUE";
    case -3:
        return "MOSQ_ERR_NO_SUBSCRIBERS";
    case -2:
        return "MOSQ_ERR_SUB_EXISTS";
    case -1:
        return "MOSQ_ERR_CONN_PENDING";
    case 0:
        return "MOSQ_ERR_SUCCESS";
    case 1:
        return "MOSQ_ERR_NOMEM";
    case 2:
        return "MOSQ_ERR_PROTOCOL";
    case 3:
        return "MOSQ_ERR_INVAL";
    case 4:
        return "MOSQ_ERR_NO_CONN";
    case 5:
        return "MOSQ_ERR_CONN_REFUSED";
    case 6:
        return "MOSQ_ERR_NOT_FOUND";
    case 7:
        return "MOSQ_ERR_CONN_LOST";
    case 8:
        return "MOSQ_ERR_TLS";
    case 9:
        return "MOSQ_ERR_PAYLOAD_SIZE";
    case 10:
        return "MOSQ_ERR_NOT_SUPPORTED";
    case 11:
        return "MOSQ_ERR_AUTH";
    case 12:
        return "MOSQ_ERR_ACL_DENIED";
    case 13:
        return "MOSQ_ERR_UNKNOWN";
    case 14:
        return "MOSQ_ERR_ERRNO";
    case 15:
        return "MOSQ_ERR_EAI";
    case 16:
        return "MOSQ_ERR_PROXY";
    case 17:
        return "MOSQ_ERR_PLUGIN_DEFER";
    case 18:
        return "MOSQ_ERR_MALFORMED_UTF8";
    case 19:
        return "MOSQ_ERR_KEEPALIVE";
    case 20:
        return "MOSQ_ERR_LOOKUP";
    case 21:
        return "MOSQ_ERR_MALFORMED_PACKET";
    case 22:
        return "MOSQ_ERR_DUPLICATE_PROPERTY";
    case 23:
        return "MOSQ_ERR_TLS_HANDSHAKE";
    case 24:
        return "MOSQ_ERR_QOS_NOT_SUPPORTED";
    case 25:
        return "MOSQ_ERR_OVERSIZE_PACKET";
    case 26:
        return "MOSQ_ERR_OCSP";
    case 27:
        return "MOSQ_ERR_TIMEOUT";
    case 28:
        return "MOSQ_ERR_RETAIN_NOT_SUPPORTED";
    case 29:
        return "MOSQ_ERR_TOPIC_ALIAS_INVALID";
    case 30:
        return "MOSQ_ERR_ADMINISTRATIVE_ACTION";
    case 31:
        return "MOSQ_ERR_ALREADY_EXISTS";
    }
    return "UNKNOWN MOSQ_ERR, Please Check!";
}

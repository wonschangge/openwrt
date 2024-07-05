#include "KmsCore.pb.h"
#include "kms-service.h"
#include "data_parse.h"
#include <json/json.h>
#include <mosquitto.h>
#include <string>
#include <codecvt>
#include <locale>
#include <errno.h>

int mqtt_subscribe(const char *topic)
{
    return mosquitto_subscribe(mosq, NULL, topic, 0);
}

int mqtt_unsubscribe(const char *topic)
{
    return mosquitto_unsubscribe(mosq, NULL, topic);
}

int handle_mqtt_notification_msg(proto::MessageInfo info)
{
    const google::protobuf::Any any = info.data();
    proto::Notification notification;
    if (!any.UnpackTo(&notification))
        log_warn("Failed to unpack Any to Notification.");
    else
        log_info("notification title: %s", notification.content_title()); // string

    return 0;
}

int handle_mqtt_update_system_msg(proto::MessageInfo info)
{
    const google::protobuf::Any any = info.data();
    proto::DeviceUpdate update;
    if (!any.UnpackTo(&update))
        log_warn("Failed to unpack Any to UPDATE_SYSTEM.");
    else
        log_info("update note: %s", update.release_note()); // string

    return 0;
}

int handle_mqtt_log_report_msg(proto::MessageInfo info)
{
    const google::protobuf::Any any = info.data();
    proto::LogCtrl log_ctrl;
    if (!any.UnpackTo(&log_ctrl))
        log_warn("Failed to unpack Any to LOG_REPORT.");
    else
        log_info("log level: %ld", log_ctrl.log_level()); // int32

    return 0;
}

int handle_mqtt_security_msg(proto::MessageInfo info)
{
    const google::protobuf::Any any = info.data();
    proto::SecurityInfo security_info;
    if (!any.UnpackTo(&security_info))
        log_warn("Failed to unpack Any to SECURITY.");

    // encode_msg();
    Json::Reader reader;
    Json::Value value;
    if (reader.parse(security_info.policies(), value))
    {
        log_info("security policy: %s", value.toStyledString().c_str());

        if (value.isMember("reboot_device_timeout"))
        {
            log_warn("reboot test.");
            system("reboot");
        }
        else if (value.isMember("power_off_timeout"))
        {
            log_warn("poweroff test.");
            system("poweroff");
        }
        else if (value.isMember("wipe_data_timeout"))
        {
            log_warn("wipe_data_timeout test.");
            system("firstboot -y && reboot");
        }
        else if (value.isMember("no_led") && value["no_led"].isBool())
        {
            int control = value["no_led"].asBool();
            log_warn("no_led test.%d", control);
            if (control)
            {
                // uci set system.@led[0]=led
                // uci set system.@led[0].sysfs='PWR'
                // uci set system.@led[0].trigger='none'
                // uci set system.@led[0].default='0'
                // uci set system.@led[1]=led
                // uci set system.@led[1].sysfs='ACT'
                // uci set system.@led[1].trigger='none'
                // uci set system.@led[1].default='0'
                // uci commit system
                /// etc/init.d/led reload
                FILE *fp = popen("/sbin/uci add system led; /sbin/uci set system.@led[0]=led; /sbin/uci set system.@led[0].sysfs='PWR'; /sbin/uci set system.@led[0].trigger='none';"
                                 "/sbin/uci set system.@led[0].default='0'; /sbin/uci add system led; /sbin/uci set system.@led[1]=led; /sbin/uci set system.@led[1].sysfs='ACT';"
                                 "/sbin/uci set system.@led[1].trigger='none'; /sbin/uci set system.@led[1].default='0'; /sbin/uci commit system;"
                                 "/etc/init.d/led restart",
                                 "r");
                if (fp == NULL)
                {
                    log_error("Failed to run command.");
                    return ENOEXEC;
                }
                // 读取命令的输出（如果有的话）
                char buffer[128];
                while (fgets(buffer, sizeof(buffer), fp) != NULL)
                    log_info("led: %s", buffer);
                pclose(fp);
            }
            else
            {
                // uci set system.@led[0]=led
                // uci set system.@led[0].sysfs='PWR'
                // uci set system.@led[0].trigger='heartbeat'
                // uci set system.@led[0].default='1'
                // uci set system.@led[1]=led
                // uci set system.@led[1].sysfs='ACT'
                // uci set system.@led[1].trigger='heartbeat'
                // uci set system.@led[1].default='1'
                // uci commit system
                /// etc/init.d/led reload
                FILE *fp = popen("/sbin/uci add system led; /sbin/uci set system.@led[0]=led; /sbin/uci set system.@led[0].sysfs='PWR'; /sbin/uci set system.@led[0].trigger='heartbeat';"
                                 "/sbin/uci set system.@led[0].default='1'; /sbin/uci add system led; /sbin/uci set system.@led[1]=led; /sbin/uci set system.@led[1].sysfs='ACT';"
                                 "/sbin/uci set system.@led[1].trigger='heartbeat'; /sbin/uci set system.@led[1].default='1'; /sbin/uci commit system;"
                                 "/etc/init.d/led restart",
                                 "r");
                if (fp == NULL)
                {
                    log_error("Failed to run command.");
                    return ENOEXEC;
                }
                // 读取命令的输出（如果有的话）
                char buffer[128];
                while (fgets(buffer, sizeof(buffer), fp) != NULL)
                    log_info("led: %s", buffer);
                pclose(fp);
            }
        }
    }

    return 0;
}

int handle_mqtt_download_msg(proto::MessageInfo info)
{
    int errno_code = 0;
    const google::protobuf::Any any = info.data();
    proto::Download download;
    if (!any.UnpackTo(&download))
        log_warn("Failed to unpack Any to DOWNLOAD.");

    Json::Reader reader;
    Json::Value value;

    switch (download.event_type())
    {
    case proto::ADD_GROUP:
        if (reader.parse(download.json_str(), value))
        {
            std::string group_id = value["groupId"].asString();
            std::string group_name = value["groupName"].asString();
            log_info("download EventType: %d,  Json: %s", download.event_type(), value.toStyledString().c_str()); // string
            log_info("group_name: %s", group_name.c_str());                                                       // string
            errno_code = mqtt_subscribe(("group/" + std::string(group_id)).c_str());
        }
        break;
    case proto::DELETE_GROUP:
        if (reader.parse(download.json_str(), value))
        {
            std::string group_id = value["groupId"].asString();
            log_info("download EventType: %d,  Json: %s", download.event_type(), value.toStyledString().c_str()); // string
            errno_code = mqtt_unsubscribe(("group/" + std::string(group_id)).c_str());
        }
        break;
    case proto::PUSH_FILE:
        if (reader.parse(download.json_str(), value))
        {
            std::string original = value["original"].asString();
            std::string downloadUrl = value["downloadUrl"].asString();
            Json::Value downloadParams;
            downloadParams["task_num"] = 2;
            downloadParams["uri"] = downloadUrl;
            downloadParams["path"] = "./";
            downloadParams["file_name"] = original;
            log_info("download EventType: %d,  Json: %s", download.event_type(), downloadParams.toStyledString().c_str()); // string
            // load_download(downloadParams.toStyledString().c_str());
        }
        break;
    case proto::REMOVE_FILE:
        if (reader.parse(download.json_str(), value))
        {
            log_info("download EventType: %d,  Json: %s", download.event_type(), value.toStyledString().c_str()); // string
            // load_remove(download.json_str().c_str());
        }
        break;
    default:
        log_warn("Failed to unpack Any to nothing.");
        break;
    }

    return errno_code;
}

int decode_msg(const char *data, const size_t len)
{
    int errno_code = 0;

    proto::MessageInfo info;
    if (!info.ParseFromArray(data, len)) // 反序列化 MessageInfo
    {
        log_error("Failed to parse binary data into MessageInfo.");
        return EIO;
    }

    log_debug("MessageInfo :\n%s", info.Utf8DebugString().c_str());
    log_info("Message Type: %d", info.type());       // enum
    log_info("Message ID: %lld", info.msg_id());     // int64
    log_info("Message Time: %lld", info.msg_time()); // int64

    switch (info.type())
    {
    case proto::NOTIFICATION:
        errno_code = handle_mqtt_notification_msg(info);
        break;
    case proto::UPDATE_SYSTEM:
        errno_code = handle_mqtt_update_system_msg(info);
        break;
    case proto::LOG_REPORT:
        errno_code = handle_mqtt_log_report_msg(info);
        break;
    case proto::SECURITY:
        errno_code = handle_mqtt_security_msg(info);
        break;
    case proto::DOWNLOAD:
        errno_code = handle_mqtt_download_msg(info);
        break;
    default:
        log_warn("Failed to unpack Any to nothing.");
        break;
    }

    return errno_code;
}

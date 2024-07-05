#include "kms-service.h"
#include "cJSON.h"

typedef struct Register
{
    char type[64];
    char description[64];
    char code[64];
    char label[64];
    char model[64];
    char platform[64];
    char cpuarch[64];
} RegisterInfo, *pRegisterInfo;

typedef struct Activate
{
    char sn[64];
    char version[64];
    char label[64];
    char modelId[64];
} ActivateInfo, *pActivateInfo;

int register_device(const RegisterInfo *info)
{
    if (!info)
    {
        log_error("[%s %n]invalid param info=%p", __func__, __LINE__, info);
        return EINVAL;
    }

    cJSON *reqModelParamsRoot = cJSON_CreateObject();

    if (NULL == reqModelParamsRoot)
    {
        log_error("[%s %n]invalid param reqModelParamsRoot=%p", __func__, __LINE__, reqModelParamsRoot);
        return EINVAL;
    }

    cJSON_AddStringToObject(reqModelParamsRoot, "type", info->type);
    cJSON_AddStringToObject(reqModelParamsRoot, "description", info->description);
    cJSON_AddStringToObject(reqModelParamsRoot, "code", info->code);
    cJSON_AddStringToObject(reqModelParamsRoot, "label", info->label);
    cJSON_AddStringToObject(reqModelParamsRoot, "model", info->model);
    cJSON_AddStringToObject(reqModelParamsRoot, "platform", info->platform);
    cJSON_AddStringToObject(reqModelParamsRoot, "cpuArch", info->cpuarch);

    char *reqModelParams = cJSON_Print(reqModelParamsRoot);
    cJSON_Delete(reqModelParamsRoot);
    log_info("request REST:model params: %s", reqModelParams);

    // https://restlet.com/modules/client  https://dapi.starkylin.xyz/model
    char *resModelInfoStr = curl_https_post(REST_API_MODEL, reqModelParams); // resModelInfoStr need to free
    if (NULL == resModelInfoStr)
    {
        if (NULL != reqModelParams)
            free(reqModelParams);
        return -1;
    }

    cJSON *resModelInfo = cJSON_Parse(resModelInfoStr);

    if (NULL == resModelInfo)
    {
        log_error("invalid param resModelInfo=%p", resModelInfo);
        if (NULL != reqModelParams)
            free(reqModelParams);

        if (NULL != resModelInfoStr)
            free(resModelInfoStr);
        return -1;
    }

    log_info("response REST:model info: %s", cJSON_Print(resModelInfo));
    strcpy(model_id, cJSON_GetObjectItem(resModelInfo, "data")->valuestring);
    cJSON_Delete(resModelInfo);

    if (NULL != reqModelParams)
        free(reqModelParams);

    if (NULL != resModelInfoStr)
        free(resModelInfoStr);

    return 0;
}

int activate_device(const char *model_id, const ActivateInfo *info)
{
    if (!info || !model_id)
    {
        log_error("invalid param model_id=%p info=%p", model_id, info);
        return EINVAL;
    }

    cJSON *reqDeviceActivateParamsRoot = cJSON_CreateObject();

    if (NULL == reqDeviceActivateParamsRoot)
    {
        log_error("invalid param reqDeviceActivateParamsRoot=%p", reqDeviceActivateParamsRoot);
        return EINVAL;
    }

    cJSON_AddStringToObject(reqDeviceActivateParamsRoot, "sn", info->sn);
    cJSON_AddStringToObject(reqDeviceActivateParamsRoot, "version", info->version);
    cJSON_AddStringToObject(reqDeviceActivateParamsRoot, "label", info->label);
    cJSON_AddStringToObject(reqDeviceActivateParamsRoot, "modelId", model_id);

    char *reqDeviceActivateParams = cJSON_Print(reqDeviceActivateParamsRoot);
    log_info("request REST:device/activate params: %s", reqDeviceActivateParams);

    cJSON_Delete(reqDeviceActivateParamsRoot);

    char *resDeviceInfoStr = curl_https_post(REST_API_DEVICE_ACTIVATE, reqDeviceActivateParams); // resDeviceInfoStr need to free
    log_debug("activate DeviceInfo: %s", resDeviceInfoStr);

    cJSON *resDeviceInfo = cJSON_Parse(resDeviceInfoStr);

    if (NULL == resDeviceInfo)
    {
        if (NULL != reqDeviceActivateParams)
            free(reqDeviceActivateParams);
        if (NULL != resDeviceInfoStr)
            free(resDeviceInfoStr);
        log_error("invalid param resDeviceInfo=%p", resDeviceInfo);
        return EINVAL;
    }

    log_info("response REST:device/activate info: %s", cJSON_Print(resDeviceInfo));
    if (cJSON_GetObjectItem(resDeviceInfo, "success")->valueint)
    {
        cJSON *object = cJSON_GetObjectItem(resDeviceInfo, "data");
        strcpy(device_config.device_id, cJSON_GetObjectItem(object, "deviceId")->valuestring);
        strcpy(device_config.url, cJSON_GetObjectItem(object, "url")->valuestring);
        strcpy(device_config.topic, cJSON_GetObjectItem(object, "topic")->valuestring);
        strcpy(device_config.username, cJSON_GetObjectItem(object, "username")->valuestring);
        strcpy(device_config.password, cJSON_GetObjectItem(object, "password")->valuestring);
    }
    else
    {
        cJSON_Delete(resDeviceInfo);
        if (NULL != reqDeviceActivateParams)
            free(reqDeviceActivateParams);
        if (NULL != resDeviceInfoStr)
            free(resDeviceInfoStr);
        log_error("parse received activate json data:unsuccessed\n");
        return EIO;
    }

    cJSON_Delete(resDeviceInfo);
    if (NULL != reqDeviceActivateParams)
        free(reqDeviceActivateParams);
    if (NULL != resDeviceInfoStr)
        free(resDeviceInfoStr);

    return 0;
}

int device_auth()
{
    int errno_code = 0;

    const char *type = "iot"; // 设备类型
    const char *description = "aosp_blueline-userdebug";
    const char *code = "Raspberry Pi";
    const char *label = "aosp_blueline";
    const char *model = "blueline";
    const char *platform = "Linux-aarch64";
    const char *cpu_arch = "arm64";
    const char *sn = "10000000af8ce224";
    const char *version = "V1.0.0";

    RegisterInfo *pRegisterInfo = (RegisterInfo *)malloc(sizeof(RegisterInfo));
    if (NULL == pRegisterInfo)
    {
        log_error("fail to malloc pRegisterInfo");
        return ENOMEM;
    }

    memset(pRegisterInfo, 0x0, sizeof(RegisterInfo));

    strncpy(pRegisterInfo->type, type, strlen(type) < 64 ? strlen(type) : 63);
    strncpy(pRegisterInfo->description, description, strlen(description) < 64 ? strlen(description) : 63);
    strncpy(pRegisterInfo->code, code, strlen(code) < 64 ? strlen(code) : 63);
    strncpy(pRegisterInfo->label, label, strlen(label) < 64 ? strlen(label) : 63);
    strncpy(pRegisterInfo->model, model, strlen(model) < 64 ? strlen(model) : 63);
    strncpy(pRegisterInfo->platform, platform, strlen(platform) < 64 ? strlen(platform) : 63);
    strncpy(pRegisterInfo->cpuarch, cpu_arch, strlen(cpu_arch) < 64 ? strlen(cpu_arch) : 63);

    errno_code = register_device((const RegisterInfo *)pRegisterInfo);

    if (NULL != pRegisterInfo)
        free(pRegisterInfo);

    if (EINVAL == errno_code)
    {
        log_error("fail to register_device\n");
        return EINVAL;
    }

    ActivateInfo *pActivateInfo = (ActivateInfo *)malloc(sizeof(ActivateInfo));
    if (NULL == pActivateInfo)
    {
        log_error("fail to malloc pActivateInfo\n");
        return ENOMEM;
    }

    memset(pActivateInfo, 0x0, sizeof(ActivateInfo));

    strncpy(pActivateInfo->sn, sn, strlen(sn) < 64 ? strlen(sn) : 63);
    strncpy(pActivateInfo->version, version, strlen(version) < 64 ? strlen(version) : 63);
    strncpy(pActivateInfo->label, label, strlen(label) < 64 ? strlen(label) : 63);
    strncpy(pActivateInfo->modelId, model_id, strlen(model_id) < 64 ? strlen(model_id) : 63);

    errno_code = activate_device(model_id, pActivateInfo);

    if (NULL != pActivateInfo)
        free(pActivateInfo);

    if (errno_code)
    {
        log_error("fail to activate_device\n");
        return errno_code;
    }

    return 0;
}

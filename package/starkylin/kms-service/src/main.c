#include "kms-service.h"

/* Global Data */

char model_id[64] = {0};

DeviceInfo device_config = {0};

mqttclient *mosq = NULL;

int main()
{
	int errno_code = 0;
	int mosq_err_code = 0;

	char host[64] = {0};
	int port = 0;

	init_logging(0);
	if ((errno_code = handle_etc_hosts()))
		goto err;
	if ((errno_code = device_auth()))
		goto err;
	if ((errno_code = parse_url(device_config.url, host, &port)))
		goto err;
	log_info("%s:%d", host, port);
	if ((errno_code = connect_to_mqtt_server(host, port)))
		goto err;
	if ((mosq_err_code = mosquitto_loop_forever(mosq, -1, 1)))
		log_error("mosq_err:%2d, %s", mosq_err_code, stremosq_err(mosq_err_code));

	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return mosq_err_code;

err:
	log_error("errno:%2d, %s", errno_code, strerror(errno_code));
	return errno_code;
}
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"
#include "lib/sensors.h"

#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

extern coap_resource_t gas_sensor;

PROCESS(coap_server_process, "COAP server process");
AUTOSTART_PROCESSES(&coap_server_process);

PROCESS_THREAD(coap_server_process, ev, data)
{
    PROCESS_BEGIN();

    printf("CoAP server has started...");

    adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);
    printf("Loading sesnsors...");

    PROCESS_PAUSE();

    coap_activate_resource(&gas_sensor, "gas_sensor");

    while(1)
    {
            PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}

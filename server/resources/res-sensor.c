/* Standard library */
#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/relay.h"
#include "lib/sensors.h"
#if CONTIKI_TARGET_ZOUL
/* Sensors */
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
#else
/* Assumes Z1 mote */
#include "dev/z1-phidgets.h"
#endif

/* CoAP engine */
#include "coap-engine.h"
/* A counter to keep track of the number of sent messages */
static int counter = 0;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/* Define the resource */
RESOURCE(res_all_sensors, "title=\"Gas,  LED Status \";rt=\"JSON\"", res_get_handler, NULL, NULL, NULL);

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

#if CONTIKI_TARGET_ZOUL
    adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);
#else /* Assumes Z1 mote */
    SENSORS_ACTIVATE(phidgets);
	SENSORS_ACTIVATE(relay);
#endif

#if CONTIKI_TARGET_ZOUL

    int gas_sensor_reading = adc_zoul.value(ZOUL_SENSORS_ADC3);
    printf("gas_sensor_reading ADC3 = %u mV\n", gas_sensor_reading);
   
    int gas_sensor_threshold = 800;
    bool condition = (gas_sensor_reading >= gas_sensor_threshold);
    if (condition)
    {
        leds_toggle(LEDS_RED);
		relay.value(RELAY_ON);
		printf("\nRelay: switch should be ON --> %u\n", relay.status(SENSORS_ACTIVE));
    }
    else
    {
        leds_toggle(LEDS_GREEN);
		relay.value(RELAY_OFF);
		printf("Relay: switch should be OFF --> %u\n\n", relay.status(SENSORS_ACTIVE));
    }
#else
    printf("Phidget 5V 1:%d\n", phidgets.value(PHIDGET5V_1));
    printf("Phidget 5V 2:%d\n", phidgets.value(PHIDGET5V_2));
    printf("Phidget 3V 1:%d\n", phidgets.value(PHIDGET3V_1));
    printf("Phidget 3V 2:%d\n\n", phidgets.alue(PHIDGET3V_2));
#endif

    /* Receive sensor values and encode them */
    char message[COAP_MAX_CHUNK_SIZE] = "";
    
    int result = snprintf(message, COAP_MAX_CHUNK_SIZE - 1, "{\"message_id\": %d, \"gas_reading\": %d , \"led_status\": %d , \"relay_status\": %d}", counter, adc_zoul.value(ZOUL_SENSORS_ADC3), leds_get(), relay.status(SENSORS_ACTIVE));

    counter++;

    /* Send messages if encoding succeeded */
    if (result < 0)
    {
        puts("Error while encoding message");
    }
    else
    {
        puts("Sending Gas, led values");

        int length = strlen(message);
        memcpy(buffer, message, length);

        coap_set_header_content_format(response, APPLICATION_JSON);
        coap_set_header_etag(response, (uint8_t *)&length, 1);
        coap_set_payload(response, buffer, length);
    }
}

#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/relay.h"
#include "lib/sensors.h"

#if CONTIKI_TARGET_ZOUL
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
#endif

#include "coap-engine.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(gas_sensor, "title=\"GAS SENSOR READING\", rt=\"JSON\"", res_get_handler, NULL, NULL, NULL);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
#if CONTIKI_TARGET_ZOUL
    adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);
	SENSORS_ACTIVATE(relay);
#endif

#if CONTIKI_TARGET_ZOUL


    uint16_t gas_sensor_reading = adc_zoul.value(ZOUL_SENSORS_ADC2);
    printf("Gas Sensor Reading = %u mV\n", gas_sensor_reading);

    uint16_t gas_sensor_threshold = 800;
    
	bool condition = (gas_sensor_reading >= gas_sensor_threshold);
    if (condition)
    {
        leds_toggle(LEDS_RED);
		relay.value(RELAY_ON);
		printf("\nRelay: Switch is ON --> %u\n", relay.status(SENSORS_ACTIVE));
    }
    else
    {
        leds_toggle(LEDS_GREEN);
		relay.value(RELAY_OFF);
		printf("Relay: Switch is OFF --> %u\n\n", relay.status(SENSORS_ACTIVE));
    }

    char message[COAP_MAX_CHUNK_SIZE] = "";

    
    int result = snprintf(message, COAP_MAX_CHUNK_SIZE - 1, "\n\Message Id: %u\Gas Reading: %u mV, \Led Status: %u \Relay Status: - %u", counter, gas_sensor_reading, leds_get(), relay.status(SENSORS_ACTIVE));


    if(result < 0)
    {
        puts("Error while encoding message");
    }
    else
    {

        puts("Success. Sending Values");

        int len = strlen(message);
        memcpy(buffer, message, len);

        coap_set_header_content_format(response, APPLICATION_JSON);
        coap_set_header_etag(response, (uint8_t *)&len, 1);
        coap_set_payload(response, buffer, len);
    }

#endif
}

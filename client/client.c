#include <stdio.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
//#include "dev/adc-zoul.h"
//#include "dev/zoul-sensors.h"

#include "coap-log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define SERVER_EP "coap://[fe80::212:4b00:abcd:ef00]"
#define TOGGLE_INTERVAL 10

PROCESS(coap_client, "COAP Client Process");
AUTOSTART_PROCESSES(&coap_client);

static struct etimer et;

char *service_url = "/gas_sensor";

void client_chunk_handler(coap_message_t *response)
{
    const uint8_t *chunk;

    if(response == NULL)
    {
        puts("Request time out");
        return;
    }

    int len = coap_get_payload(response, &chunk);

    printf("|%.*s", len, (char *)chunk);
}

PROCESS_THREAD(coap_client, ev, data)
{
    static coap_endpoint_t server_ep;
    PROCESS_BEGIN();

    static coap_message_t request[1];

    coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
    coap_endpoint_connect(&server_ep);

    etimer_set(&et, TOGGLE_INTERVAL * CLOCK_SECOND);

    while(1)
    {
        PROCESS_YIELD();

        if(etimer_expired(&et))
        {
            printf("Sending request to %s", service_url);

            coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
            coap_set_header_uri_path(request, service_url);

            COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);

            printf("Done, Request Sent.");

            etimer_reset(&et);
        }
    }

    coap_endpoint_disconnect(&server_ep);

    PROCESS_END();
}

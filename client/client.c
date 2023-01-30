/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 *               2022, Schmalkalden University of Applied Sciences
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *      CoAP client example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Tobias Tefke <t.tefke@stud.fh-sm.de>
 */

#include <string.h>

#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "coap-callback-api.h"

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
#include "lwm2m-object.h"
#include "lwm2m-engine.h"
#include "lwm2m-security.h"
#endif

/* Logging */
#include "coap-log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_DBG

/* Define server address
 * Starting with coap:// -> use CoAP protocol
 * Starting with coaps:// -> use CoAPs (CoAP secure) protocol, encrypt traffic
 * Change this according to your server address
 */
#ifdef WITH_DTLS
#define SERVER_EP "coaps://[fe80::212:4b00:abcd:ef00]"
#else
#define SERVER_EP "coap://[fe80::212:4b00:abcd:ef00]"
#endif

/* Define Server URLs */
#define NUMBER_OF_URLS 4
char *server_urls[NUMBER_OF_URLS] =
{
    //".well-known/core",
    //"hello",
    //"leds/toggle",
    //"temperature",
    "all_sensors"
};
static short url_to_fetch = 0;

/* Set a timer to fetch URLs regularly */
static struct etimer et;

#if LOG_CONF_LEVEL_COAP == LOG_LEVEL_DBG
#define REQUEST_INTERVAL 60
#else
#define REQUEST_INTERVAL 10
#endif

/* Define client process */
PROCESS(coap_client, "CoAP Client");
AUTOSTART_PROCESSES(&coap_client);

/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(coap_message_t *response)
{
  const uint8_t *chunk;

  if(response == NULL) {
    puts("Request timed out");
    return;
  }

  int len = coap_get_payload(response, &chunk);

  printf("|%.*s", len, (char *)chunk);
}

PROCESS_THREAD(coap_client, ev, data)
{
    /* Create a struct holding our endpoind (server) information */
    static coap_endpoint_t server_ep;

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    /* Init keystore */
    /* Here, we could initialize a keystore with pre-shared keys.
     * We do not use this as of now
     */
    //lwm2m_security_init();
#endif
    
    /* Start process */
    PROCESS_BEGIN();
    
    puts("Client started");
    
    /* Request package */
    /* Use an array so that the packet can be treates as pointer, too */
    static coap_message_t request[1];
    
    /* Configure endpoint */
    coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
    
    /* Log endpond (server) information */
    puts("Server address:");
    LOG_INFO_COAP_EP(&server_ep);
    LOG_INFO_("\n");

    /* Log whether DTLS is enabled */
    if (coap_endpoint_is_secure(&server_ep)) {
        puts("Confidentiality is protected by DTLS");
    } else {
        puts("Confidentiality is not protected - DTLS is not enabled");
    }
    
    /* Connect to endpoint (server) */
    coap_endpoint_connect(&server_ep);
    
    /* Set our timer */
    etimer_set(&et, REQUEST_INTERVAL * CLOCK_SECOND);
    
    while (1) {
        /* Wait until timer expired */
        PROCESS_YIELD();
        if (etimer_expired(&et)) {
            printf("\nSending CoAP request to %s\n", server_urls[url_to_fetch]);
            
            /* Prepare CoAP request */
            coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
            coap_set_header_uri_path(request, server_urls[url_to_fetch]);
            
            /* Send request */
            COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
            
            /* Increse number of url_to_fetch */
            url_to_fetch++;
            if (url_to_fetch == NUMBER_OF_URLS) {
                url_to_fetch = 0;
            }
            
            /* Reset the timer */
            etimer_reset(&et);
        }
    }
    
    /* Disconnect from the endpoint */
    coap_endpoint_disconnect(&server_ep);
    
    PROCESS_END();
}

/*
 * Copyright (c) 2022, Schmalkalden University of Applied Sciences
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
 *      Example CoAP server
 * \author
 *      Tobias Tefke <t.tefke@stud.fh-sm.de>
 */

#include <stdio.h>
#include <stdlib.h>

/* Contiki main files */
#include "contiki.h"
#include "coap-engine.h"

/* Sensors */
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"

/* LWM */
#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
#include "lwm2m-object.h"
#include "lwm2m-engine.h"
#include "lwm2m-security.h"
#endif

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t
 
  res_all_sensors;

PROCESS(coap_server, "CoAP server");
AUTOSTART_PROCESSES(&coap_server);

PROCESS_THREAD(coap_server, ev, data)
{
    PROCESS_BEGIN();
    
#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    /* Here, we could initialize a keystore with pre-shared keys.
     * We do not use this as of now
     */
    //lwm2m_security_init();
#endif
    
    /* Activate sensors */
    adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);
    
    PROCESS_PAUSE();
    
    printf("Activating resource\n");
    
    /* Activate CoAP resources */   
    coap_activate_resource(&res_all_sensors, "all_sensors");
    while (1) {
        PROCESS_WAIT_EVENT();
    }
    
    printf("Should never happen\n");
    PROCESS_END();
}

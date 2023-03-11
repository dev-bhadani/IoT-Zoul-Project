#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/relay.h"
#include "lib/sensors.h"
#if CONTIKI_TARGET_ZOUL
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
#else
#endif

PROCESS(gas_sensor, "Gas Sensor");
AUTOSTART_PROCESSES(&gas_sensor);

static struct etimer et;

PROCESS_THREAD(gas_sensor, ev, data)
{
  PROCESS_BEGIN();

#if CONTIKI_TARGET_ZOUL

  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);
  SENSORS_ACTIVATE(relay);

#endif


  etimer_set(&et, CLOCK_SECOND);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

#if CONTIKI_TARGET_ZOUL

    
    int gas_sensor_reading = adc_zoul.value(ZOUL_SENSORS_ADC3);
    printf("\nGas Sensor Value := %u mV", gas_sensor_reading);
    int gas_sensor_threshold = 550;

    if(gas_sensor_reading >= gas_sensor_threshold)
    {
      leds_toggle(LEDS_RED);
      relay.value(RELAY_ON);
      printf("\nGas leakage detected!");
      printf("\nRelay status : ON");
      printf("\nLed status : RED\n");
    }
    else
    {
      leds_toggle(LEDS_GREEN);
      relay.value(RELAY_OFF);
      printf("\nGas leakage not detected yet!");
      printf("\nRelay status : OFF");
      printf("\nLed status : Green\n");
    }
     
#endif
    etimer_reset(&et);
  }

  PROCESS_END();
}

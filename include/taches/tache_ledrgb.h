#ifndef TACHE_LEDRGB
#define TACHE_LEDRGB

#include <NeoPixelBus.h>

const uint16_t PixelCount = 1; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 18;  // make sure to set this to the correct pin, ignored for Esp8266

void task_ledrgb( void *pvParameters );

#endif
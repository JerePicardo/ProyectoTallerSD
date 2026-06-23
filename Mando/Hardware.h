



#ifndef Hardware_h
#define Hardware_h
#include <Arduino.h>

void initButtonInterrupt(uint8_t BUTTON_PIN,void (*callback)());
void initMPUInterrupt(uint8_t pin, void (*callback)());

#endif
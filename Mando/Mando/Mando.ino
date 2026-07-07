#include <Wire.h>
#include <SPI.h>
#include <RF24.h>
#include <MPU6050.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "Definiciones.h"
#include "Hardware.h"
#include "MandoFSM.h"

volatile bool buttonFlag      = false;
volatile bool mpuMotionFlag = false;
volatile bool watchdogFlag    = false;
volatile bool sampleFlag      = false;
Mando M ={0};
RF24 radio(CE_PIN, CSN_PIN);
MPU6050 mpu1(0x68);
MPU6050 mpu2(0x69);
char txt[50];
static uint32_t lastSample = 0;
static uint32_t lastMovement = 0;
char c;
uint32_t now;

EventQueue eventQueue ={0};






void initMPUInterrupt(uint8_t MPU_PIN){

    pinMode(MPU_PIN, INPUT);


    attachInterrupt(
        digitalPinToInterrupt(MPU_PIN),
       mpuMotionISR,
        RISING
    );

}




void configureMPUs()
{
    mpu1.initialize();
    mpu2.initialize();

    if(!mpu1.testConnection())
    {
        M.event = EVENTO_ERROR;
    }

    if(!mpu2.testConnection())
    {
        M.event = EVENTO_ERROR;
    }
   // Configuracion interrupcion movimiento

}


ISR(WDT_vect)
{
    watchdogFlag = true;
}

void initWatchdog()
{
    cli();

    MCUSR &= ~(1 << WDRF);

    WDTCSR |= (1 << WDCE) | (1 << WDE);

    WDTCSR =
        (1 << WDIE) |      // interrupt
        (1 << WDP3) |
        (1 << WDP0);       // 8s
    
  sei();

}

ISR(TIMER1_COMPA_vect)
{
    sampleFlag = true;
    //pushEvent(EVENTO_SAMPLE);
}



void initSamplingTimer()
{
    cli();

    TCCR1A = 0;
    TCCR1B = 0;

    TCNT1 = 0;

    OCR1A = (16000000UL / 64UL / fS) - 1;

    TCCR1B |= (1 << WGM12);

    TCCR1B |=
        (1 << CS11) |
        (1 << CS10);

    TIMSK1 |= (1 << OCIE1A);

    sei();
}



void configureRadio()
{
    radio.begin();

    radio.setPALevel(RF24_PA_LOW);

    radio.setDataRate(RF24_250KBPS);

    radio.stopListening();
}

void enterSleep()
{

    disableSamplingTimer();


    enableMPUWakeup();


    // aca despues va:
    // apagar NRF24
    // bajar frecuencia CPU
    // sleep_mode()


}
void eventUpdate(void);

void setup() {
   Serial.begin(115200);

    Wire.begin();

    initButtonInterrupt(BUTTON_PIN);

    initMPUInterrupt(MPU_INT_PIN);

    initSamplingTimer();

    initWatchdog();

    configureRadio();

    configureMPUs();
    disableMPUWakeup();

    sei();

}

void loop() {

    eventUpdate();

    while((M.event = popEvent()) != EVENTO_NONE)
    {
        stateMachine(&M);
    }
}



void eventUpdate(void){
     now = millis();
  if(buttonFlag)
{
    buttonFlag = false;
    pushEvent(EVENTO_BUTTON_PRESS);
    lastMovement = now;
}
if(mpuMotionFlag)
{
    mpuMotionFlag = false;
   pushEvent(EVENTO_WAKEUP);
   lastMovement=now;
}

if(watchdogFlag)
{
    watchdogFlag = false;
    pushEvent(EVENTO_WAKEUP);
 }
 if(sampleFlag)
{
    sampleFlag = false;
lastMovement = now;
    pushEvent(EVENTO_SAMPLE);
}
if (!radio.isChipConnected()) {
     pushEvent(EVENTO_RF_TIMEOUT);
}

if (mpu1.getDeviceID() != 0x68 ) {
    pushEvent(EVENTO_SENSOR_TIMEOUT);
}

if (mpu2.getDeviceID() != 0x69 ) {
    pushEvent(EVENTO_SENSOR_TIMEOUT);
}
if(Serial.available())
{   
    char c1 = Serial.read();

    if(c1=='m')
    {
        pushEvent(EVENTO_MANUAL_CMD);
    }else if(c1=='q'){
        pushEvent(EVENTO_EXIT_MANUAL);
    }
}


if (now - lastMovement > SLEEP_TIME_MS) {
    pushEvent(EVENTO_SLEEP_TIMEOUT);
    lastMovement = now;
}

}


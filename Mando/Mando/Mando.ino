#include <Wire.h>
#include <SPI.h>
#include <RF24.h>
#include <MPU6050.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "Hardware.h"
#include "MandoFSM.h"

#define CE_PIN            9
#define CSN_PIN           10
#define fS                100
#define SAMPLE_PERIOD_MS 20
#define SLEEP_TIME_MS    10000
#define BOTON_GARRA      9
#define BOTON_MANUAL     10

volatile bool buttonFlag      = false;
volatile bool mpuMotionFlag   = false;
volatile bool watchdogFlag    = false;
volatile bool sampleFlag      = false;

const uint8_t BUTTON_PIN = 3;
const uint8_t MPU_INT_PIN = 2;
Mando M ={0};
RF24 radio(CE_PIN, CSN_PIN);
MPU6050 mpu1(0x68);
MPU6050 mpu2(0x69);

static uint32_t lastSample = 0;
static uint32_t lastMovement = 0;
char c;
uint32_t now;

EventQueue eventQueue =
{
    .head = 0,
    .tail = 0
};

void initButtonInterrupt(uint8_t BUTTON_PIN){
      pinMode(BUTTON_PIN, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(BUTTON_PIN),
        buttonISR,
        FALLING
    );
}

void buttonISR()
{
    buttonFlag = true;
}



void initMPUInterrupt(uint8_t MPU_INT_PIN){

    pinMode(MPU_INT_PIN, INPUT);


    attachInterrupt(
        digitalPinToInterrupt(MPU_INT_PIN),
       mpuMotionISR,
        RISING
    );

}


void mpuMotionISR()
{
    mpuMotionFlag = true;
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
}

void enableMPUWakeup()
{

    mpu1.setMotionDetectionThreshold(20);
    mpu1.setMotionDetectionDuration(5);

    mpu1.setIntMotionEnabled(true);



    mpu2.setMotionDetectionThreshold(20);
    mpu2.setMotionDetectionDuration(5);

    mpu2.setIntMotionEnabled(true);


    attachInterrupt(
        digitalPinToInterrupt(MPU_INT_PIN),
        mpuMotionISR,
        RISING
    );

}


void disableMPUWakeup()
{

    mpu1.setIntMotionEnabled(false);

    mpu2.setIntMotionEnabled(false);



    detachInterrupt(
        digitalPinToInterrupt(MPU_INT_PIN)
    );

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

void disableSamplingTimer(void){
    cli();
   TIMSK1 &= ~(1 << OCIE1A);
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
    now = millis();

M.state =stateMachine(&M) ;  

  if(buttonFlag)
{
    buttonFlag = false;
    M.event=EVENTO_BUTTON_PRESS;
}
if(mpuMotionFlag)
{
    mpuMotionFlag = false;
    M.event=EVENTO_WAKEUP;
}

if(watchdogFlag)
{
    watchdogFlag = false;
    M.event=EVENTO_WAKEUP;
 }
 if(sampleFlag)
{
    sampleFlag = false;

    M.event = EVENTO_SAMPLE;
}
if (!radio.isChipConnected()) {
     M.event=EVENTO_RF_TIMEOUT;
}

if (mpu1.getDeviceID() != 0x68 ) {
    M.event =EVENTO_SENSOR_TIMEOUT;
}

if (mpu2.getDeviceID() != 0x69 ) {
    M.event =EVENTO_SENSOR_TIMEOUT;
}
if (digitalRead(BOTON_MANUAL)|| Serial.read() =='m') {
    M.event = EVENTO_MANUAL_CMD;
}

if (now - lastSample >= SAMPLE_PERIOD_MS) {
    lastSample = now;
   M.event = EVENTO_SAMPLE;
}

if (now - lastMovement > SLEEP_TIME_MS) {
    M.event = EVENTO_SLEEP_TIMEOUT;
}

}





#include <Wire.h>
#include <SPI.h>
#include <RF24.h>
#include <MPU6050.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <Hardware.h>

#define CE_PIN            9
#define CSN_PIN           10
#define fS                100

volatile bool buttonFlag      = false;
volatile bool mpuMotionFlag   = false;
volatile bool watchdogFlag    = false;
volatile bool sampleFlag      = false;

const uint8_t BUTTON_PIN = 3;
const uint8_t MPU_INT_PIN = 2;

RF24 radio(CE_PIN, CSN_PIN);
MPU6050 mpu1(0x68);
MPU6050 mpu2(0x69);

volatile evento evento_actual = EVENTO_NONE;

void buttonISR()
{
    buttonFlag = true;
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
        evento_actual = EVENTO_ERROR;
    }

    if(!mpu2.testConnection())
    {
        evento_actual = EVENTO_ERROR;
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

void setup() {
   Serial.begin(115200);

    Wire.begin();

    initButtonInterrupt(BUTTON_PIN, buttonISR);

    initMPUInterrupt(MPU_PIN_INT, mpuMotionISR);

    initSamplingTimer();

    initWatchdog();

    configureRadio();

    configureMPUs();
    disableMPUWakeup();

    sei();

}

void loop() {
  if(buttonFlag)
{
    buttonFlag = false;
    evento_actual=EVENTO_BUTTON_PRESS;
}
if(mpuMotionFlag)
{
    mpuMotionFlag = false;
    evento_actual=EVENTO_WAKEUP;
}

if(watchdogFlag)
{
    watchdogFlag = false;
    evento_actual=EVENTO_WAKEUP;
 }
 if(sampleFlag)
{
    sampleFlag = false;

    evento_actual = EVENTO_SAMPLE;
}

}

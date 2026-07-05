#include "Arduino.h"
#include "MandoFSM.h"
#define EVENT_QUEUE_SIZE 16



//-------------------------------ESTADOS------------------------
void stateMachine(Mando * M) {

switch(M->state) {

case ESTADO_ACTIVE:

    switch(M->event) {

    case EVENTO_SAMPLE:
        readSensors();
        buildPacket();
        transmitPacket();
        break;

    case EVENTO_BUTTON_PRESS:
        sendGripperCommand();
        break;

    case EVENTO_SLEEP_TIMEOUT:
        enterSleep();
        M->state = ESTADO_SLEEP;
        break;

    case EVENTO_MANUAL_CMD:
        M->state = ESTADO_MANUAL;
        break;

    case EVENTO_RF_TIMEOUT:
    case EVENTO_SENSOR_TIMEOUT:
    case EVENTO_ERROR:
        M->state = ESTADO_ERROR;
        break;

    default:
        break;
    }

    break;

case ESTADO_SLEEP:

      if(M->event == EVENTO_WAKEUP)
    {

        disableMPUWakeup();


        enableSamplingTimer();


        M->state = ACTIVE;

    }

    break;

case ESTADO_MANUAL:

    processManualCommands();

    if(M->event == EVENTO_EXIT_MANUAL) {
        M->state = ESTADO_ACTIVE;
    }

    break;

case ESTADO_ERROR:

    handleError();

    if(errorRecovered()) {
        M->state = ESTADO_ACTIVE;
    }

    break;

default:
    break;
}

}





bool pushEvent(evento ev)
{
    uint8_t next = (eventQueue.head + 1) % EVENT_QUEUE_SIZE;

    // Cola llena
    if(next == eventQueue.tail)
    {
        return false;
    }

    eventQueue.buffer[eventQueue.head] = ev;
    eventQueue.head = next;

    return true;
}

evento popEvent(void)
{
    // Cola vacía
    if(eventQueue.head == eventQueue.tail)
    {
        return EVENTO_NONE;
    }

    evento ev = eventQueue.buffer[eventQueue.tail];

    eventQueue.tail = (eventQueue.tail + 1) % EVENT_QUEUE_SIZE;

    return ev;
}




#include <MandoFSM.h>



// ----------------------------------EVENTOS------------------------------

evento updateEvents() {

static uint32_t lastSample = 0;
static uint32_t lastMovement = 0;

uint32_t now = millis();

if (now - lastSample >= SAMPLE_PERIOD_MS) {
    lastSample = now;
    return EVENTO_SAMPLE;
}

if (buttonPressed()) {
    return EVENTO_BUTTON_PRESS;
}

if (serialCommandAvailable()) {
    return EVENTO_MANUAL_CMD;
}

if (movementDetected()) {
    lastMovement = now;
}

if (now - lastMovement > SLEEP_TIME_MS) {
    return EVENTO_SLEEP_TIMEOUT;
}

if (!rfOk()) {
    return EVENTO_RF_TIMEOUT;
}

if (!sensorsOk()) {
    return EVENTO_SENSOR_TIMEOUT;
}

return EVENTO_NONE;

}



//-------------------------------ESTADOS------------------------
estado   stateMachine(Mando * M) {

switch(st_actual) {

case ESTADO_ACTIVE:

    switch(ev) {

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
        currentState = ESTADO_SLEEP;
        break;

    case EVENTO_MANUAL_CMD:
        currentState = ESTADO_MANUAL;
        break;

    case EVENTO_RF_TIMEOUT:
    case EVENTO_SENSOR_TIMEOUT:
    case EVENTO_ERROR:
        currentState = ESTADO_ERROR;
        break;

    default:
        break;
    }

    break;

case ESTADO_SLEEP:

      if(evento_actual == EVENTO_WAKEUP)
    {

        disableMPUWakeup();


        enableSamplingTimer();


        currentState = ACTIVE;

    }

    break;

case ESTADO_MANUAL:

    processManualCommands();

    if(ev == EVENTO_EXIT_MANUAL) {
        currentState = ESTADO_ACTIVE;
    }

    break;

case ESTADO_ERROR:

    handleError();

    if(errorRecovered()) {
        currentState = ESTADO_ACTIVE;
    }

    break;

default:
    break;
}

}






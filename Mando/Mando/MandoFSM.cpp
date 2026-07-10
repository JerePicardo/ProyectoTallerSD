#include "Arduino.h"
#include "MandoFSM.h"

uint8_t conta=0;


//-------------------------------ESTADOS------------------------
void stateMachine(Mando * M) {

switch(M->state) {

case ESTADO_ACTIVE:

    switch(M->event) {

    case EVENTO_SAMPLE:
        readSensors(M);
        buildPacket(M);
        transmitPacket(M);
        printStatus(M);
        break;

    case EVENTO_BUTTON_PRESS:
        M->gripperClosed = !M->gripperClosed;

        break;

    case EVENTO_SLEEP_TIMEOUT:
        disableSamplingTimer();
        enableMPUWakeup();
        // sleep_cpu();
        M->state = ESTADO_SLEEP;
        break;

     case EVENTO_MANUAL_CMD:
        Serial.println(F("Modo manual"));
        M->state = ESTADO_MANUAL;
    break;

    case EVENTO_RF_TIMEOUT:
     Serial.println(F("ERROR: NRF24 desconectado"));
     M->state = ESTADO_ERROR;
     break;

    case EVENTO_SENSOR_TIMEOUT:
        Serial.println(F("ERROR: MPU6050 no responde"));
        M->state = ESTADO_ERROR;
    break;
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
        M->state=ESTADO_ACTIVE;
    }

    break;

case ESTADO_MANUAL:
    if(M->event == EVENTO_MANUAL_CMD ){
        processManualCommands(M);
        break;
    }
    if(M->event == EVENTO_EXIT_MANUAL) {
        M->state = ESTADO_ACTIVE;
        M->payload.flag &= ~FLAG_MANUAL;
        break;
    }

    break;

case ESTADO_ERROR:
conta++;
    if(M->event == EVENTO_SAMPLE && conta==0){
         M->state = ESTADO_ACTIVE;
        break;
    }
    if (M->event == EVENTO_MANUAL_CMD){
        M->state = ESTADO_MANUAL;
        break;
    }
    //handleError();

    /*if(errorRecovered()) {
        M->state = ESTADO_ACTIVE;
    }
*/
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

void processManualCommands(Mando *M)
{
    uint8_t canal;
    uint8_t angulo;

 if(Serial.available())
{
    canal = Serial.parseInt();
    angulo = Serial.parseInt();

        M->payload.channel = canal;
        M->payload.angle = angulo;

        M->payload.flag |= FLAG_MANUAL;

        transmitPacket(M);

        Serial.print("Enviado -> Servo ");
        Serial.print(canal);
        Serial.print("  Angulo ");
        Serial.println(angulo);
    }
}
void printStatus(Mando *M)
{
    static uint32_t lastPrint = 0;

    if(millis() - lastPrint < 500)
        return;

    lastPrint = millis();

    Serial.println(F("----------------------------------------"));

    Serial.print(F("Estado: "));
    Serial.println(estadoToString(M->state));

    Serial.print(F("Ultimo evento: "));
    Serial.println(eventoToString(M->event));

    Serial.print(F("Pote: "));
    Serial.println(M->payload.pote);

    Serial.print(F("Pinza: "));
    if(M->payload.flag & FLAG_APRETAR_PINZA)
        Serial.println(F("CERRADA"));
    else
        Serial.println(F("ABIERTA"));

    Serial.print(F("MPU1 AX: "));
    Serial.print(M->payload.acelerometros[0][0]);
    Serial.print(F("  GX: "));
    Serial.println(M->payload.giroscopios[0][0]);

    Serial.print(F("MPU2 AX: "));
    Serial.print(M->payload.acelerometros[1][0]);
    Serial.print(F("  GX: "));
    Serial.println(M->payload.giroscopios[1][0]);

    Serial.print(F("Timestamp: "));
    Serial.println(M->payload.TIMESTAMP);

    Serial.println();
}
const char *estadoToString(estado st)
{
    switch(st)
    {
        case ESTADO_ACTIVE: return "ACTIVE";
        case ESTADO_SLEEP:  return "SLEEP";
        case ESTADO_MANUAL: return "MANUAL";
        case ESTADO_ERROR:  return "ERROR";
        default:            return "UNKNOWN";
    }
}

const char *eventoToString(evento ev)
{
    switch(ev)
    {
        case EVENTO_NONE:            return "NONE";
        case EVENTO_SAMPLE:          return "SAMPLE";
        case EVENTO_SLEEP_TIMEOUT:   return "SLEEP";
        case EVENTO_WAKEUP:          return "WAKEUP";
        case EVENTO_BUTTON_PRESS:    return "BUTTON";
        case EVENTO_MANUAL_CMD:      return "MANUAL";
        case EVENTO_EXIT_MANUAL:     return "EXIT_MANUAL";
        case EVENTO_RF_TIMEOUT:      return "RF_TIMEOUT";
        case EVENTO_SENSOR_TIMEOUT:  return "SENSOR_TIMEOUT";
        case EVENTO_BAT_LOW:         return "BAT_LOW";
        case EVENTO_ERROR:           return "ERROR";
        default:                     return "UNKNOWN";
    }
}

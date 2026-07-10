

#ifndef definiciones_h
#define definiciones_h

#define EVENT_QUEUE_SIZE 20

#define CE_PIN            8
#define CSN_PIN           7
#define fS                100
#define SAMPLE_PERIOD_MS 100
#define SLEEP_TIME_MS    10000
#define BOTON_GARRA      9
#define BOTON_MANUAL     10 // Este boton no lo estoy usando ni lo tengo puesto


#define BUTTON_PIN  3
#define MPU_INT_PIN  2

extern volatile bool mpuMotionFlag;
extern volatile bool buttonFlag;

typedef enum{
ESTADO_ACTIVE,
ESTADO_SLEEP,
ESTADO_MANUAL,
ESTADO_ERROR
} estado;

typedef enum{
EVENTO_NONE,
EVENTO_SAMPLE,
EVENTO_SLEEP_TIMEOUT,
EVENTO_WAKEUP,
EVENTO_BUTTON_PRESS,
EVENTO_MANUAL_CMD,        //
EVENTO_EXIT_MANUAL,       // SOLO HACE FALTA UNO?
EVENTO_RF_TIMEOUT,        //hACE FALTA TANTOS TIMEOUTS? SE PUEDE ENCAPSULAR EN 1
EVENTO_SENSOR_TIMEOUT,
EVENTO_BAT_LOW,           // NO SE HACERLO
EVENTO_ERROR        
}evento;

#define  FLAG_EMPTY           (1 << 0)
#define  FLAG_DORMIR          (1 << 1)
#define  FLAG_MANUAL          (1 << 2)
#define  FLAG_APRETAR_PINZA   (1 << 3)


typedef struct __attribute__((packed)) {
  int16_t acelerometros[2][3];
  int16_t giroscopios[2][3];
  uint8_t pote; //hay que hacer un map por que es de 10bitsa
  uint16_t TIMESTAMP;
  uint8_t angle;
  uint8_t channel;
  uint8_t flag;
}data;

typedef struct {
  data payload;
  estado state;
  evento event;
  bool gripperClosed;
} Mando;

typedef struct
{
    evento buffer[EVENT_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} EventQueue;

extern EventQueue eventQueue;


#endif
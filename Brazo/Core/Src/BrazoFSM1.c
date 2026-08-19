#include <BrazoFSM1.h>
#include <math.h>
#include <stdint.h>
#include <string.h>




//########################################			VARIABLE			####################################################

static rx_data rx_buffer;
static char txt[64];
static void processComm(Brazo *B);
static float mapf(float x, float in_min, float in_max, float out_min, float out_max);
static float clampf(float v, float min, float max);
static uint32_t last_ms = 0;
UART_HandleTypeDef *huart;
static uint8_t selected_servo = 0;
static uint8_t screenTimer = 20;
static Quaternion q1 ={0};
static Quaternion q2 ={0};
static Mahony imu1 = {
    .q = {1.0f, 0.0f, 0.0f, 0.0f},
    .ix = 0.0f,
    .iy = 0.0f,
    .iz = 0.0f
};

static Mahony imu2 = {
    .q = {1.0f, 0.0f, 0.0f, 0.0f},
    .ix = 0.0f,
    .iy = 0.0f,
    .iz = 0.0f
};



//########################################			INIT MAQUINA			####################################################

void FSM_Brazo_init(Brazo *B, UART_HandleTypeDef *huartf) {
	B->actual = STATE_INICIO;
	huart = huartf;
	last_ms = 0;
	park(B->pos);
}




//########################################			MAQUINA			####################################################


void FSM_Brazo(Brazo *B, evento event) {
	switch (B->actual) {
	case STATE_INICIO:
		switch(event) {
		case EVENT_NEW_DATA:
			B->actual = STATE_ACTIVO;
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			nrf24_receive((uint8_t*) &rx_buffer, sizeof(rx_data));
			B->last_rf_comm = rx_buffer;
			processComm(B);
			UpdateBrazo(B->pos);
			break;
		case EVENT_MANUAL:
			B->actual = STATE_MANUAL;
			strcpy(txt,"ACTIVADO MODO MANUAL\n\n\r");
			HAL_UART_Transmit(huart, (uint8_t *) txt, strlen(txt), HAL_MAX_DELAY);
			break;

		default:
			break;
		}
		break;

	case STATE_ACTIVO:
		switch (event) {
		case EVENT_NEW_DATA:
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			nrf24_receive((uint8_t*) &rx_buffer, sizeof(rx_data));

			//contador para actualizar pantalla cada 20 samples
			if(screenTimer = 20){
				screenTimer = 0;
				ssd1306_Fill(Black);
				sprintf(txt, "S0=%d S1=%d S2=%d",B->pos[0],B->pos[1],B->pos[2] );
				ssd1306_SetCursor(0,20);
				ssd1306_WriteString(txt, Font_6x8, White);
				sprintf(txt, "S3=%d S4=%d S5=%d",B->pos[3],B->pos[4],B->pos[5] );
				ssd1306_SetCursor(0,40);
				ssd1306_WriteString(txt, Font_6x8, White);
				ssd1306_UpdateScreen();
			} else{
				screenTimer++;
			}

			B->last_rf_comm = rx_buffer;
			processComm(B);
			UpdateBrazo(B->pos);
			break;

		case EVENT_TIMEOUT:
			//HAY QUE ARMAR ESTE ESTADO EVENTO CUANDO ALGUIEN ESTE AL PEDO
			break;


		case EVENT_PARKEAR:

			park(B->pos);
			//Debug prints:
			//HAL_UART_Transmit(huart, (uint8_t *) "Parkeado bien chill\n\n\r", sizeof("Parkeado bien chill\n\n\r"), HAL_MAX_DELAY);
			mensaje_ssd("parkeado bien chill", Font_6x8, 0, 0, 1);
			//PARA DESPERTARLO HAL_TIM_Base_Start_IT(&htim1);
			break;


		case EVENT_EVIL_DATA:
			//HAL_UART_Transmit(huart, (uint8_t *) "Error: datos corruptos\n\n\r", sizeof("Error: datos corruptos\n\n\r"), HAL_MAX_DELAY);
			mensaje_ssd("err: datos corruptos", Font_6x8, 0, 0, 1);
			break;

		case EVENT_MANUAL:
			B->actual = STATE_MANUAL;
			char *msg = "ACTIVADO MODO MANUAL\r\n\n";
			HAL_UART_Transmit(huart, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
			break;
		default:
			break;
	}
	break;

	case STATE_PARK:
		switch(event) {

		case EVENT_NEW_DATA:
			B->actual = STATE_ACTIVO;
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			nrf24_receive((uint8_t*) &rx_buffer, sizeof(rx_data));
			B->last_rf_comm = rx_buffer;
			processComm(B);
			UpdateBrazo(B->pos);
			break;


		case EVENT_MANUAL:
			B->actual = STATE_MANUAL;
			char *msg = "ACTIVADO MODO MANUAL\r\n\n";
			HAL_UART_Transmit(huart, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
			break;
		default:
			break;
		}
		break;

	case STATE_MANUAL:
		switch(event){
		case EVENT_MAS:
			Brazo_increaseAngle(B->pos);
			selected_servo = Brazo_getSelectedServo();
			sprintf(txt, "Posicion actual del servo %d: %d\r\n", selected_servo, B->pos[selected_servo]);
			HAL_UART_Transmit(huart, (uint8_t *) txt, strlen(txt), HAL_MAX_DELAY);
			break;
		case EVENT_MENOS:
			Brazo_decreaseAngle(B->pos);
			selected_servo = Brazo_getSelectedServo();
			sprintf(txt, "Posicion actual del servo %d: %d\r\n", selected_servo, B->pos[selected_servo]);
			HAL_UART_Transmit(huart, (uint8_t *) txt, strlen(txt), HAL_MAX_DELAY);
			break;
		case EVENT_AUTO:
			char *msgAuto = "CAMBIANDO A MODO AUTOMATICO\r\n\n";
			HAL_UART_Transmit(huart, (uint8_t *) msgAuto, strlen(msgAuto), HAL_MAX_DELAY);
		default:
			break;
		}
		break;

	default:
		break;
	}
}

//########################################			FUNCIONES			####################################################



static float clampf(float v, float min, float max){
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

static float mapf(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//########################################			PROCESAMIENTO			####################################################


static void processComm(Brazo *B)
{
    static float roll_f[2]  = {0.0f, 0.0f};
    static float pitch_f[2] = {0.0f, 0.0f};
    static float yaw_f[2] = {0.0f, 0.0f};

    if(!last_ms){
        last_ms = B->last_rf_comm.timeStamp;
    	return;
    }

    float dt = ((B->last_rf_comm.timeStamp - last_ms) / 1000.0f);
    last_ms = B->last_rf_comm.timeStamp;

    for (int s = 0; s < 2; s++)
    {
        float ax = (float)B->last_rf_comm.acelerometros[s][0];
        float ay = (float)B->last_rf_comm.acelerometros[s][1];
        float az = (float)B->last_rf_comm.acelerometros[s][2];

        float gx = (float)B->last_rf_comm.giroscopios[s][0] / GYRO_SENS_250;
        float gy = (float)B->last_rf_comm.giroscopios[s][1] / GYRO_SENS_250;
        float gz = (float)B->last_rf_comm.giroscopios[s][2] / GYRO_SENS_250;

        float roll_acc  = atan2f(ay, az) * RAD_TO_DEG;
        float pitch_acc = atan2f(-ax, sqrtf(ay * ay + az * az)) * RAD_TO_DEG;
        float yaw_acc  = atan2f(ay, ax) * RAD_TO_DEG;
        roll_f[s]  = ALPHA * (roll_f[s]  + gx * dt) + (1.0f - ALPHA) * roll_acc;
        pitch_f[s] = ALPHA * (pitch_f[s] + gy * dt) + (1.0f - ALPHA) * pitch_acc;
        yaw_f[s] = ALPHA * (yaw_f[s] + gz * dt) + (1.0f - ALPHA ) * yaw_acc;
    }


    B->pos[0] = 180-(uint8_t)clampf(mapf(roll_f[1],  -90.0f,  90.0f,   0.0f, 180.0f), LIM_INF_SERVO_0, LIM_SUP_SERVO_0 );  // muñeca: giro
    B->pos[1] = 175 - (uint8_t)clampf(mapf(1.3*pitch_f[1],  -45.0f,  45.0f,   0.0f, 180.0f), 0.0f, 175.0f);
	B->pos[2] = (uint8_t)clampf(mapf(yaw_f[1], -45.0f,  45.0f,   0.0f, 180.0f), LIM_INF_SERVO_1, LIM_SUP_SERVO_1);  // muñeca: flexión
	B->pos[4] = (uint8_t)clampf(mapf(roll_f[0],  -45.0f,  45.0f,   0.0f, 180.0f), LIM_INF_SERVO_4, LIM_SUP_SERVO_4);  // hombro: abducción
	B->pos[5] = (uint8_t)clampf(mapf(yaw_f[0], -45.0f,  45.0f,   0.0f, 180.0f), LIM_INF_SERVO_5, LIM_SUP_SERVO_5);  // hombro: flex/ext

    //codo
	B->pos[3] = (alpha_codo * B->last_rf_comm.pote + (1.0f - alpha_codo) * B->pos[3]);
	//sprintf(txt, "pote:%d\n\r", (uint8_t)B->pos[3] );
	//mensaje_ssd(txt,Font_6x8, 0, 40, 0);
    //pinza
    if(B->last_rf_comm.flag & FLAG_APRETAR_PINZA){
    	B->pos[6] = 170;
	} else{
		B->pos[6] = 5;
	}


    if (B->last_rf_comm.flag & FLAG_DORMIR) {
		B->last_pr_comm.dormido = 1;
		park(B->pos);
		ssd1306_Fill(Black);
		sprintf(txt, "Dormido.... ZzZzz%c",'z');
		ssd1306_SetCursor(0,0);
		ssd1306_WriteString(txt, Font_6x8, White);
		ssd1306_UpdateScreen();
	}

    q1.w=B->last_rf_comm.acelerometros[0][0];
    q1.x=B->last_rf_comm.acelerometros[0][0];
    q1.y=B->last_rf_comm.acelerometros[0][1];
    q1.z=B->last_rf_comm.acelerometros[0][2];
    q2.w=B->last_rf_comm.giroscopios[1][0]/ GYRO_SENS_250 * DEG_TO_RAD;
    q2.x=B->last_rf_comm.giroscopios[1][0]/ GYRO_SENS_250 * DEG_TO_RAD;
    q2.y=B->last_rf_comm.giroscopios[1][1]/ GYRO_SENS_250 * DEG_TO_RAD;
    q2.z=B->last_rf_comm.giroscopios[1][2]/ GYRO_SENS_250 * DEG_TO_RAD;

    Mahony_Update(&imu1,q1.x,q1.y,q1.z,q2.x,q2.y,q2.z,dt);

    return;
}










//########################################			COMENTARIOS COPIABLES 				####################################################



/*
static float unwrapAngle(float angle, float *previous)
{
    while(angle - *previous > 180.0f)
        angle -= 360.0f;

    while(angle - *previous < -180.0f)
        angle += 360.0f;

    *previous = angle;

    return angle;
}
*/

//Debug prints:
/*
sprintf(txt, "AX:%d\n\r", rx_buffer.acelerometros[0][0]);
HAL_UART_Transmit(huart, (uint8_t *) txt, strlen(txt), HAL_MAX_DELAY);
//ssd1306_Fill(Black);
//ssd1306_SetCursor(0,10);
//ssd1306_WriteString(txt, Font_6x8, White);
sprintf(txt, "Ay:%d\n\r", rx_buffer.acelerometros[0][1]);
HAL_UART_Transmit(huart, (uint8_t *) txt, strlen(txt), HAL_MAX_DELAY);
//ssd1306_SetCursor(0,20);
//ssd1306_WriteString(txt, Font_6x8, White);
sprintf(txt, "Az:%d\n\r", rx_buffer.acelerometros[0][2]);
HAL_UART_Transmit(huart, (uint8_t *) txt, strlen(txt), HAL_MAX_DELAY);
//ssd1306_SetCursor(0,30);
//ssd1306_WriteString(txt, Font_6x8, White);

if(B->last_rf_comm.flag & FLAG_MANUAL){
	HAL_UART_Transmit(huart, (uint8_t *) "SYNC MANUAL\n\n\r", strlen("SYNC MANUAL\n\n\r"), HAL_MAX_DELAY);
	//ssd1306_SetCursor(0,40);
	//ssd1306_WriteString("SYNC Manual", Font_6x8, White);
}else{
	HAL_UART_Transmit(huart, (uint8_t *) "SYNC Activo\n\n\r", strlen("SYNC Activo\n\n\r"), HAL_MAX_DELAY);
	//ssd1306_SetCursor(0,40);
	//ssd1306_WriteString("SYNC Activo", Font_6x8, White);
	//ssd1306_UpdateScreen();
}
*/

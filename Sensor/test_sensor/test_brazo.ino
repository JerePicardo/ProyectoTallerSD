#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_PWMServoDriver.h>

// Instanciamos los dispositivos
Adafruit_MPU6050 mpu;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Configuración de pulsos para los servos
#define SERVOMIN  150 
#define SERVOMAX  600 
#define SERVO_FREQ 50 

// Asignación de canales en el PCA9685
const int canalPot    = 3; // Motor 3 -> Controlado por Potenciómetro
const int canalServoY  = 4; // Motor 4 -> Eje Y del Sensor
const int canalServoZ  = 5; // Motor 5 -> Eje X del Sensor (Mapeo recomendado)

// Pin analógico para el potenciómetro
const int pinPotenciometro = A0;

void setup() {
  // Inicializamos la comunicación serie a 115200 baudios (recuerda configurarlo en tu monitor)
  Serial.begin(115200); 
  
  // Inicializar PCA9685
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);

  // Inicializar MPU6050
  if (!mpu.begin()) {
    Serial.println("Error: No se pudo encontrar el MPU6050. Revisa el cableado I2C!");
    while (1) { delay(10); } 
  }
  Serial.println("MPU6050 y PCA9685 configurados correctamente.");

  // Configuración del filtro para mitigar vibraciones por ruido mecánico
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); 

  delay(100);
}

void loop() {
  // ----------------------------------------------------
  // 1. LECTURA Y CONTROL DEL POTENCIÓMETRO (Motor 3)
  // ----------------------------------------------------
  int valorPot = analogRead(pinPotenciometro); // Lee un valor entre 0 y 1023
  int anguloPot = map(valorPot, 0, 1023, 0, 180); // Lo convierte a grados (0-180)
  
  // Convertimos a pulso PWM para el PCA9685
  int pulsoPot = map(anguloPot, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(canalPot, 0, pulsoPot);


  // ----------------------------------------------------
  // 2. LECTURA Y CONTROL DEL SENSOR MPU6050 (Motores 4 y 5)
  // ----------------------------------------------------
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calcular ángulos de inclinación basados en el acelerómetro (Radianes a Grados)
  float angleY = atan2(a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / M_PI;
  float angleX = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180.0 / M_PI;

  // Adaptar el rango de la fórmula (-90 a 90) a grados de servo (0 a 180)
  int servoPosY = map(angleY, -90, 90, 0, 180);
  int servoPosZ = map(angleX, -90, 90, 0, 180);

  // Restringir valores por seguridad de los engranajes
  servoPosY = constrain(servoPosY, 0, 180);
  servoPosZ = constrain(servoPosZ, 0, 180);

  // Convertir grados del sensor a pulsos PWM
  int pulsoY = map(servoPosY, 0, 180, SERVOMIN, SERVOMAX);
  int pulsoZ = map(servoPosZ, 0, 180, SERVOMIN, SERVOMAX);

  // Enviar señales a los motores del sensor
  pwm.setPWM(canalServoY, 0, pulsoY);
  pwm.setPWM(canalServoZ, 0, pulsoZ);


  // ----------------------------------------------------
  // 3. MONITOREO SERIAL
  // ----------------------------------------------------
  Serial.print("POT [M3]: "); Serial.print(anguloPot);
  Serial.print("° | SensorY [M4]: "); Serial.print(servoPosY);
  Serial.print("° | SensorZ [M5]: "); Serial.print(servoPosZ);
  Serial.println("°");

  delay(20); // Mantiene el bucle fluido pero estable
}
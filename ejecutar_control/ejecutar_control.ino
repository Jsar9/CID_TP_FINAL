#define R_EN 4
#define L_EN 5
#define R_PWM 6
#define L_PWM 7

//Variables para la lectura analógica
const uint8_t analog_pin = A0; // uint8_t va de 0 a 255, mientras que el nano tiene pocos pines. Se ocupa 1 byte menos que con el int tradicional que es de 2bytes
uint16_t raw_value = 0; // el ADC del arduino, posee resolución de 10 bits (entre 0 y 1023) y es un valor positivo, por lo que este tipo es optimo (lee de 0 a 65535)

//Variables temporales
unsigned long init_time = 0;
unsigned long previous_time = 0;

void setup() {

  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
 
  Serial.begin(9600);
  Serial.println("BTS7960 Motor Driver Test");
  
  init_time = millis()
}

void loop() {
  unsigned long current_time = millis();

  //Se lee el valor raw del sensor
  raw_value = analogRead(analog_pin);

  //Si raw_value es un valor entre 0 y 512, se considera que es un valor negativo para el motor
  if (raw_value <= 512)
  {
    pwm_val = (raw_value * 100)/512.0;
  }







  //Se actualiza el tiempo previo
  previous_time = current_time;
}

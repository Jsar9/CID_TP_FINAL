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
unsigned long pwm_period = 20; // en ms

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

  //Se lee el valor raw del sensor
  raw_value = analogRead(analog_pin);

  //se desplaza la escala [0; 1024] hacia [-512; 512]
  raw_val_mod = raw_value - 512.0; 

  //Si raw_value es un valor entre 0 y 512, se considera que es un valor "negativo" para el motor -> se acciona "hacia atrás"
  if (raw_val_mod <= -10) //Avance "-"
  {   
      pwm_val = (-raw_val_mod * 100)/512.0; //los valores entre -10 y 10, serán zona muerta: el motor se encuentra apagado.
      digitalWrite(R_EN, LOW);
      digitalWrite(L_EN, HIGH);
      analogWrite(R_PWM, 0); // Set speed (0-255)
      analogWrite(L_PWM, pwm_val);
  } 
  else if(pwm_val >=10) //Avance "+"
  {
      pwm_val = (raw_val_mod * 100)/512.0; //los valores entre -10 y 10, serán zona muerta: el motor se encuentra apagado.
      digitalWrite(R_EN, RIGHT);
      digitalWrite(L_EN, LOW);
      analogWrite(R_PWM, pwm_val); // Set speed (0-255)
      analogWrite(L_PWM, 0);
  }
  else{ //Detenido en la zona muerta
      pwm_val = 0;
      digitalWrite(R_EN, LOW);
      digitalWrite(L_EN, LOW);
      analogWrite(R_PWM, 0); // Set speed (0-255)
      analogWrite(L_PWM, 0);

  }
}

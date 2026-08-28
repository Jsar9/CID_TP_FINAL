#define R_EN 4
#define L_EN 5
#define R_PWM 6
#define L_PWM 7

#define INF_THR (-5)
#define SUP_THR 5


//Variables para la lectura analógica
const uint8_t analog_pin = A7; // uint8_t va de 0 a 255, mientras que el nano tiene pocos pines. Se ocupa 1 byte menos que con el int tradicional que es de 2bytes
uint16_t raw_value = 0; // el ADC del arduino, posee resolución de 10 bits (entre 0 y 1023) y es un valor positivo, por lo que este tipo es optimo (lee de 0 a 65535)
int raw_value_mod = 0; //Los datos raw del arduino se desplazan de [0;1024] a [-512; 512]
uint8_t pwm_val = 0;


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
}

void loop() {

  //Se lee el valor raw del sensor
  raw_value = analogRead(analog_pin);

  //Se desplaza el rango [0; 1023] a [-512; 511]
  raw_value_mod = raw_value - 512;

  //Si raw_value es un valor entre 0 e inf_thr , se considera que es un valor negativo para el motor -> se acciona "hacia atrás"
  if (raw_value_mod < INF_THR) //Avance "-"
  {   
      //Se hace un cast a long para evitar overflow y luego se divide por 512 que equivale a desplazar 9 bits a derecha
      pwm_val = (uint8_t)((((long)-raw_value_mod) * 255) >> 9);

      digitalWrite(R_EN, LOW);
      digitalWrite(L_EN, HIGH);
      analogWrite(R_PWM, 0);
      analogWrite(L_PWM, pwm_val); // Set speed (0-255)
  }
  //Si raw_value es un valor entre sup_thr y 255, se considera que es un valor positivo para el motor -> se acciona "hacia adelante"
  else if(raw_value_mod > SUP_THR) //Avance "+"
  {
      //Se hace un cast a long para evitar overflow y luego se divide por 511
      pwm_val = (uint8_t)((((long)raw_value_mod) * 255)/ 511);
      
      digitalWrite(R_EN, HIGH);
      digitalWrite(L_EN, LOW);
      analogWrite(R_PWM, pwm_val); // Set speed (0-255)
      analogWrite(L_PWM, 0);
  }
  else{ //Detenido en la zona muerta - Se considera la zona muerta como un 2% de la velocidad nominal.
      pwm_val = 0;
      digitalWrite(R_EN, LOW);
      digitalWrite(L_EN, LOW);
      analogWrite(R_PWM, 0);
      analogWrite(L_PWM, 0);
  }

  Serial.print("raw_value:");
  Serial.println(raw_value);

  Serial.print("raw_value_mod: ");
  Serial.println(raw_value_mod);

  Serial.print("pwm_val: ");
  Serial.println(pwm_val);
}

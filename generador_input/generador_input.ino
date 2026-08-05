#define R_EN 4
#define L_EN 5
#define R_PWM 6
#define L_PWM 7

unsigned int long init_time_state = 0;
const unsigned long time_forward = 2000; //ms
const unsigned long time_backward = 2000; //ms
const unsigned long time_state_stop = 1000; //ms



unsigned int  seq_state = 1;
unsigned int pwm_value = 0;
unsigned int pwm_forward = 200;
unsigned int pwm_backward = 200;

 
void setup() {
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
 
  Serial.begin(9600);
  Serial.println("BTS7960 Motor Driver Test");

  init_time_state = millis();
}
 
void loop() {
  unsigned long current_time = millis();

  // Evaluamos en qué estado de la secuencia estamos
  switch (seq_state) {
    
    case 1: // STATE 1 - FORWARD
      digitalWrite(R_EN, HIGH);
      digitalWrite(L_EN, HIGH);
      analogWrite(R_PWM, pwm_forward); // Set speed (0-255)
      analogWrite(L_PWM, 0);
      pwm_value = pwm_forward;

      if (current_time - init_time_state >= time_forward) {
        seq_state = 2;             //Se avanza al siguiente estado
        init_time_state = millis();   //se resetea el tiempo de inicio del estado como el actual
        Serial.println("FIN ETAPA 1");
      }
      break;

    case 2:  // STATE 2 - STOP
      digitalWrite(R_EN, LOW);
      digitalWrite(L_EN, LOW);
      analogWrite(R_PWM, 0);
      analogWrite(L_PWM, 0);
      pwm_value = 0;
    
      if (current_time - init_time_state >= time_state_stop) {
        seq_state = 3; 
        init_time_state = millis();
        Serial.println("FIN ETAPA 2");
      }
      break;

    case 3: // STATE 3 - RETURN TO CENTER
      digitalWrite(R_EN, HIGH);
      digitalWrite(L_EN, HIGH);
      analogWrite(R_PWM, 0);
      analogWrite(L_PWM, pwm_backward); // Set speed (0-255)
      pwm_value = pwm_backward;
      
      if (current_time - init_time_state >= time_backward) {
        seq_state = 4; //pasa a la siguiente etapa
        init_time_state = millis(); //resetea el timer
        Serial.println("FIN ETAPA 3");
      }
      break;

    case 4: // STATE 4 - STOP
      digitalWrite(R_EN, LOW);
      digitalWrite(L_EN, LOW);
      analogWrite(R_PWM, 0);
      analogWrite(L_PWM, 0);
      pwm_value = 0;

      if (current_time - init_time_state >= time_state_stop) {
          seq_state = 5; 
          init_time_state = millis();
          Serial.println("FIN ETAPA 4");
        }
        break;

     case 5: // STATE 5 - BACKWARD
      digitalWrite(R_EN, HIGH);
      digitalWrite(L_EN, HIGH);
      analogWrite(R_PWM, 0);
      analogWrite(L_PWM, pwm_backward); // Set speed (0-255)
      pwm_value = pwm_backward;

      if (current_time - init_time_state >= time_backward) {
        seq_state = 6; 
        init_time_state = millis(); //resetea el timer
        Serial.println("FIN ETAPA 5");
      }
      break;

      case 6: // STATE 6 - STOP
        digitalWrite(R_EN, LOW);
        digitalWrite(L_EN, LOW);
        analogWrite(R_PWM, 0);
        analogWrite(L_PWM, 0);
        pwm_value = 0;

        if (current_time - init_time_state >= time_state_stop) {
            seq_state = 7 ; 
            init_time_state = millis();
            Serial.println("FIN ETAPA 6");
        }
        break;

      case 7: // STATE 7 - RETURN TO CENTER
        digitalWrite(R_EN, HIGH);
        digitalWrite(L_EN, HIGH);
        analogWrite(R_PWM, pwm_forward); // Set speed (0-255)
        analogWrite(L_PWM, 0);
        pwm_value = pwm_forward;

        if (current_time - init_time_state >= time_forward) {
          seq_state = 8;            
          init_time_state = millis();   
          Serial.println("FIN ETAPA 7");
        }
        break;


      case 8: // STATE 8 - STOP
        digitalWrite(R_EN, LOW);
        digitalWrite(L_EN, LOW);
        analogWrite(R_PWM, 0);
        analogWrite(L_PWM, 0);
        pwm_value = 0;

        if (current_time - init_time_state >= time_state_stop) {
            seq_state = 1; 
            init_time_state = millis();
            Serial.println("FIN ETAPA 8 - REINICIO");
        }
        break;
  }

  Serial.print(current_time);
  Serial.print(",");
  Serial.println(pwm_value);
}

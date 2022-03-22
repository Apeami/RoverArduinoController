#include <AFMotor.h>
#include <Servo.h>

//Pins used in this program
//Pin 9 - OCR1A - Rover Left Side
//Pin 10 - OCR1B - Rover Right Side
//Pin A0-5 -  used for the inputs from the controller

void pciSetup(byte pin){
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin));                   // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin));                   // enable interrupt for the group
}
const int pwmPIN[]={A0,A1,A2,A3,A4};
const int num_ch = 5;
int prev_pinState[num_ch];

int pwmTimer[num_ch];
int timeHigh[num_ch];
int timeLow[num_ch];

int pciTime;

ISR(PCINT1_vect){                                                 
  pciTime = micros();                                             
  for (int i = 0; i < num_ch; i++){                              
      int state = digitalRead(pwmPIN[i]);                               
      if(prev_pinState[i] == 0 && state){                         
        prev_pinState[i] = 1; 
                                          
        timeLow[i] = pciTime - pwmTimer[i];                     
        pwmTimer[i] = pciTime; 
        //Serial.println(timeLow[i]);                                      
      }
      else if (prev_pinState[i] == 1 && !state){                  
        prev_pinState[i] = 0; 
                                            
        timeHigh[i] = pciTime - pwmTimer[i];    
        pwmTimer[i] = pciTime;   
        //Serial.println(timeHigh[i]);                     
      }   
  }
}

void setupControllerReader(){
  //Sets pin to input and sets-up the interrupt
  //Pin interrupt setup
  for(int i = 0; i < num_ch; i++){              
    pciSetup(pwmPIN[i]);  
    pinMode(pwmPIN[i],INPUT);                    
  }
}

void setupMotorDriver(){
  //Sets the pins into output mode
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  //This sets all of the required constants for the PWM signal to be created
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11) ;
  TCCR1B = _BV(WGM12)| _BV(WGM13) | _BV(CS12);

  //This value was experimentally calculated
  ICR1 = 1260;

  //The default value is motor off. This is between minValue and maxValue
  OCR1A = 94.5;
  OCR1B = 94.5;
}

void controlMotor(int side, int motorSpeed){
  //These are constants that were experimentally calculated as the time.
  const int minValue=63;
  const int maxValue=126;
  const int range=maxValue-minValue;

  const int maxSpeed = 255;
  const int minSpeed = -255;
  const int speedRange = maxSpeed-minSpeed;

  //Ensure that motor speed is within bounds
  if (motorSpeed>maxSpeed){motorSpeed=maxSpeed;}
  if (motorSpeed<minSpeed){motorSpeed=minSpeed;}


  //Choose the side, this changes the variable to write too
  //It also sets ORC variable so it is mapped within the range of values
  
  // ((motorSpeed*range)/200)+minValue
  if (side==0){
    OCR1A =0;
    delay(1000);
    OCR1A = (((motorSpeed-minSpeed)*range)/speedRange)+minValue;
  }if (side==1){
    OCR1B =0;
    delay(1000);
    OCR1B = (((motorSpeed-minSpeed)*range)/speedRange)+minValue;
  }
}

int calculateDutyCycle(int A, int B){
  return (A*100)/(A+B);
}

AF_DCMotor motor1(1, MOTOR12_64KHZ);
AF_DCMotor motor2(2, MOTOR12_64KHZ);
void controlHMotor(int side, int motorSpeed){
  if (side==0){
    if (motorSpeed==0){motor1.run(RELEASE);return;}
    if (motorSpeed>0){motor1.run(FORWARD);}
    if (motorSpeed<0){motor1.run(BACKWARD);}
    motor1.setSpeed(abs(motorSpeed));
  }
    if (side==1){
    if (motorSpeed==0){motor2.run(RELEASE);return;}
    if (motorSpeed>0){motor2.run(FORWARD);}
    if (motorSpeed<0){motor2.run(BACKWARD);}
    motor2.setSpeed(abs(motorSpeed));
  }
  return;
}

Servo ESC1;
Servo ESC2;
void controlESC(int side, int motorSpeed){
  motorSpeed = map(motorSpeed, -255, 255, 0, 180);
  if (side==0){
    ESC1.write(motorSpeed);
  }
  if (side==0){
    ESC2.write(motorSpeed);
  }
}


void setup() {
  Serial.begin(9600);
  motor.setSpeed(0);
  ESC1.attach(10,1000,2000);
  ESC2.attach(11,1000,2000);
  setupMotorDriver();
  setupControllerReader();

}

void loop() {
  static int potValue;
  static int motorPower = 100;
  static int debounce=0;
  //Read byte from serial and set as motor power

  if (Serial.available() > 0) {
    if (debounce==0){
      motorPower=Serial.parseInt();
      controlMotor(0, motorPower);
      controlMotor(1, motorPower);
      controlHMotor(0, motorPower);
      controlHMotor(1, motorPower);
      controlESC(0, motorPower);
      controlESC(1, motorPower);
      debounce=1;
     }else{
      Serial.parseInt();
      debounce=0;
     }  
  }

  Serial.println(timeHigh[0]);
  Serial.println(timeLow[0]);

  //Delay for smoothness
  delay(100);
}

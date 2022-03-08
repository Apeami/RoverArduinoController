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
int pwmPeriod[num_ch];
int PW[num_ch];
int pwmPIN_reg[num_ch];

int pciTime;

ISR(PCINT1_vect){                                                 // this function will run if a pin change is detected on portC

  pciTime = micros();                                             // Record the time of the PIN change in microseconds

  for (int i = 0; i < num_ch; i++){                               // run through each of the channels                                    
      if(prev_pinState[i] == 0 && PINC & pwmPIN_reg[i]){          // and the pin state has changed from LOW to HIGH (start of pulse)
        prev_pinState[i] = 1;                                     // record pin state
        pwmPeriod[i] = pciTime - pwmTimer[i];                     // calculate the time period, micro sec, between the current and previous pulse
        pwmTimer[i] = pciTime;                                    // record the start time of the current pulse
      }
      else if (prev_pinState[i] == 1 && !(PINC & pwmPIN_reg[i])){ // or the pin state has changed from HIGH to LOW (end of pulse)
        prev_pinState[i] = 0;                                     // record pin state
        PW[i] = pciTime - pwmTimer[i];                             // calculate the duration of the current pulse
      }   
  }
}

void setupControllerReader(){
  //Sets pin to input and sets-up the interrupt
  //Pin interrupt setup
  for(int i = 0; i < num_ch; i++){              
    pciSetup(pwmPIN[i]);                       
  }
}

//BTW these functions are not working
//These are global variables
int value2=0;
long timeLast2=0;
void readerHigh2(){
   //Get the change in time from the last
   value2= millis()-timeLast2;
   timeLast2 = timeLast2+value2;
}

int getControllerValue(int pin){
  if (pin==2){
    return value2;
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

  const int maxSpeed = 200;
  const int minSpeed = 0;
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
    OCR1A = ((motorSpeed*range)/speedRange)+minValue;
  }if (side==1){
    OCR1B =0;
    delay(1000);
    OCR1B = ((motorSpeed*range)/speedRange)+minValue;
  }
}


void setup() {
  Serial.begin(9600);
  setupMotorDriver();
  setupControllerReader();

}

void loop() {
  static int motorPower = 100;
  static int debounce=0;
  //Desired frequency = 50Hz
  //Read byte from serial and set as motor power

  
  if (Serial.available() > 0) {
    if (debounce==0){
      motorPower=Serial.parseInt();
      controlMotor(0, motorPower);
      controlMotor(1, motorPower);
      debounce=1;
     }else{
      Serial.parseInt();
      debounce=0;
     }  
  }


  //Read from remote controller 
  //Serial.println("Message");
  //Serial.println(getControllerValue(2),DEC);
  //controlMotor(0, -100);
}

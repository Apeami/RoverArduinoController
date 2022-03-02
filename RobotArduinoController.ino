//Pins used in this program
//Pin 9 - OCR1A - Rover Left Side
//Pin 10 - OCR1B - Rover Right Side
//Pin A0-5 -  used for the inputs from the controller

void setupControllerReader(){
  //Sets pin to input and sets-up the interrupt
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), readerHigh2, RISING);
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

  //Set motor speed range from 0 to 200
  motorSpeed+=100;

  //Choose the side, this changes the variable to write too
  //It also sets ORC variable so it is mapped within the range of values
  if (side==0){
    OCR1A = ((motorSpeed/200)*range)+minValue;
  }if (side==1){
    OCR1B = ((motorSpeed/200)*range)+minValue;
  }
}


void setup() {
  Serial.begin(9600);
  setupMotorDriver();
  setupControllerReader();

}

void loop() {
  //Desired frequency = 50Hz
  int motorPower=0;
  char* incomingByte;

  //Read byte from serial and set as motor power
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    motorPower=atoi(incomingByte)
  }
  controlMotor(0, motorPower);

  //Read from remote controller 
  //Serial.println("Message");
  //Serial.println(getControllerValue(2),DEC);
  //controlMotor(0, -100);
}

//Pins used in this program
//Pin 9 - OCR1A - Rover Left Side
//Pin 10 - OCR1B - Rover Right Side
//Pin A0-5 -  used for the inputs from the controller

int getControllerValue(int pin){
  //returns value between 0 and 255

  //Constants that come from the control stick
  const int minInput = 0;
  const int maxInput = 5000;

  //Read the analog value at the pin using the built in analog read function
  int val=0;
  const int times = 50;

  
  while(analogRead(pin)>10);
  long time1=millis();
  while(analogRead(pin)<10);
  while(analogRead(pin)>10);
  Serial.println(millis()-time1);
  Serial.println(millis());

  //Checks to see if the analog reading is within the correct range that is 
  //expected to come from the control stick
  if (val>maxInput || val<minInput){
    return -1; //Error not expected value
  }

  //Scales the value to the desired range
  return ((val-minInput)*(255/(maxInput-minInput)));
}

void setupControllerReader(){
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), readerHigh2, RISING);
}

int value2=0;
long timeLast2=0;
void readerHigh2(){
   value2= millis()-timeLast2;
   timeLast2 = timeLast2+value2;
   //Serial.println("H");
}

int getControllerValue2(int pin){
  if (pin==2){
    return value2;
  }
}


void setupMotorDriver(){
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11) ;
  TCCR1B = _BV(WGM12)| _BV(WGM13) | _BV(CS12);
  ICR1 = 1260;
  OCR1A = 94.5;
  OCR1B = 94.5;
}

void controlMotor(int side, int motorSpeed){
  const int minValue=63;
  const int maxValue=126;
  const int range=maxValue-minValue;
  
  motorSpeed+=100;
  
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
  
  Serial.println("Message");
  Serial.println(getControllerValue2(2),DEC);
  //controlMotor(0, -100);
}

//Pins used in this program
//Pin 9 - OCR1A - Rover Left Side
//Pin 10 - OCR1B - Rover Right Side
//Pin A0-5 -  used for the inputs from the controller

#define FREQ 16000000
void pwmToPin(int pin, int dutyCycle, int frequency){
//Valid values of "pin" are 9 and 10, all other values will be ignored
//Duty cycle is a value from 1 to 1000 measured in percent
//Frequency is a value in Hz


//Step 1: Calculate values

//Step 2: Set Waveform Generation Mode Bit Description
//Mode 15: Fast PWM with TOP as ICR1. WGM13-10 = 1110

//Step 3: Set Clock Select Bit Description
//CS12-10 = based on calculated value
//Clk/1 -- 001

//Setp 4: Set Input Capture Register
//ICR1H and ICR1L = based on calculated value
//12422 = 0b 0011 0000 1000 0110

//Step 5: Set Output Compare Register
//OCR1AH, OCR1AL or OCR1BH, OCR1BL = based on dutyCycle from calculation

//Step 6: Set Compare Output Mode, Fast PWM
//Should be COM1A1/COM1B1=0 , COM1A0/COM1B0=1
 
}

int getControllerValue(int pin){
  //returns value between 0 and 255

  //Constants that come from the control stick
  const int minInput = 10;
  const int maxInput = 50;

  //Read the analog value at the pin using the built in analog read function
  int val = analogRead(pin);
  Serial.println(val);

  //Checks to see if the analog reading is within the correct range that is 
  //expected to come from the control stick
  if (val>maxInput || val<minInput){
    return -1; //Error not expected value
  }

  //Scales the value to the desired range
  return ((val-minInput)*(255/(maxInput-minInput)));
}


void setup() {
  Serial.begin(9600);

}

void loop() {
  //Desired frequency = 50Hz
  
  //Serial.println("Message");
  Serial.println(getControllerValue(A0),DEC);
}

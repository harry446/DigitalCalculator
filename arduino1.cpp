/*
Name: Harry Liu
*/

/*
  Code for Arduino 1. Mainly to control the displays of the user input
Also transfers commands to Arduino 3 for operation sign display
*/

#include <Keypad.h>

//initialize variables

//For keypad
const byte ROWS = 4;
const byte COLS = 4;
const byte C1 = 13;
const byte C2 = 12;
const byte C3 = 11;
const byte C4 = 10;
const byte R1 = A3;
const byte R2 = A2;
const byte R3 = A1;
const byte R4 = A0;
char keys[ROWS][COLS] = { {'1','2','3','A'},
                          {'4','5','6','B'},
                          {'7','8','9','C'},
                          {'*','0','#','D'}
                        };
byte rowPins[ROWS] = {A3, A2, A1, A0}; 	//row pin numbers
byte colPins[COLS] = {13, 12, 11, 10}; 	//col pin numbers
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//for the 7-segment decoders
const byte firstNum1 = 6;
const byte firstNum2 = 7;
const byte firstNum3 = 8;
const byte firstNum4 = 9;
const byte secondNum1 = 2;
const byte secondNum2 = 3;
const byte secondNum3 = 4;
const byte secondNum4 = 5;

//sign communication to Arduino 3
const byte wire0 = 0;
const byte wire1 = 1;

//binary arrays, store binary numbers
byte binaryInputLeft[4];
byte binaryInputRight[4];

byte index = 0;	//index pointer used to keep track of input order

//initial setup
void setup() {
  pinMode(C1, INPUT);
  pinMode(C2, INPUT);
  pinMode(C3, INPUT);
  pinMode(C4, INPUT);
  pinMode(R1, INPUT);
  pinMode(R2, INPUT);
  pinMode(R3, INPUT);
  pinMode(R4, INPUT);
  
  pinMode(firstNum1, OUTPUT);
  pinMode(firstNum2, OUTPUT);
  pinMode(firstNum3, OUTPUT);
  pinMode(firstNum4, OUTPUT);
  pinMode(secondNum1, OUTPUT);
  pinMode(secondNum2, OUTPUT);
  pinMode(secondNum3, OUTPUT);
  pinMode(secondNum4, OUTPUT);
  
  pinMode(wire0, OUTPUT);
  pinMode(wire1, OUTPUT);
}


void loop() {
  
  char key = keypad.getKey();	//get an input from keypad
  
  if (key != NO_KEY) {	//if a key is pressed
    //if key pressed is a number, display
    if (key == '1' || key == '2' || key == '3' ||
        key == '4' || key == '5' || key == '6' ||
        key == '7' || key == '8' || key == '9' || key == '0') {
      
      //if index is pressed at the very beginning
      //or after an operation sign, index++
      if (index == 0 || index == 2) {
        index++;
      } 
      
      byte input = (byte)key - 48;	//gets the input as a byte
      
      //index == 1, meaning the first number, only change the first display
      if (index == 1) {
        binaryArrayReset(binaryInputLeft);		//reset binary array
        convertToBinary(input, binaryInputLeft);//decimal to binary
        
        turnOff(firstNum1);						//turn everything off
        display(firstNum1, binaryInputLeft);	//display the number
      }  
      
      //index == 3, meaning the second number, only change the second display
      if (index == 3) {   	
        binaryArrayReset(binaryInputRight);		//reset binary array
        convertToBinary(input, binaryInputRight);//decimal to binary
        
        turnOff(secondNum1);					//turn everything off
        display(secondNum1, binaryInputRight);	//display the number
      } 
    }  
    
    //If an operation sign is entered, communicate with Arduino 3
    if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
      
      //changes index only when it is the first time coming from 
      //an integer input (doesn't change for operation changes)
      if (index == 1) {
        index++;
      }  
      
      //only enters when index == 2
      if (index == 2) {
        switch (key) {	
          case 'A':	//A = add, both wires off
            digitalWrite(wire0, LOW);
            digitalWrite(wire1, LOW);
            break;
          case 'B':	//B = subtract, wire0 on, wire1 off
            digitalWrite(wire0, HIGH);
            digitalWrite(wire1, LOW);
            break;
          case 'C':	//C = multiply, wire0 off, wire1 on
            digitalWrite(wire0, LOW);
            digitalWrite(wire1, HIGH);
            break;
          case 'D':	//D = divide, both wires on
            digitalWrite(wire0, HIGH);
            digitalWrite(wire1, HIGH);
            break;
        }  
      }  
    }
    
    //if reset key is pressed, reset everything to default condition
    if (key == '*') {
      turnOff(firstNum1);
      turnOff(secondNum1);
      digitalWrite(wire0, LOW);
      digitalWrite(wire1, LOW);
      binaryArrayReset(binaryInputLeft);
      binaryArrayReset(binaryInputRight);
      index = 0;
    }  
    
  }  
  
}


/*
  void binaryArrayReset ()
  byte binaryArray[] - This parameter is the array to reset. 
  					   Reference is passed, same memory pointer.
  This method resets the given array (sets all values to 0)
*/
void binaryArrayReset (byte binaryArray[4]) {
  for (int i=0; i<4; i++) {
    binaryArray[i] = 0;
  }  
}  

/*
  void converToBinary ()
  byte tempInput - This parameter is the decimal number to be converted
  byte binaryArray[] - This parameter is the array to store the binary in.
  				       Reference is passed, same memory pointer.
  This method converts the binary equivalent of the given decimal number
*/
void convertToBinary (byte tempInput, byte binaryArray[4]) {
  byte count = 0;
  while (tempInput > 0) {	//repeated division method
    binaryArray[count] = tempInput % 2;
    tempInput = tempInput/2;
    count++;
  }  
}  

/*
  void turnOff ()
  byte index - This parameter is the pin number to begin at
  This method turns a specific 7-segment display off (to 0)
*/
void turnOff (byte index) {
  for (int i=0; i<4; i++) {
    digitalWrite(index+i, LOW);
  } 
} 

/*
  void display ()
  byte index - This parameter is the pin number to begin at
  byte binaryArray[] - This parameter is the array to get binary number from.
  				       Reference is passed, same memory pointer.
  This method displays the number on the 7-segment display
*/
void display (byte index, byte binaryArray[4]) {
  for (int i=0; i<4; i++) {
    if (binaryArray[i] == 1) {
      digitalWrite(index+i, HIGH);
    }  
  } 
}
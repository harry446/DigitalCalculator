/*
Name: Harry Liu
*/

/*
  Code for Arduino 2. Mainly used for calculation purposes, also displays
the answer on the two 7-segment displays. Decimal numbers are always
truncated(rounded down) into whole numbers.
  Communicates with Arduino 3, tells it when to start lighting up operation
LEDs, when to light up equal sign, when to light up negative sign, and
when to reset (switch everything off)
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

//for the 7-segment decoder
const byte firstNum1 = 6;
const byte firstNum2 = 7;
const byte firstNum3 = 8;
const byte firstNum4 = 9;
const byte secondNum1 = 2;
const byte secondNum2 = 3;
const byte secondNum3 = 4;
const byte secondNum4 = 5;

//sign communication to Arduino 3
const byte wire3 = 0; 
const byte wire4 = 1; 

//binary arrays, store binary numbers
byte binaryAnswerTens[4];
byte binaryAnswerOnes[4];

byte num1, num2; //two numbers inputted
byte answerTens, answerOnes; //answer, two digits
char sign; //operation sign 

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
  
  pinMode(wire3, OUTPUT);
  pinMode(wire4, OUTPUT);
}

void loop() {
  
  char key = keypad.getKey();	//get an input from keypad
  
  if (key != NO_KEY) {	//if a key is pressed
    //if key pressed is a number, store
    if (key == '1' || key == '2' || key == '3' ||
        key == '4' || key == '5' || key == '6' ||
        key == '7' || key == '8' || key == '9' || key == '0') {
      
      //if index is pressed at the very beginning
      //or after an operation sign, index++
      if (index == 0 || index == 2) {
        index++;
      } 
      
      byte input = (byte)key - 48;	//gets the input as a byte
      
      //store the two input numbers
      if (index == 1) { //index == 1, meaning the first number
        num1 = input;
      } 
      else if (index == 3) {  //index == 3, indicating the second number
        num2 = input;
      }  
      
    }  
    
    
    //If an operation sign is entered, store and communicate with Arduino 3
    if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
      
      //communicate with Arduino 3 for operation sign LED display
      digitalWrite(wire3, HIGH); 
      digitalWrite(wire4, LOW);
      
      //changes index only when it is the first time coming from 
      //an integer input (doesn't change for operation changes)
      if (index == 1) {
        index++;
      }  
      
      //only enters when index == 2
      if (index == 2) {
        switch (key) {	
          case 'A':	//A = add
            sign = '+';
            break;
          case 'B':	//B = subtract
            sign = '-';
            break;
          case 'C':	//C = multiply
            sign = '*';
            break;
          case 'D':	//D = divide
            sign = '/';
            break;
        }  
      } 
    }  
    
    //If equal sign is pressed, calculate and display
    if (key == '#' && index == 3) { //only works when all 3 values are entered
      
      //communicate with Arduino 3 to ligh up equal sign LEDs
      digitalWrite(wire3, LOW);
      digitalWrite(wire4, HIGH);
      
      if (sign == '+') {	//add numbers
        byte sum = num1 + num2;
        answerTens = sum/10;
        answerOnes = sum-answerTens*10;
      }  
      else if (sign == '-') {	//subtract numbers
        if (num1 < num2) {//Tell Arduino 3 to turn on negative sign
          delay(10);	//short delay for Arduino 3 inputs...
          digitalWrite(wire3, HIGH);
          digitalWrite(wire4, HIGH);
          
          byte difference = num2 - num1; //calculate but the other way around
          answerTens = difference/10;
          answerOnes = difference-answerTens*10;
        }  
        else {	//normal subtraction
          byte difference = num1 - num2;
          answerTens = difference/10;
          answerOnes = difference-answerTens*10;
        }  
      }  
      else if (sign == '*') {	//multiplication
        byte product = num1 * num2;
        answerTens = product/10;
        answerOnes = product-answerTens*10;
      }  
      else if (sign == '/') {	//division
        if (num2 == 0) {  //cannot be divided by 0
          answerTens = 0;
          answerOnes = 0;
        }  
        else {	//normal division
          byte quotient = num1 / num2;
          answerTens = quotient/10;
          answerOnes = quotient-answerTens*10;
        }  
      } 
      
      
      //output(display)
      convertToBinary(answerTens, binaryAnswerTens);//decimal to binary
      convertToBinary(answerOnes, binaryAnswerOnes);//decimal to binary

      display(firstNum1, binaryAnswerTens);
      display(secondNum1, binaryAnswerOnes);
      
    }  
    
    
    //If reset switch is pressed, reset everything
    if (key == '*') {
      turnOff(firstNum1);
      turnOff(secondNum1);
      digitalWrite(wire3, LOW);
      digitalWrite(wire4, LOW);
      binaryArrayReset(binaryAnswerOnes);
      binaryArrayReset(binaryAnswerTens);
      index = 0;
      answerOnes = 0;	
      answerTens = 0;	
      num1 = 0;			
      num2 = 0;			
      sign = ' ';
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
void turnOff (int index) {
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
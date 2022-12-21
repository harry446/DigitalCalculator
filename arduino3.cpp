/*
Name: Harry Liu
*/

/*
  Code for Arduino 3. This Arduino is used to control the LED displays
of the operation signs, equal sign, and negative sign. It receives input
from both Arduino 1 and 2. Arduino 1 tells it which operation sign to 
display. Arduino 2 tells it when to start displaying the operation signs,
equal sign, negative sign, and when to reset (turn every LED off).
*/

//sign communication from Arduino 1
const byte wire0 = 3;
const byte wire1 = 2;
const byte wire3 = 7;
const byte wire4 = 6;

//output wires
const byte equalSign = 4;
const byte negativeSign = 5;
const byte addition = 9;
const byte subtraction = 10;
const byte multiplication = 11;
const byte division = 12;

bool operatorEntered = false; //used to keep every operation LED off at first

//initial setup
void setup() {
  pinMode(wire0, INPUT);
  pinMode(wire1, INPUT);
  pinMode(wire3, INPUT);
  pinMode(wire4, INPUT);
  
  pinMode(equalSign, OUTPUT);
  pinMode(negativeSign, OUTPUT);
  pinMode(addition, OUTPUT);
  pinMode(subtraction, OUTPUT);
  pinMode(multiplication, OUTPUT);
  pinMode(division, OUTPUT);
}

void loop() {
  
  //very short delay to allow system to do initial start up
  //without any chance of triggering the if statement below
  if(!operatorEntered) {
    delay(10);
  }  
  
  //This is to make sure at the beginning every operation LED is off
  if (digitalRead(wire3) == HIGH && digitalRead(wire4) == LOW && !operatorEntered) {
    operatorEntered = true;
  }  
    
  //only enters when flag = true, which is when at least one 
  //operation sign button is pressed
  if (operatorEntered) {
    if (digitalRead(wire0) == LOW && digitalRead(wire1) == LOW) {	//(+)
      digitalWrite(addition, HIGH);
      digitalWrite(subtraction, LOW);
      digitalWrite(multiplication, LOW);
      digitalWrite(division, LOW);
    }  
    else if (digitalRead(wire0) == HIGH && digitalRead(wire1) == LOW){ //(-)
      digitalWrite(addition, LOW);
      digitalWrite(subtraction, HIGH);
      digitalWrite(multiplication, LOW);
      digitalWrite(division, LOW);
    }  
    else if (digitalRead(wire0) == LOW && digitalRead(wire1) == HIGH){ //(*)
      digitalWrite(addition, LOW);
      digitalWrite(subtraction, LOW);
      digitalWrite(multiplication, HIGH);
      digitalWrite(division, LOW);
    } 
    else if (digitalRead(wire0) == HIGH && digitalRead(wire1) == HIGH){ //(/)
      digitalWrite(addition, LOW);
      digitalWrite(subtraction, LOW);
      digitalWrite(multiplication, LOW);
      digitalWrite(division, HIGH);
    }  
  }  
  
  //equal sign
  if (digitalRead(wire3) == LOW && digitalRead(wire4) == HIGH) {
    digitalWrite(equalSign, HIGH);
  }  
  
  //negative sign for subtraction
  if (digitalRead(wire3) == HIGH && digitalRead(wire4) == HIGH) {
    digitalWrite(negativeSign, HIGH);
  }
  
  //reset, check operatorEntered to avoid unecessary resets
  //that might mess up the program
  if (digitalRead(wire3) == LOW && digitalRead(wire4) == LOW && operatorEntered) {
    operatorEntered = false;
    digitalWrite(equalSign, LOW);
    digitalWrite(negativeSign, LOW);
    digitalWrite(addition, LOW);
    digitalWrite(subtraction, LOW);
    digitalWrite(multiplication, LOW);
    digitalWrite(division, LOW);
  }  
  
}
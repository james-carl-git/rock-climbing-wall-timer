/*
Rock Climbing Wall Timer
The below code is for controlling the PCB of this project: https://github.com/james-carl-git/rock-climbing-wall-timer
This code was written for an ATMega328P (could probably be adopted to others)
The microcontroller connects to a TLC5940 LED driver which drives all the LEDs for 2x 7 segment LED arrays, decimal point, and 3 button/LEDs

- The timer works by first pressing the bottom_button. This wakes up the microcontroller from a low power state and illuminates all the LEDs and displays 0.0 on the digit display.
- After pressing the bottom_button a second time, there is an audible countdown similar to everyone's favorite karting game ;)
- At the end of the countdown the timer will start incrementing, from 0-9.9 seconds the timer will show tenths of seconds and the decimal point. From 10-99 seconds, the tenths will not be displayed as there are not enough digits.
- The timer stops when the left or right button are pressed. Whichever button is pressed first will illuminate the corresponding LED to show who won. The timer will stop if it reaches 99 seconds. 
- This whole process can be started again by pressing the bottom_button again
- If no button is pressed for 
*/

#include "Tlc5940.h"
#include "LowPower.h"

#define right_led 4
#define left_led 7
#define bottom_led A1
#define right_button 5
#define left_button 8
#define bottom_button 2
#define speaker 6

bool awake = 0;
int button_pressed = 0;
int awake_timer = 0;

void wakeUp()
{
    // Handler for the pin interrupt.
}

void display_number(int left_number, int right_number, bool decimal_point){ //function to display times on the two 7-segment displays and the decimal point
  Tlc.clear();
    switch(right_number) { //each case below represents the encoding of the 7-segment digit for the number on the right
    case 0:
      Tlc.set(0,4095);Tlc.set(5,4095);Tlc.set(4,4095);Tlc.set(3,4095);Tlc.set(2,4095);Tlc.set(1,4095);
    break;
    case 1:
      Tlc.set(5,4095);Tlc.set(4,4095);
    break;
    case 2:
      Tlc.set(0,4095);Tlc.set(5,4095);Tlc.set(6,4095);Tlc.set(2,4095);Tlc.set(3,4095);
    break;
    case 3:
      Tlc.set(0,4095);Tlc.set(5,4095);Tlc.set(6,4095);Tlc.set(4,4095);Tlc.set(3,4095);
    break;
    case 4:
      Tlc.set(1,4095);Tlc.set(4,4095);Tlc.set(5,4095);Tlc.set(6,4095);
    break;
    case 5:
      Tlc.set(0,4095);Tlc.set(1,4095);Tlc.set(6,4095);Tlc.set(4,4095);Tlc.set(3,4095);
    break;
    case 6:
      Tlc.set(0,4095);Tlc.set(1,4095);Tlc.set(2,4095);Tlc.set(3,4095);Tlc.set(4,4095);Tlc.set(6,4095);
    break;
    case 7:
      Tlc.set(0,4095);Tlc.set(5,4095);Tlc.set(4,4095);
    break;
    case 8:
      Tlc.set(0,4095);Tlc.set(5,4095);Tlc.set(4,4095);Tlc.set(3,4095);Tlc.set(2,4095);Tlc.set(1,4095);Tlc.set(6,4095);
    break;
    case 9:
      Tlc.set(0,4095);Tlc.set(5,4095);Tlc.set(4,4095);Tlc.set(3,4095);Tlc.set(1,4095);Tlc.set(6,4095);
    break;
  }
  switch(left_number) { //each case below represents the encoding of the 7-segment digit for the number on the left
    case 0:
      Tlc.set(8,4095);Tlc.set(13,4095);Tlc.set(12,4095);Tlc.set(11,4095);Tlc.set(10,4095);Tlc.set(9,4095);
    break;
    case 1:
      Tlc.set(13,4095);Tlc.set(12,4095);
    break;
    case 2:
      Tlc.set(8,4095);Tlc.set(13,4095);Tlc.set(14,4095);Tlc.set(10,4095);Tlc.set(11,4095);
    break;
    case 3:
      Tlc.set(8,4095);Tlc.set(13,4095);Tlc.set(14,4095);Tlc.set(12,4095);Tlc.set(11,4095);
    break;
    case 4:
      Tlc.set(9,4095);Tlc.set(12,4095);Tlc.set(13,4095);Tlc.set(14,4095);
    break;
    case 5:
      Tlc.set(8,4095);Tlc.set(9,4095);Tlc.set(14,4095);Tlc.set(12,4095);Tlc.set(11,4095);
    break;
    case 6:
      Tlc.set(8,4095);Tlc.set(9,4095);Tlc.set(10,4095);Tlc.set(11,4095);Tlc.set(12,4095);Tlc.set(14,4095);
    break;
    case 7:
      Tlc.set(8,4095);Tlc.set(13,4095);Tlc.set(12,4095);
    break;
    case 8:
      Tlc.set(8,4095);Tlc.set(13,4095);Tlc.set(12,4095);Tlc.set(11,4095);Tlc.set(10,4095);Tlc.set(9,4095);Tlc.set(14,4095);
    break;
    case 9:
      Tlc.set(8,4095);Tlc.set(13,4095);Tlc.set(12,4095);Tlc.set(11,4095);Tlc.set(9,4095);Tlc.set(14,4095);
  }
  if(decimal_point)
    Tlc.set(15,4095);
  Tlc.update();
}

void play_tone(long frequency, long milliseconds){ //frequency of the tone, how long to play the tone
  for(int i=0;i<(frequency*milliseconds/1000);i++){
    digitalWrite(speaker, HIGH);   
    delayMicroseconds(500000/frequency);                     
    digitalWrite(speaker, LOW);
    delayMicroseconds(500000/frequency); 
  }
}

int timer_ten_seconds(){       //Function for displaying timer from 0.0 seconds to 9.9 seconds. Returns 1 if left button is pressed, 2 if right button is pressed
  for(int i=0;i<10;i++){
    for(int j=0;j<10;j++){
      display_number(i,j,true);
      for(int k=0;k<5;k++){
        delay(20);
        if((!digitalRead(left_button))||(!digitalRead(right_button))){
          delay(5);                                                 //debounce
          if(!digitalRead(left_button))
            return 1;                                               //exit timer funtion as left button is pressed
          else if(!digitalRead(right_button))
            return 2;                                               //exit timer funtion as right button is pressed
        }
      }      
    }
  }
  return 0;
}

int timer_ten_plus(){       //Function for displaying timer from 10 seconds to 99 seconds. Returns true if end button is pressed
  for(int i=1;i<10;i++){
    for(int j=0;j<10;j++){
      display_number(i,j,false);
      for(int k=0;k<50;k++){
        delay(20);
        if((!digitalRead(left_button))||(!digitalRead(right_button))){
          delay(5);                                                 //debounce
          if(!digitalRead(left_button))
            return 1;                                               //exit timer funtion as left button is pressed
          else if(!digitalRead(right_button))
            return 2;                                               //exit timer funtion as right button is pressed
        }
      }      
    }
  }
  return false;
}

void blink_winner(int winner){                //winner encoding: 1 left, 2 right
  int button_flash;
  if(winner == 1){                             //decide which LED to blink for the winner
    button_flash = left_led;
    digitalWrite(right_led, LOW);
  }
  else{
    button_flash = right_led;
    digitalWrite(left_led, LOW);
  }
  for(int i=0;i<30;i++){                      //blink for 30s or until bottom button is pressed
    for(int j=0;j<50;j++){
      if(!digitalRead(bottom_button)){
        delay(5);                             //debounce
        if(!digitalRead(bottom_button)){
          Tlc.clear();
          Tlc.update();
          digitalWrite(button_flash, LOW);
          delay(500);
          digitalWrite(bottom_led, HIGH);
          return;
        }
      }
      if(j==0)
        digitalWrite(button_flash, HIGH);
      if(j==25)
        digitalWrite(button_flash, LOW);
      delay(20);
    }
  }
  awake = false;
  awake_timer = 0;
  Tlc.clear();
  Tlc.update();
  SPCR &= ~(_BV(SPE)); //disable SPI for energy savings
  return;
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Tlc.init(0);
  SPCR &= ~(_BV(SPE)); //disable SPI for energy savings
  pinMode(right_led, OUTPUT);
  digitalWrite(right_led, LOW); 
  pinMode(left_led, OUTPUT);
  digitalWrite(left_led, LOW); 
  pinMode(bottom_led, OUTPUT);
  digitalWrite(bottom_led, LOW); 
  pinMode(speaker, OUTPUT);
  digitalWrite(speaker, LOW); 
  pinMode(speaker, OUTPUT);
  digitalWrite(speaker, LOW); 
  pinMode(right_button, INPUT_PULLUP);
  pinMode(left_button, INPUT_PULLUP);
  pinMode(bottom_button, INPUT_PULLUP);
}

// the loop function runs over and over again forever
void loop() {
  if(awake){
    delay(25);
    awake_timer++;
    if(awake_timer > 1200){  //used as timeout to put the microcontroller into deep sleep
      awake = false;
      awake_timer = 0;
      Tlc.clear();
      Tlc.update();
      digitalWrite(bottom_led, LOW);
      digitalWrite(left_led, LOW);
      digitalWrite(right_led, LOW);
      SPCR &= ~(_BV(SPE)); //disable SPI for energy savings
    }
  }  
  else{
    SPCR &= ~(_BV(SPE)); //disable SPI for energy savings
    attachInterrupt(digitalPinToInterrupt(bottom_button), wakeUp, LOW); //use bottom_button and an interrupt trigger to wake up microcontroller
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);                //for battery powered applications, put microcontroller in a deep sleep
    detachInterrupt(0); 
  }
  if(!digitalRead(bottom_button)){
    delay(5);                         //debounce
    if(!digitalRead(bottom_button)){  //bottom_button pressed starting audible and visual countdown sequence
      if(awake){
        display_number(0, 0, true);   //display 0.0
        digitalWrite(bottom_led, LOW);
        digitalWrite(left_led, HIGH);
        digitalWrite(right_led, HIGH);
        play_tone(220, 500);
        digitalWrite(left_led, LOW);
        digitalWrite(right_led, LOW);
        delay(500);
        digitalWrite(left_led, HIGH);
        digitalWrite(right_led, HIGH);
        play_tone(220, 500);
        digitalWrite(left_led, LOW);
        digitalWrite(right_led, LOW);
        delay(500);
        digitalWrite(left_led, HIGH);
        digitalWrite(right_led, HIGH);
        play_tone(220, 500);
        digitalWrite(left_led, LOW);
        digitalWrite(right_led, LOW);
        delay(500);
        digitalWrite(left_led, HIGH);
        digitalWrite(right_led, HIGH);
        play_tone(438, 1000);
        button_pressed = timer_ten_seconds(); //function that displays times 0.0 to 9.9, returns which button is pressed if left(1) or right(2) button is pressed between 0.0 and 9.9. Returns 0 if no button is pressed
        if(button_pressed){
          blink_winner(button_pressed);
        }
        else{
          button_pressed = timer_ten_plus(); //function that displays times 10 to 99, returns which button is pressed if left(1) or right(2) button is pressed between 10 and 99. Returns 0 if no button is pressed
          if(button_pressed){
            blink_winner(button_pressed);
          }
          else{
            digitalWrite(left_led, LOW);
            digitalWrite(right_led, LOW);
            Tlc.clear();
            Tlc.update();
            awake = false;
            awake_timer = 0;
          }
        }
      }
      else{                                 //Game is awake and ready to start, just need to press the bottom_button
        SPCR |= _BV(SPE); //enable SPI
        Tlc.clear();
        Tlc.update();
        digitalWrite(left_led, LOW);
        digitalWrite(right_led, LOW);
        awake = true;
        awake_timer = 0;
        delay(500);
        digitalWrite(bottom_led, HIGH);
      }
    }
  }         
}

#include "mbed.h"
#include <stdio.h>
#include <iostream>

using namespace std;

#define DISPLAY_DELAY 0.0012   // seconds

typedef struct SevenSegment
{
  int SevenSegmentMap[8];
};

static const SevenSegment Digits[] = {
// a, b, c, d, e, f, g, decimal
  {1, 1, 1, 1, 1, 1, 0, 0},    // 0
  {0, 1, 1, 0, 0, 0, 0, 0},    // 1
  {1, 1, 0, 1, 1, 0, 1, 0},    // 2
  {1, 1, 1, 1, 0, 0, 1, 0},    // 3
  {0, 1, 1, 0, 0, 1, 1, 0},    // 4
  {1, 0, 1, 1, 0, 1, 1, 0},    // 5
  {1, 0, 1, 1, 1, 1, 1, 0},    // 6
  {1, 1, 1, 0, 0, 0, 0, 0},    // 7
  {1, 1, 1, 1, 1, 1, 1, 0},    // 8
  {1, 1, 1, 1, 0, 1, 1, 0},    // 9
  {1, 1, 1, 0, 1, 1, 1, 0},    // A (10)
  {0, 0, 1, 1, 1, 1, 1, 0},    // B (11)
  {1, 0, 0, 1, 1, 1, 0, 0},    // C (12)
  {0, 1, 1, 1, 1, 0, 1, 0},    // D (13)
  {1, 0, 0, 1, 1, 1, 1, 0},    // E (14)
  {1, 0, 0, 0, 1, 1, 1, 0}     // F (15)
};

DigitalOut led(PG_13);    // just for testing... that the dev board is alive

// select which digit to light up 
DigitalOut Digit1(PG_5); // anode for Digit1 (leftmost) set to HIGH to activate Digit
DigitalOut Digit2(PG_7); // anode for Digit2 (middle)
DigitalOut Digit3(PC_6); // anode for Digit3 (rightmost)

// NOTE: LED display is common anode, so these need to be logic 0 to light up LED segment
DigitalOut SegmentA(PD_0);  // clockwise, starting from top segment
DigitalOut SegmentB(PD_1);
DigitalOut SegmentC(PD_2);
DigitalOut SegmentD(PD_3);
DigitalOut SegmentE(PD_4);
DigitalOut SegmentF(PD_5);
DigitalOut SegmentG(PD_6);  // middle segment
DigitalOut SegmentP(PD_7);  // decimal point 

// serial comms for debugging
Serial pc(USBTX, USBRX);  

void Display_Init(){
  // reset all pins, clear display
  SegmentA = !0;
  SegmentB = !0;
  SegmentC = !0;
  SegmentD = !0;
  SegmentE = !0;
  SegmentF = !0;
  SegmentG = !0;
  SegmentP = !0;
  Digit1 = 0;
  Digit2 = 0;
  Digit3 = 0;
}

void Display_Digit(int DigitPosition, int Number){
    // we need to invert logic because we're using a common anode display
    SegmentA = !Digits[Number].SevenSegmentMap[0];
    SegmentB = !Digits[Number].SevenSegmentMap[1];
    SegmentC = !Digits[Number].SevenSegmentMap[2];
    SegmentD = !Digits[Number].SevenSegmentMap[3];
    SegmentE = !Digits[Number].SevenSegmentMap[4];
    SegmentF = !Digits[Number].SevenSegmentMap[5];
    SegmentG = !Digits[Number].SevenSegmentMap[6];
    SegmentP = !Digits[Number].SevenSegmentMap[7];
    // we need to clear out the other digits before displaying the new number. 
    // otherwise, the same number will be displayed in all the digits. 
    switch (DigitPosition) {
      case 1: Digit1 = 1;
              Digit2 = 0;
              Digit3 = 0;
              break;
      case 2: Digit1 = 0;
              Digit2 = 1;
              Digit3 = 0;
              break;
      case 3: Digit1 = 0;
              Digit2 = 0;
              Digit3 = 1;
              break;                            
    }
    wait(DISPLAY_DELAY);
}

void Display_Number(int Number){
  int hundreds, tens, ones;

  // This is probably not the most efficient 
  // way of extracting the individual digits.
  // It works, but have a feeling there's a faster way.
  if (Number > 99){
    hundreds = Number / 100;
    Display_Digit(1, hundreds);
    wait(0.004);    // if delay is too short, LED will be dim. If too long, we'll have flickering

    tens = (Number % 100) / 10;
    Display_Digit(2, tens);
    wait(0.001);

    ones = (Number % 100) % 10;
    Display_Digit(3, ones);
    wait(0.0009);
  }

  if (Number > 9){  
    tens = (Number % 100) / 10;
    Display_Digit(2, tens);
    wait(0.003);

    ones = (Number % 100) % 10;
    Display_Digit(3, ones);
    wait(0.002);
  }
  else {
    Display_Digit(3, Number);
    wait(0.006);
  } 
}

int main() {
  // set usb serial
  pc.baud(115200);

  Display_Init();

  while(true){
    for (int i=0; i<1000; i++){
       Display_Number(i);
    }
    // Display_Number(125);
  }

  return 0;  
}
#include "mbed.h"
using namespace std;

// serial comms for debugging
Serial pc(USBTX, USBRX);  
DigitalOut led(PG_13);    // just for testing... that the dev board is alive

#define DISPLAY_DELAY 0.0002   // seconds

static const int Digits[] = {
  //abcdefgp    // 7-segment display + decimal point
  0b11111100,   // 0
  0b01100000,   // 1
  0b11011010,   // 2
  0b11110010,   // 3
  0b01100110,   // 4
  0b10110110,   // 5
  0b10111110,   // 6
  0b11100000,   // 7
  0b11111110,   // 8
  0b11110110,   // 9
  0b11101110,   // A (10)
  0b00111110,   // B (11)
  0b10011100,   // C (12)
  0b01111010,   // D (13)
  0b10011110,   // E (14)
  0b10001110    // F (15)
};

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

void Display_Clear(){
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
    SegmentA = !((Digits[Number] & 0b10000000) >> 7);
    SegmentB = !((Digits[Number] & 0b01000000) >> 6);
    SegmentC = !((Digits[Number] & 0b00100000) >> 5);
    SegmentD = !((Digits[Number] & 0b00010000) >> 4);
    SegmentE = !((Digits[Number] & 0b00001000) >> 3);
    SegmentF = !((Digits[Number] & 0b00000100) >> 2);
    SegmentG = !((Digits[Number] & 0b00000010) >> 1);
    SegmentP = !((Digits[Number] & 0b00000001) >> 0);

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
    wait(0.0025);    // if delay is too short, LED will be dim. If too long, we'll have flickering

    tens = (Number % 100) / 10;
    Display_Digit(2, tens);
    wait(0.002);

    ones = (Number % 100) % 10;
    Display_Digit(3, ones);
    wait(0.001);
  }

  if (Number > 9){  
    tens = (Number % 100) / 10;
    Display_Digit(2, tens);
    wait(0.001);

    ones = (Number % 100) % 10;
    Display_Digit(3, ones);
    wait(0.002);
  }
  else {
    Display_Digit(3, Number);
    wait(0.0025);
  } 
}

int main() {
  // set usb serial
  pc.baud(115200);

  Display_Clear();

  while(true){
    for (int i=0; i<1000; i++){
       Display_Number(i);
    }
    // Display_Number(125);
  }

  return 0;  
}
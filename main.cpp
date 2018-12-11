#include "mbed.h"
using namespace std;

// serial comms for debugging
Serial pc(USBTX, USBRX);  

#define DISPLAY_DELAY 0.0001   // 2 milliseconds

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
DigitalOut Digit1(PC_6); // anode for Digit1 (ones)
DigitalOut Digit2(PG_7); // anode for Digit2 (tens)
DigitalOut Digit3(PG_5); // anode for Digit3 (hundreds)

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
  SegmentA = SegmentB = SegmentC = SegmentD = SegmentE = SegmentF = SegmentG = SegmentP = !0;
  Digit3 = Digit2 = Digit1 = 0;
}

void Display_Digit(int DigitPosition, int Number){
    // common anode display, so invert logic to light up each segment
    SegmentA = !((Digits[Number] & 0b10000000) >> 7);
    SegmentB = !((Digits[Number] & 0b01000000) >> 6);
    SegmentC = !((Digits[Number] & 0b00100000) >> 5);
    SegmentD = !((Digits[Number] & 0b00010000) >> 4);
    SegmentE = !((Digits[Number] & 0b00001000) >> 3);
    SegmentF = !((Digits[Number] & 0b00000100) >> 2);
    SegmentG = !((Digits[Number] & 0b00000010) >> 1);
    SegmentP = !((Digits[Number] & 0b00000001) >> 0);

    // we need to clear out the other digits before displaying the new digit 
    // otherwise, the same number will be displayed in all the digits. 
    // common anode display, so logic 1 to light up the digit. 
    switch (DigitPosition) {
      case 1: Digit1 = 1;   // ones 
              Digit2 = 0;
              Digit3 = 0;
              break;
      case 2: Digit1 = 0;   // tens
              Digit2 = 1;
              Digit3 = 0;
              break;
      case 3: Digit1 = 0;   // hundreds
              Digit2 = 0;
              Digit3 = 1;
              break;                            
    }
    wait(DISPLAY_DELAY);
}

void Display_Number(int Number, uint32_t Duration_ms)
{
  int hundreds, tens, ones;
  uint32_t start_time, elapsed_time_ms = 0;
  Timer t;

  hundreds = Number / 100;
  tens = (Number % 100) / 10;
  ones = (Number % 100) % 10;

  t.start(); // start timer, we'll use this to setup elapsed time
  start_time = t.read();
  while (elapsed_time_ms < Duration_ms)
  {
    Display_Digit(3, hundreds);
    Display_Digit(2, tens);
    Display_Digit(1, ones);
    elapsed_time_ms = t.read_ms() - start_time;
  }
  t.stop(); // stop timer
}

int main() {
  // set usb serial
  pc.baud(115200);

  Display_Clear();

  while(true){
    for (int i=0; i<1000; i++){
       Display_Number(i, 250); // Number to display, Duration_ms
    }
  }

  return 0;  
}
//use for debug to speed up cycles
//#define MY_DEBUG

//you may need to disable millis in packages/MicroCore/hardware/avr/1.0.4/cores/microcore/core_settings.h

//default 9.6MHz / 8 = 1200000 or at 4.8MHz / 8 = 600000
//use 1.2MHz then 150000
#define F_CPU 150000UL

// ATtiny13 candle with long sleep mode, wake watchdog timer
// microCore: https://github.com/MCUdude/MicroCore
// Date: 12 October 2013

// http://forum.arduino.cc/index.php?topic=200590.0
// https://electronics.stackexchange.com/questions/74840/use-avr-watchdog-like-normal-isr

// ATMEL ATTINY 13 / ARDUINO
//
// ######### NOTES ########
// disable BOD
// set CPU clock to 1.2MHz should run OK with 1.8V
// ########################

//                      +-\/-+
// RST Ain0 (D 5) PB5  1|    |8  Vcc
//     Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1 SCK
//     Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1 MISO
//                GND  4|    |5  PB0 (D 0) pwm0 MOSI
//                      +----+

//                  +-------------+--o VCC
//                  |             |
//    47k          | |            |    
//                 | |            |
//                  |   +-\/-+    |
//             LED  +--1|    |8---+
//    LED    +--|<-----2|    |7   |                
//           |         3|    |6   |
//    100   | |    +---4|    |5   |
//          | |    |    +----+   ===
//           |     |             === 220pF
//           |     |              |
//          ---   ---            ---
//
// option: add LED on pin 3 (PB4) for heart beat every WDT interrupt

/* ISP 6pol
              +--------+
      MISO    |  o  o  |    VTG (VCC)
              ++       |
      SCK      | o  o  |    MOSI
              ++       |
      RST     |  o  o  |    GND
              +--------+
*/


#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management
#include <avr/wdt.h>      // Watchdog timer

#ifndef WDIF
  #define WDIF WDTIF
#endif
#ifndef WDIE
  #define WDIE WDTIE
#endif

#define USE_HEART_BEAT_LED

//for 8seconds 450 times will be one hour
//we have two intervals: on-time=4hours; off-time=20 hours
volatile uint16_t sec8_counter=0;
#ifndef MY_DEBUG
  //450x8 = 3600 seconds = 60 minutes
  #define HOUR_INTERVAL 450
  #define MAX_OFF_HOURS 20
  #define MAX_ON_HOURS 4
#else
  //DEBUG: 1x8 = 8 seconds : ON time = 8x2, OFF time=8*2 seconds
  #define HOUR_INTERVAL 1
  #define MAX_OFF_HOURS 2
  #define MAX_ON_HOURS 2
#endif
volatile uint8_t  on_hours=0;

volatile uint8_t  off_hours=0;
volatile uint8_t bLedIsOn=1; //we start in ON mode

const byte LED = 3;  // pin 2
#ifdef USE_HEART_BEAT_LED
const byte LED2 = 4; // pin 3 for heart beat LED
#endif

//for candle simulation via pwm
uint8_t PWM_CTR=0;    // 4 bit-Counter
uint8_t FRAME_CTR=0;  // 5 bit-Counter

uint8_t PWM_VAL=0;    // 4 bit-Register
uint8_t NEXTBRIGHT=0; // 4 bit-Register
uint8_t RAND=0;     // 5 bit Signal
uint8_t randflag=0;   // 1 bit Signal

/*
  32 Bit maximum length LFSR
  see http://www.ece.cmu.edu/~koopman/lfsr/index.html
  
  Using inverted values so the LFSR also works with zero initialisiation.
  
*/
uint8_t Rand(void) {
  static uint32_t Z;
  
    if (Z & 1)  { Z = (Z >> 1); }
    else        { Z = (Z >> 1) ^ 0x7FFFF159; }
  
  return (uint8_t)Z;
}

// watchdog interrupt
ISR (WDT_vect)
{
  sec8_counter++;

#ifdef USE_HEART_BEAT_LED  
  //flash heart beat LED
  digitalWrite(LED2, HIGH);
  delay(1);
  digitalWrite(LED2, LOW);
#endif
  
  if(sec8_counter>=HOUR_INTERVAL){
    sec8_counter=0;
    if(bLedIsOn==1){
      on_hours++;
      if(on_hours>=MAX_ON_HOURS){
        bLedIsOn=0; //switch to OFF mode
        off_hours=0;
      }
    }else{
      off_hours++;
      if(off_hours>=MAX_OFF_HOURS){
        bLedIsOn=1; //switch to ON mode
        on_hours=0;
      }
    }
  }
  wdt_reset();
}  // end of WDT_vect

void resetWatchdog ()
{
  // clear various "reset" flags
  MCUSR = 0;    
  // allow changes, disable reset, clear existing interrupt
  WDTCR = bit (WDCE) | bit (WDE) | bit (WDIF);
  // set interrupt mode and an interval (WDE must be changed from 1 to 0 here)
  ///WDTCR = bit (WDIE) | bit (WDP2) | bit (WDP1) | bit (WDP0);    // set WDIE, and 2 seconds delay
  WDTCR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  // pat the dog
  wdt_reset();  
}  // end of resetWatchdog
 

 
void goToSleep ()
{
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  //set all unused ports to output
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  PORTB = 0b000000; //all low
  noInterrupts ();       // timed sequence coming up
  resetWatchdog ();      // get watchdog ready
  sleep_enable ();       // ready to sleep
  interrupts ();         // interrupts are required now
  sleep_cpu ();        // sleep                
  sleep_disable ();      // precaution
  power_all_enable ();   // power everything back on
}  // end of goToSleep



void setup ()
{
  resetWatchdog ();  // do this first in case WDT fires

  CLKPR=_BV(CLKPCE);
  CLKPR=0;      // Set clk division factor to 1
  
  pinMode (LED, OUTPUT);
  #ifdef USE_HEART_BEAT_LED
  pinMode (LED2, OUTPUT);
  digitalWrite(LED2, LOW);
  #endif
  //disable BOD
//  MCUCR |= _BV(BODS) | _BV(BODSE);
}  // end of setup


//mimic candle: https://github.com/cpldcpu/CandleLEDhack/blob/master/Emulator/CandeflickerLED.c
void doCandle(){
if(F_CPU==150000){
  _delay_us(1e6/440/16*8);
}else{
  _delay_us(1e6/440/16);   // Main clock=440*16 Hz
}
  // PWM    
  PWM_CTR++;
  PWM_CTR&=0xf;   // only 4 bit
  
  if (PWM_CTR<=PWM_VAL) {
     digitalWrite (LED, HIGH);
  } else {
     digitalWrite (LED, LOW);
  }
  // FRAME
  if (PWM_CTR==0) 
  {
    FRAME_CTR++;
    FRAME_CTR&=0x1f;
    
    if ((FRAME_CTR&0x07)==0)  // generate a new random number every 8 cycles. In reality this is most likely bit serial
    {
      RAND=Rand()&0x1f;         
      if ((RAND&0x0c)!=0) randflag=1; else randflag=0;// only update if valid         
    }
    
    // NEW FRAME            
    if (FRAME_CTR==0)
    {
      PWM_VAL=NEXTBRIGHT; // reload PWM
      randflag=1;       // force update at beginning of frame
    }         
    
    if (randflag)
    {
      NEXTBRIGHT=RAND>15?15:RAND;   
    }
  }
}//end doCandle

void loop ()
{
  //sleep 20 hours and work 4 hours
  noInterrupts();
  if(bLedIsOn==1){
    doCandle();
  }else{
    digitalWrite (LED, LOW); //ensure LED is OFF
   goToSleep ();
  }
  interrupts();
}  // end of loop


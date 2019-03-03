/*
  CodeVisionAVR C Compiler
  (C) 1998-2000 Pavel Haiduc, HP InfoTech S.R.L.

  Prototypes for I2C bus master functions

  BEFORE #include -ING THIS FILE YOU
  MUST DECLARE THE I/O ADDRESS OF THE
  DATA REGISTER OF THE PORT AT WHICH
  THE I2C BUS IS CONNECTED AND
  THE DATA BITS USED FOR SDA & SCL

  EXAMPLE FOR PORTB:

    #asm
        .equ __i2c_port=0x18
        .equ __sda_bit=3
        .equ __scl_bit=4
    #endasm
    #include <i2c.h>
*/

#ifndef _I2C_INCLUDED_
#define _I2C_INCLUDED_

void I2C_Init(void);
unsigned char I2C_Start(void);
void I2C_Stop(void);
unsigned char I2C_Read(unsigned char ack);
unsigned char I2C_Write(unsigned char data);

#endif


/*
    FILE: Toner_Chip_Reset.ino
  AUTHOR: Ernesto Lorenz
 VERSION: 0.1
 PURPOSE: Reset chip for the Ricoh Aficio SP-204 toner cartridge
    DATE: 10.02.2018
  GITHUB: 

 Released to the public domain
 * 
*/

#include <extEEPROM.h>

// the contents to flash on the eeprom to reset the chip.
#include "reset_bin.h"


const char version[] = "0.1";

// eeprom address on the i2c bus:
// 0x53 ( 1010 A0=0 A1=1 A2=1 )

extEEPROM toner_chip(kbits_2, 1, 8, 0x53);

// dump the content of the eeprom
void dump(void) {
   for (unsigned pageaddr  = 0; pageaddr  < 241; pageaddr  += 16) {
       byte buffer[16]; // Hold a page  of EEPROM (16 bytes)
       char outbuf[6];  //Room for three hex digits and ':' and ' ' and '\0'
       sprintf(outbuf, "%03x: ", pageaddr);
       Serial.print(outbuf);
       toner_chip.read(pageaddr, buffer, 16);
       for (int j = 0; j < 16; j++) {
           if (j == 8) {
               Serial.print(" ");
           }
           sprintf(outbuf, "%02x ", buffer[j]);
           Serial.print(outbuf);            
       }
       Serial.print(" |");
       for (int j = 0; j < 16; j++) {
           if (isprint(buffer[j])) {
               char asciiSTR = toascii(buffer[j]);
               Serial.print( asciiSTR );
           }
           else {
               Serial.print('.');
           }
       }
       Serial.println("|");
   }  
}

// initialize serial connection and wait for user input.
// initialize i2c bus after user input.
void initialize(void) {

  Serial.begin(57600);
  Serial.println("Let's get started");
  
  byte i2cStat = toner_chip.begin(toner_chip.twiClock800kHz);
  
  if ( i2cStat != 0 ) {
    Serial.println("I2C bus init error");
  } else {
  Serial.println("I2C bus initalized");
  }
}

void reset(void) {
  toner_chip.write(0x0, chip_reset, chip_dump_len); //buffered write to the chip
  Serial.println();
  dump();
}

char getCommand()
{
  char c = '\0';
  if (Serial.available())
  {
    c = Serial.read();
  }
  return c;
}

void displayHelp()
{
  Serial.print(F("\nRICOH toner chip reset - "));
  Serial.println(version);
  Serial.println();
  Serial.println(F("Commands:"));
  Serial.println(F("\td = dump the toner chip contents"));
  Serial.println(F("\tr = reset the toner chip")); 
  Serial.println(F("\n\t? = help - this page"));
  Serial.println();
}

void setup(void) {
  Serial.begin(57600);
  Serial.println("Initializing I2C bus");
  byte i2cStat = toner_chip.begin(toner_chip.twiClock800kHz);
  if ( i2cStat != 0 ) {
    Serial.println("I2C bus init error");
  }
  else {
    Serial.println("I2C bus initalized");
  }
  displayHelp();
}

void loop() {
  char command = getCommand();
  switch (command)
  {
    case 'd':
      Serial.println("Toner chip dump:");
      dump();
      break;

    case 'r':
      Serial.println("Reseting the toner chip");
      reset();

    case '?':
      displayHelp;
      break;
      
    default:
      break;
  }
}

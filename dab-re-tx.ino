//
// 2104 = 0x3100
// 2200 = 0x0000
// dab.vol = 60
// audiodev = 6500
// rdsdev = 250
// pilotdev = 750
//

#include <EEPROM.h>

#include "pins.h"
#include "dabnextion.h"
#include "utils.h"
#include "fmtx.h"
#include "dabrx.h"
#include "cli.h"

#define EEPROMVALID 0x55aa

boolean heartbeat;
unsigned long int lastMillis;

typedef struct
{
    unsigned int txFreq;
    unsigned int dataValid;
} EEPROMDATA;

EEPROMDATA eeprom;

unsigned char calcChecksum(unsigned char *dPtr, unsigned int dataSize)
{
    unsigned char checksum;

    checksum = 0;
    while(dataSize)
    {
        checksum = checksum - *dPtr;
        dPtr++;
        dataSize--;
    }
}

void defaultEeprom()
{
    eeprom.txFreq = FSTART;
    eeprom.dataValid = EEPROMVALID;
}

void readEeprom(EEPROMDATA *ePtr)
{
    EEPROM.get(0, ePtr);
    if(ePtr -> dataValid != EEPROMVALID)
    {
        Serial.println("EEPROM loaded with defaults");
        defaultEeprom();
    }
    else
    {
        Serial.println("Got configuration from EEPROM");
    }
}

void writeEeprom()
{
    unsigned char eepromCs;
    unsigned char configCs;
    EEPROMDATA eData;

    readEeprom(&eData);
    eepromCs = calcChecksum((unsigned char *)&eData, sizeof(EEPROMDATA));

    configCs = calcChecksum((unsigned char *)&eeprom, sizeof(EEPROMDATA));

    if(configCs != eepromCs)
    {
        EEPROM.put(0, eeprom);
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.println("DAB re-transmitter");

    nextionInit();

    pinMode(__GPLED0_PIN, OUTPUT);  // overmod and hardware error LED
    pinMode(__GPLED1_PIN, OUTPUT);  // heartbeat LED

    lastMillis = millis();
    heartbeat = false;

    txSetup();
    rxSetup();

    nextionUpdate();

    Serial.println("Setup complete");

    cliSetup();

}

void setDefaults()
{
    defaultEeprom();
    txSetup();
}

void loop()
{
    unsigned long int millisNow;

    nextionLoop();
    cliLoop();    

    millisNow = millis();
    if(millisNow - lastMillis > 50)
    {
        if(heartbeat == true)
        {
            heartbeat = false;
            //digitalWrite(__GPLED1_PIN, HIGH);
        }
        else
        {
            heartbeat = true;
            //digitalWrite(__GPLED1_PIN, LOW);
        }
          
        lastMillis = millisNow;
        
        txLoop();
        rxLoop();
    }   
}

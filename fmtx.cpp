#include <Adafruit_Si4713.h>

#include "pins.h"
#include "utils.h"
#include "fmtx.h"

char rdsText[RDSBUFFERSIZE];
int rdsPage;
int rdsTicks;

int txFreq;

Adafruit_Si4713 tx;

void loadRDSText(char *newTxt)
{
    int c;

    Serial.print("Set RDS text to '");
    Serial.print(newTxt);
    Serial.println("'");

    c = 0;
    while(newTxt[c] != '\0' && c < RDSBUFFERSIZE)
    {
        rdsText[c] = newTxt[c];
        c++;
    }

    while(c < RDSBUFFERSIZE)
    {
        rdsText[c] = '.';
        c++;
    }

    rdsPage = 0;
    rdsTicks = RDSREFRESHTIME;
}

void showRdsPage()
{
    char rdsMsg[RDSPAGESIZE + 1];
    char *rdsPtr;
    int c;

//    Serial.print("RDS page ");
//    Serial.println(rdsPage);

    rdsPtr = &rdsText[RDSPAGESHIFT * rdsPage];

    for(c = 0; c < RDSPAGESIZE; c++)
    {
        rdsMsg[c] = rdsPtr[c];
    }
    rdsMsg[c] = '\0';
//    Serial.print("Text '");
//    Serial.print(rdsMsg);
//    Serial.println("'");
    
    tx.setRDSstation(rdsMsg);

    rdsPage++;
    if(rdsPage == RDSPAGES)
    {
        rdsPage = 0;
    }
}

void dumpTxProps(uint16_t startProp, uint16_t numProp)
{
    char propTxt[32];
    int c;

    c = 0;
    while(c != numProp)
    {
        if(c % 8 == 0)
        {
            sprintf(propTxt, "\n0x%04x    ", startProp);
            Serial.print(propTxt);
        }
        
        sprintf(propTxt, "%04x  ", tx.getProperty(startProp));
        Serial.print(propTxt);
        
        c++;
        startProp++;
    }
}

void dumpAllTxProps()
{
    dumpTxProps(0x2100, 8);
    dumpTxProps(0x2200, 6);
    dumpTxProps(0x2300, 5);
    dumpTxProps(0x2c00, 8);

    Serial.println("");
}

void setDeviation(char *p, char *msg, int propName)
{
      char devMsg[30];
      unsigned int deviation;
      
      if(*p != '\0')
      {
          deviation = atoi(p);
          tx.setProperty(propName, deviation);
      }

      deviation = tx.getProperty(propName);
      sprintf(devMsg, "%s deviation - %d", msg, deviation);
      Serial.println(devMsg);    
}

void incFTxMHz()
{
    txFreq = txFreq + 100;
    if(txFreq >= 10800)
    {
        txFreq = 8800;
    }

    setFTx(txFreq);
}

void incFTxKHz()
{
    int khz;
    int mhz;

    mhz = txFreq / 100;
    khz = txFreq % 100;

    khz = khz + 10;
    if(khz >= 100)
    {
        khz = 0;
    }

    txFreq = (mhz * 100) + khz;

    setFTx(txFreq);
}

void setFTx(int f)
{
    char fStr[16];

    txFreq = f;
    
    fToString(txFreq, fStr);    
    Serial.print("Frequency ");
    Serial.println(fStr);

    tx.tuneFM(txFreq);
}

int getFTx()
{
    return txFreq;
}

unsigned int txGetProp(unsigned int prop)
{
    return tx.getProperty(prop);
}

void txSetProp(unsigned int prop, unsigned int propVal)
{
    tx.setProperty(prop, propVal);
}

void txSetup()
{    
    char fStr[16];
    unsigned int propVal;

    Serial.println("FM transmitter");
    tx = Adafruit_Si4713(__FMRESET_PIN);
    if(!tx.begin())
    {
        Serial.println("Couldn't find transmitter?");
        hwError();
    }

    Serial.print("FM transmitter Si47");
    Serial.println(tx.getRev());

    setFTx(FSTART);

    tx.setTXpower(115);  // dBuV, 88-115 max

    // This will tell you the status in case you want to read it from the chip
    //tx.readTuneStatus();
    //Serial.print("\tCurr freq: "); 
    //Serial.println(tx.currFreq);
    //Serial.print("\tCurr ANTcap:"); 
    //Serial.println(tx.currAntCap);

    // begin the RDS/RDBS transmission
    tx.beginRDS();
    tx.setRDSstation("ArseFM");
    tx.setRDSbuffer( "Isle of Man");

    // 50us pre-emphasis
    Serial.println("Pre-emphasis - 50uS");
    tx.setProperty(SI4713_PROP_TX_PREEMPHASIS, PEMPH_50US);

    tx.setProperty(SI4713_PROP_TX_LINE_LEVEL_INPUT_LEVEL, INPUT_LEVEL);
    
    // audio deviation
    Serial.print("Audio deviation - ");
    tx.setProperty(SI4713_PROP_TX_AUDIO_DEVIATION, AUDIO_DEV);
    propVal = tx.getProperty(SI4713_PROP_TX_AUDIO_DEVIATION);
    sprintf(fStr, "%d", propVal);
    Serial.println(fStr);

    // RDS deviation
    Serial.print("RDS deviation - ");
    tx.setProperty(SI4713_PROP_TX_RDS_DEVIATION, RDS_DEV);
    propVal = tx.getProperty(SI4713_PROP_TX_RDS_DEVIATION);
    sprintf(fStr, "%d", propVal);
    Serial.println(fStr);

    // Pilot deviation
    Serial.print("Pilot deviation - ");
    tx.setProperty(SI4713_PROP_TX_PILOT_DEVIATION, PILOT_DEV);
    propVal = tx.getProperty(SI4713_PROP_TX_PILOT_DEVIATION);
    sprintf(fStr, "%d", propVal);
    Serial.println(fStr);

    // RDS data block split
    // Send PS 87.5% of the time - default is 50%
    tx.setProperty(SI4713_PROP_TX_RDS_PS_MIX, 0x05);

    // RDS PI code
    tx.setProperty(SI4713_PROP_TX_RDS_PI, RDS_PI);

    // Scrolling RDS stuff
    rdsTicks = RDSREFRESHTIME;
    rdsPage = 0;

    // Over modulation LED
    // pinMode(__GPLED0_PIN, OUTPUT);
    digitalWrite(__GPLED0_PIN, LOW);
}

void txLoop()
{
    tx.readASQ();

    // overmod
    if((tx.currASQ & 0x04) == 0x04)
    {
        digitalWrite(__GPLED0_PIN, HIGH);
    }
    else
    {
        digitalWrite(__GPLED0_PIN, LOW);
    }

    // scroll RDS
    if(rdsTicks)
    {
        rdsTicks--;
    }
    else
    {
        showRdsPage();
        rdsTicks = RDSREFRESHTIME;
    }
}

#include <Arduino.h>
#include <Adafruit_Si4713.h>

#include "dabrx.h"
#include "fmtx.h"
#include "cli.h"

char cliBuffer[80];
char *cliPtr;
int charsAvailable;

CLICOMMAND cliCmd[] =
{
    { "help", helpCmd },
    { "dabinfo", dabInfoCmd },
    { "dabnext", dabNextCmd },
    { "dabscan", dabScanCmd },
    { "dabservice", dabServiceCmd },
    { "dabstatus", dabStatusCmd },
    { "dabtune", dabTuneCmd },
    { "dabvol", dabVolCmd },
    { "fmprop", propCmd },
    { "rdsdev", rdsDevCmd },
    { "audiodev", audioDevCmd },
    { "pilotdev", pilotDevCmd },
    { "?", helpCmd },
    { "", NULL }
};

void helpCmd(char *p)
{
    CLICOMMAND *cmdPtr;

    Serial.println("Available commands:-");

    cmdPtr = cliCmd;
    while(cmdPtr -> fn != NULL)
    {
        Serial.print("  ");
        Serial.println(cmdPtr -> name);
        cmdPtr++;
    }
}

void dabNextCmd(char *)
{
    dabNextService();
}

void dabStatusCmd(char *p)
{
    dabStatus();
}

void dabVolCmd(char *p)
{
    char volMsg[32];
    int dabVol;
    
    if(*p != '\0')
    {
        dabVol = atoi(p);
        dabSetVolume(dabVol);
    }

    dabVol = dabGetVolume();

    sprintf(volMsg, "DAB volume - %d", dabVol);
    Serial.println(volMsg);
}

void dabServiceCmd(char *p)
{
    int serv;
    char *serviceLabel;

    if(*p == '\0')
    {
        dabGetService(); // GetService puts info out on debug port
    }
    else
    {
        serv = atoi(p);
        dabSetService(serv);
        serviceLabel = dabGetService();
        loadRDSText(serviceLabel);
    }
}

void dabInfoCmd(char *p)
{
    dabGetEnsemble();
}

void dabTuneCmd(char *p)
{
    dabTune((uint8_t)atoi(p));
}

void dabScanCmd(char *p)
{
    dabScan();
}

void propCmd(char *p)
{
    char *p1;
    char *p2;
    char *endPtr;
    unsigned int prop;
    unsigned int propVal;
    char propMsg[30];

    p1 = strtok(p, " ");
    if(p1 == NULL)
    {
        dumpAllTxProps();
    }
    else
    {
        prop = strtol(p1, &endPtr, 16);
      
        p2 = strtok(NULL, " ");
        if(p2 != NULL)
        {
          propVal = strtol(p2, &endPtr, 16);
          txSetProp(prop, propVal);
        }

        propVal = txGetProp(prop);
        sprintf(propMsg, "Property 0x%04x - 0x%04x", prop, propVal);
        Serial.println(propMsg);    
    }
}

void audioDevCmd(char *p)
{
    // Audio deviation
    setDeviation(p, "Audio", SI4713_PROP_TX_AUDIO_DEVIATION);
}

void rdsDevCmd(char *p)
{
    setDeviation(p, "RDS", SI4713_PROP_TX_RDS_DEVIATION);
}

void pilotDevCmd(char *p)
{
    setDeviation(p, "Pilot", SI4713_PROP_TX_PILOT_DEVIATION);
}

void doCommand()
{
    int c;
    char *paramPtr;
    
    c = 0;
    while(cliCmd[c].fn != NULL && strncmp(cliBuffer, cliCmd[c].name, strlen(cliCmd[c].name)) != 0)
    {
        c++;
    }

    if(cliCmd[c].fn == NULL)
    {
        Serial.println("Bad command");
    }
    else
    {
        paramPtr = cliBuffer;
        while(*paramPtr != ' ' && *paramPtr != '\0')
        {
            paramPtr++;
        }

        if(*paramPtr == ' ')
        {
            while(*paramPtr == ' ' && *paramPtr != '\0')
            {
                paramPtr++;
            }
        }

        cliCmd[c].fn(paramPtr);
    }
}

void cliSetup()
{
    cliPtr = cliBuffer;
    Serial.println(CLIHELLO);
    Serial.print(CLIPROMPT);
}

void cliLoop()
{
    charsAvailable = Serial.available();
    while(charsAvailable > 0)
    {
        *cliPtr = Serial.read();
        if(*cliPtr == '\n')
        {
            Serial.println("");
            *cliPtr = '\0';
            doCommand();

            cliPtr = cliBuffer;
            Serial.print(CLIPROMPT);
        }
        else
        {
            if(*cliPtr != '\r')
            {
                cliPtr++;
            }
        }

        charsAvailable = Serial.available();
    }
}


#include <Arduino.h>

#include <SPI.h>
#include <DABShield.h>

#include "pins.h"
#include "utils.h"
#include "fmtx.h"
#include "dabrx.h"

DAB Dab;
int serviceIndex;
int dabVolume;

uint8_t dabGetFreqIndex()
{
    return Dab.freq_index;
}

char *dabGetEnsemble(void)
{
    return Dab.Ensemble;
}

void dabGetEnsembleInfo(void)
{
  char infoMsg[80];
  uint8_t i;

  Serial.print(F("Ensemble Freq "));
  sprintf(infoMsg, "%02d\t %03d.", Dab.freq_index, (uint16_t)(Dab.freq_khz(Dab.freq_index) / 1000));
  Serial.print(infoMsg);
  sprintf(infoMsg, "%03d MHz", (uint16_t)(Dab.freq_khz(Dab.freq_index) % 1000));
  Serial.print(infoMsg);

  Serial.print(F("\n"));
  Serial.print(Dab.Ensemble);
  Serial.print(F("\n"));

  Serial.print(F("\nServices:\n"));
  for (i = 0; i < Dab.numberofservices; i++)
  {
    sprintf(infoMsg, "  %d\t0x%04x\t0x%04x\t", i, Dab.service[i].ServiceID, Dab.service[i].CompID);
    Serial.print(infoMsg);
    Serial.println(Dab.service[i].Label);
  }
  Serial.print(F("\n"));
}

int dabSignal()
{
    return Dab.signalstrength;
}

void dabStatus(void)
{
    char strbuf[32];
    uint32_t fkhz;

    Dab.status();

    fkhz = Dab.freq_khz(Dab.freq_index);

    sprintf(strbuf, "Freq - %03d.", (uint16_t)(fkhz / 1000));
    Serial.print(strbuf);
    sprintf(strbuf, "%03d MHz\n", (uint16_t)(fkhz % 1000));
    Serial.print(strbuf);

    sprintf(strbuf, "RSSI - %d\n", Dab.signalstrength);
    Serial.print(strbuf);

    sprintf(strbuf, "SNR - %d dB\n", Dab.snr);
    Serial.print(strbuf);

    sprintf(strbuf, "CNR - %d dB\n", Dab.cnr);
    Serial.print(strbuf);

    sprintf(strbuf, "FIC quality - %d %%\n", Dab.ficquality);
    Serial.print(strbuf);

    sprintf(strbuf, "FIB error count - %d\n", Dab.fiberrorcount);
    Serial.print(strbuf);
    sprintf(strbuf, "CU count - %d\n", Dab.cucount);
    Serial.print(strbuf);    
}

void dabScan(void)
{
  uint8_t freq_index;
  char freqstring[32];

  for (freq_index = 0; freq_index < DAB_FREQS; freq_index++)
  {
    Serial.print(F("\nScanning Freq "));
    sprintf(freqstring, "%02d\t %03d.", freq_index, (uint16_t)(Dab.freq_khz(freq_index) / 1000));
    Serial.print(freqstring);
    sprintf(freqstring, "%03d MHz", (uint16_t)(Dab.freq_khz(freq_index) % 1000));
    Serial.print(freqstring);
    dabTune(freq_index);
  }
  Serial.print(F("\n\n"));
}

boolean dabValid()
{
    return Dab.servicevalid();
}

boolean dabTune(uint8_t freqIndex)
{    
    boolean rtn;

    rtn = false;
    
    Dab.tune(freqIndex);
    if(Dab.servicevalid() == true)
    {
        rtn = true;
        dabStatus();
        dabGetEnsembleInfo();
    }
    else
    {
        Serial.println("No DAB ensemble found");
    }

    return rtn;
}

boolean dabNextChannel()
{  
}

char *dabGetService()
{
    char serviceMsg[80];

    sprintf(serviceMsg, "%d: service ID = 0x%04x, component ID = 0x%04x, label = %s",
              serviceIndex, Dab.service[serviceIndex].ServiceID, Dab.service[serviceIndex].CompID, Dab.service[serviceIndex].Label);
    Serial.println(serviceMsg);

    return Dab.service[serviceIndex].Label;
}

void dabSetService(int service)
{
    serviceIndex = service;
    Dab.set_service(service);

    Serial.print("DAB service: ");
    Serial.println(Dab.service[service].Label);
}

void dabNextService()
{
    serviceIndex++;
    if(serviceIndex == Dab.numberofservices)
    {
        serviceIndex = 0;
    }

    dabSetService(serviceIndex);
}

void dabSetVolume(int dabVol)
{
    dabVolume = dabVol;
    Dab.vol(dabVolume);
}

int dabGetVolume()
{
    return dabVolume;
}

void DABSpiMsg(unsigned char *data, uint32_t len)
{
  int c;
  char hex[16];
  unsigned char *dPtr;
  
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));    //2MHz for starters...
  digitalWrite (__DABCS_PIN, LOW);
  SPI.transfer(data, len);
  digitalWrite (__DABCS_PIN, HIGH);
  SPI.endTransaction();
}

void ServiceData(void)
{
    Serial.print(Dab.ServiceData);
    Serial.print(F("\n"));
}

void rxLoop()
{
    Dab.task();
}

void rxSetup()
{
    int service;
    char msg[16];
    
    pinMode(__DABCS_PIN, OUTPUT);
    digitalWrite(__DABCS_PIN, HIGH);
    SPI.begin();
    Dab.setCallback(ServiceData);
    Dab.begin(__DABRESET_PIN, __DABPWREN_PIN, __DABINTERRUPT_PIN);

    if(Dab.error != 0)
    {
      Serial.print(F("ERROR: "));
      Serial.print(Dab.error);
      Serial.print(F("\nCouldn't find receiver?\n"));
      hwError();
    }

    Serial.print("DAB receiver Si");
    Serial.println(Dab.PartNo);

    // BBC DAB
    if(dabTune(29) != true)   // 225.648MHz
    {
        Serial.println("Can't find BBC DAB ensemble!");
        dabGetEnsembleInfo();
        hwError();
    }

    // Find Radio 4
    service = 0;
    while(Dab.service[service].ServiceID != 0xc224)
    {
        service++;
    }
    
    dabSetService(service);      // BBC R4

    Serial.print("DAB service ID: ");
    sprintf(msg, "0x%04x", Dab.service[service].ServiceID);
    Serial.println(msg);

    Serial.print("DAB component id: ");
    sprintf(msg, "0x%04x", Dab.service[service].CompID);
    Serial.println(msg);

    dabVolume = 60;
    Dab.vol(dabVolume);
    loadRDSText(Dab.service[service].Label);
}

#include <Nextion.h>

#include "pins.h"
#include "utils.h"
#include "fmtx.h"
#include "dabrx.h"

SoftwareSerial HMISerial(__NXTRXD_PIN, __NXTTXD_PIN);

NexDSButton btnScan      = NexDSButton(0, 1, "btnScan");
NexDSButton btnSpare1    = NexDSButton(0, 2, "btnSpare1");
NexDSButton btnKHz       = NexDSButton(0, 3, "btnKHz");
NexProgressBar barSignal = NexProgressBar(0, 4, "barProgress");
NexDSButton btnEnsemble  = NexDSButton(0, 5, "btnEnsemble");
NexDSButton btnService   = NexDSButton(0, 6, "btnService");
NexDSButton btnFreq      = NexDSButton(0, 7, "btnFreq");
NexText txtStatus        = NexText(0, 8, "txtStatus");
    
NexTouch *nexListenList[] =
{
    &btnEnsemble,
    &btnService,
    &btnFreq,
    &btnScan,
    &btnKHz,
    NULL
};

void btnFreqUpdate()
{
    char fStr[20];
    int f;
    
    f = getFTx();
    
    fToString(f, fStr);
    btnFreq.setText(fStr);
}

void barSignalUpdate()
{
    int rssi;

    rssi = dabSignal();
    barSignal.setValue(rssi);
}

void btnEnsembleCallback(void *ptr)
{
}

void btnServiceCallback(void *ptr)
{
    dabNextService();
    btnService.setText(dabGetService());
}

void btnFreqCallback(void *ptr)
{
    incFTxMHz();
    btnFreqUpdate();
}

void btnScanCallback(void *ptr)
{
    uint8_t freqIndex;

    freqIndex = dabGetFreqIndex();
    dabTune(freqIndex);
}

void btnKHzCallback(void *ptr)
{
    incFTxKHz();
    btnFreqUpdate();
}

void nextionInit()
{
    nexInit();

    btnEnsemble.attachPush(btnEnsembleCallback, &btnEnsemble);
    btnService.attachPush(btnServiceCallback, &btnService);
    btnFreq.attachPush(btnFreqCallback, &btnFreq);
    btnScan.attachPush(btnScanCallback, &btnScan);
    btnKHz.attachPush(btnKHzCallback, &btnKHz);

    btnEnsemble.setText("Init");
    btnService.setText("Init");
    btnFreq.setText("Init");
}

void nextionUpdate()
{
    if(dabValid() == true)
    {
        btnEnsemble.setText(dabGetEnsemble());
        btnService.setText(dabGetService());
        barSignalUpdate();
    }
    else
    {
        btnEnsemble.setText("Ensemble");
        btnService.setText("Service");
        barSignal.setValue(0);
    }

    btnFreqUpdate();
}

void nextionLoop()
{
    nexLoop(nexListenList);
    barSignalUpdate();
}

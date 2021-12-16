char *dabGetEnsemble(void);
uint8_t dabGetFreqIndex(void);
void dabGetEnsembleInfo(void);
void dabStatus(void);
void dabScan(void);
boolean dabTune(uint8_t freqIndex);
boolean dabNextChannel(void);
void dabSetService(int service);
char *dabGetService(void);
void dabNextService(void);
int dabGetVolume(void);
void dabSetVolume(int);
boolean dabValid(void);
int dabSignal(void);
void DABSpiMsg(unsigned char *data, uint32_t len);
void ServiceData(void);
void rxLoop(void);
void rxSetup();

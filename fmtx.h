void loadRDSText(char *newTxt);
void showRdsPage();
void dumpTxProps(uint16_t startProp, uint16_t numProp);
void dumpAllTxProps();
void setDeviation(char *p, char *msg, int propName);
void incFTxMHz(void);
void incFTxKHz(void);
void setFTx(int);
int getFTx(void);
unsigned int txGetProp(unsigned int prop);
void txSetProp(unsigned int prop, unsigned int propVal);
void txSetup();
void txLoop();

#define RDSBUFFERSIZE  16
#define RDSPAGESIZE    8
#define RDSPAGESHIFT   1
#define RDSPAGES       8
#define RDSREFRESHTIME 100    // 100's of milliseconds...

#define PEMPH_50US 1        // 50us pre-emphasis - magic number from datasheet
#define AUDIO_DEV  6500     // deviation values in 10's of kHz - total 75kHz
#define RDS_DEV    250
#define PILOT_DEV  750
#define INPUT_LEVEL 0x3100  // 250mV p-p input

#define RDS_PI   0xbeef

#define FSTART   10230      // 10230 == 102.30 MHz

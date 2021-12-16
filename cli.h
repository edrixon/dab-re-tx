#define CLIHELLO "Command line interpretter version 1.0"
#define CLIPROMPT "CLI> "

typedef struct
{
    char *name;
    void (*fn)(char *param);
} CLICOMMAND;

void helpCmd(char *p);
void dabNextCmd(char *);
void dabStatusCmd(char *p);
void dabVolCmd(char *p);
void dabServiceCmd(char *p);
void dabInfoCmd(char *p);
void dabTuneCmd(char *p);
void dabScanCmd(char *p);
void propCmd(char *p);
void audioDevCmd(char *p);
void rdsDevCmd(char *p);
void pilotDevCmd(char *p);
void doCommand();
void cliLoop();
void cliSetup();

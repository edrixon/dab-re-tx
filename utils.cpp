#include <Arduino.h>

#include "pins.h"
#include "utils.h"

void fToString(int f, char *fStr)
{
    sprintf(fStr, "%03d.%02d MHz", f / 100, f % 100);
}

void hwError()
{
    while(1)
    {
        digitalWrite(__GPLED0_PIN, HIGH);
        delay(500);
        digitalWrite(__GPLED0_PIN, LOW);
        delay(500);
    }
}

#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include <string>
#include <vector>
#include "Arduino.h"


// IO settings

class display
{
public:
    int id, busy, rst, dc, cs, btn;
    int lastPressTime;
    uint8_t image[2024];
    std::vector<uint8_t> action;

    display(int id, int busy, int rst, int dc, int cs, int btn);

    void select();
    void deselect();
};

int isEPD_W21_BUSY(display& d);
void EPD_W21_RST_0(display& d);
void EPD_W21_RST_1(display& d);
void EPD_W21_DC_0(display& d);
void EPD_W21_DC_1(display& d);
void EPD_W21_CS_0(display& d);
void EPD_W21_CS_1(display& d);

void SPI_Write(unsigned char value);

void EPD_W21_WriteDATA(unsigned char datas, display& d);
void EPD_W21_WriteCMD(unsigned char command, display& d);

#endif

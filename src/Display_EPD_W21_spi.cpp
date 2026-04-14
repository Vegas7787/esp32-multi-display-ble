#include "Display_EPD_W21_spi.h"
#include <SPI.h>

display::display(int id, int busy, int rst, int dc, int cs ,int btn)
{
	this->id = id;
	this->busy = busy;
	this->rst = rst;
	this->dc = dc;
	this->cs = cs;
	this->btn = btn;
}


void display::select() {
    digitalWrite(cs, LOW);
}

void display::deselect() {
    digitalWrite(cs, HIGH);
}

int isEPD_W21_BUSY(display& d)
{
	return digitalRead(d.busy);
}

void EPD_W21_RST_0(display& d)
{
	digitalWrite(d.rst, LOW);
}
void EPD_W21_RST_1(display& d)
{
	digitalWrite(d.rst, HIGH);
}

void EPD_W21_DC_0(display& d)
{
	digitalWrite(d.dc, LOW);
}
void EPD_W21_DC_1(display& d)
{
	digitalWrite(d.dc, HIGH);
}

void EPD_W21_CS_0(display& d)
{
	digitalWrite(d.cs, LOW);
}
void EPD_W21_CS_1(display& d)
{
	digitalWrite(d.cs, HIGH);
}

// SPI write command
void EPD_W21_WriteCMD(unsigned char command, display& d)
{
	EPD_W21_CS_0(d);
	EPD_W21_DC_0(d); // D/C#   0:command  1:data
	SPI_Write(command);
	EPD_W21_CS_1(d);
}
// SPI write data
void EPD_W21_WriteDATA(unsigned char datas, display& d)
{
	EPD_W21_CS_0(d);
	EPD_W21_DC_1(d); // D/C#   0:command  1:data
	SPI_Write(datas);
	EPD_W21_CS_1(d);
}

// SPI write byte
void SPI_Write(unsigned char value)
{
	SPI.transfer(value);
}
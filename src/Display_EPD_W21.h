#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_


#define EPD_WIDTH   88 
#define EPD_HEIGHT  184
#define EPD_ARRAY  EPD_WIDTH*EPD_HEIGHT/8  


//Full screen update display
void EPD_Update(display& d);
void EPD_Update_NoWait(display& d);
void Epaper_READBUSY(display& d);
void EPD_HW_Init(display& d); 
void EPD_HW_Init_180(display& d);	
void EPD_WhiteScreen_ALL(const unsigned char *datas, display& d);
void EPD_WhiteScreen_White(display& d);
void EPD_WhiteScreen_Black(display& d);
void EPD_DeepSleep(display& d);
//Partial update display  
void EPD_SetRAMValue_BaseMap(const unsigned char * datas, display& d);
void EPD_Dis_PartAll(const unsigned char * datas, display& d);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Dis_Part_Time(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
	                       unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
												 unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
												 unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
											   unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
												 unsigned int PART_COLUMN,unsigned int PART_LINE, display& d
	                      );	
//Fast update display
void EPD_HW_Init_Fast(display& d);
void EPD_Load_Data(const unsigned char *datas, display& d);
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas, display& d);
//4 Gray update display
void EPD_HW_Init_4Gray(display& d);
void EPD_WhiteScreen_ALL_4Gray(const unsigned char *datas, display& d);										 
//GUI display					 
void EPD_HW_Init_GUI(display& d);
void EPD_Display(unsigned char *Image, display& d); 
												 
#endif




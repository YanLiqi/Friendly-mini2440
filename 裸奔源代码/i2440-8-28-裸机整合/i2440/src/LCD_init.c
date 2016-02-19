#include "option.h"
#include "def.h"
#include "2440addr.h"
#include "2440lib.h"

#define LCD_XSIZE  LCD_WIDTH
#define LCD_YSIZE  LCD_HEIGHT
#define SCR_XSIZE  LCD_WIDTH
#define SCR_YSIZE  LCD_HEIGHT

volatile static unsigned short LCD_BUFFER[SCR_YSIZE][SCR_XSIZE];  //����320�У�240�е����飬���ڴ����ʾ����


/**************************************************************
 TFT LCD  *����Ļ�ϻ�ͼ*
**************************************************************/

void Pait_Bmp(int x0,int y0,int h,int l,const unsigned char *bmp)
{
    int x,y;
    U32 c;
    int p = 0;
	
    for( y = 0 ; y < l ; y++ )
    {
    	for( x = 0 ; x < h ; x++ )
    	{
    	    c = bmp[p+1] | (bmp[p]<<8) ;

	    if ( ( (x0+x) < SCR_XSIZE) && ( (y0+y) < SCR_YSIZE) )
	        LCD_BUFFER[y0+y][x0+x] = c ;

    	    p = p + 2 ;
    	}
    }
}

/**************************************************************
 TFT LCD   *���ȫ��Ϊĳһ��ɫ*
**************************************************************/

void Lcd_ClearScr( U16 c)
{
    unsigned int x,y ;
		
    for( y = 0 ; y < SCR_YSIZE ; y++ )
    {
    	for( x = 0 ; x < SCR_XSIZE ; x++ )
    	{
		LCD_BUFFER[y][x] = c ;
    	}
    }
}


/**************************************************************
 TFT LCD    *�˿ڳ�ʼ��*
**************************************************************/

void Lcd_Port_Init( void )
{
    rGPCUP=0xffffffff; // Disable Pull-up register
    rGPCCON=0xaaaa02a8; //Initialize VD[7:0],VM,VFRAME,VLINE,VCLK

    rGPDUP=0xffffffff; // Disable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]
}




/**************************************************************
 TFT LCD����ģ���ʼ��//�˺���û�������ι���
**************************************************************/
void LCD_Init(void)
{
#define	M5D(n)	((n)&0x1fffff)
#define LCD_ADDR ((U32)LCD_BUFFER)
     //con1[6:5]��ʾģʽѡ��Ϊ TFT LCD��� ģʽ  [4:1]ѡ��Ϊ TFT��16bpp ����ģʽ	
	rLCDCON1 = (LCD_PIXCLOCK << 8) | (3 <<  5) | (12 << 1);
	//con2[23:14]���ø߶�320  VSPW[5:0]������Ӧ����Ŀ��; VFPD[13:6]��VBPD[31:24]��������ʱʱ��;  ѡ��option.h�ļ���T35�������
   	rLCDCON2 = (LCD_UPPER_MARGIN << 24) | ((LCD_HEIGHT - 1) << 14) | (LCD_LOWER_MARGIN << 6) | (LCD_VSYNC_LEN << 0);
   	//con3[18:8]���ø߶�240   HFPD��HBPD��������ʱʱ��;  ѡ��option.h�ļ���T35�������
   	rLCDCON3 = (LCD_RIGHT_MARGIN << 19) | ((LCD_WIDTH  - 1) <<  8) | (LCD_LEFT_MARGIN << 0);
   	//con4  HSPW[8:0]������Ӧ����Ŀ��; ѡ��option.h�ļ���T35�������
   	rLCDCON4 = (13 <<  8) | (LCD_HSYNC_LEN << 0);

#if !defined(LCD_CON5)
#    define LCD_CON5 ((1<<11) | (1 << 9) | (1 << 8) | (1 << 3) | (1 << 0))
#endif
    rLCDCON5   =  LCD_CON5;

    rLCDSADDR1 = ((LCD_ADDR >> 22) << 21) | ((M5D(LCD_ADDR >> 1)) <<  0);
    rLCDSADDR2 = M5D((LCD_ADDR + LCD_WIDTH * LCD_HEIGHT * 2) >> 1);
    rLCDSADDR3 = LCD_WIDTH;        

    rLCDINTMSK |= 3;   //�ж�����
  	rTCONSEL   &= (~7);
 

   	rTPAL     = 0x0;
   	rTCONSEL &= ~((1<<4) | 1);

    
}

/**************************************************************
LCD��Ƶ�Ϳ����ź��������ֹͣ��1������Ƶ���
**************************************************************/
void Lcd_EnvidOnOff(int onoff)
{
    if(onoff==1)
	rLCDCON1|=1; // ENVID=ON
    else
	rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
}
/**************************************************************
TFT LCD ��Դ��������ʹ��
**************************************************************/
void Lcd_PowerEnable(int invpwren,int pwren)
{
    //GPG4 is setted as LCD_PWREN
    rGPGUP = rGPGUP|(1<<4); // Pull-up disable
    rGPGCON = rGPGCON|(3<<8); //GPG4=LCD_PWREN
    
    //Enable LCD POWER ENABLE Function
    rLCDCON5 = rLCDCON5&(~(1<<3))|(pwren<<3);   // PWREN
    rLCDCON5 = rLCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}

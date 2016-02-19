#define	GLOBAL_CLK		1

#include "def.h"
#include <stdio.h>
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"  //��������
#include "2440slib.h"
#include "mmu.h"
#include "profile.h"


//���ܺ������� 
extern void RTC_Display_TICK_ALM(void);
extern void RTC_time(void);
extern void ADC_read_W1(void);
extern void LCD_Calculator(void);
extern void DMA_UART(void);
extern void watchdog(void);
extern void Uart(void); 
extern void music_player(void);
extern void IIC(void);



void display(){
    Uart_Printf("\n\n\n\n\n");
	Uart_Printf("<><><><><><><><><><><><><><><><><><><><><><><><><><\n");
	Uart_Printf("<            Hello2440 (1--9ѡ��)                 >\n");
	Uart_Printf("< 1: ʵʱʱ��RTC  ���ж�ģʽ��ʾ                  >\n");
	Uart_Printf("< 2: ʵʱʱ��RTC  ����ģ�ж�+�����ж�             >\n");
	Uart_Printf("< 3: A/Dת����������� LEDģ���ź�Ƶ��            >\n");
	Uart_Printf("< 4: LCD������    LCD��ʾ,�������ж�              >\n");
	Uart_Printf("< 5: DMAֱ�Ӵ�ȡ  ʵ��Uart�����ڣ�ͨ��            >\n");
	Uart_Printf("< 6: ���Ź��ж�  (4sһ��  ��λ�������Դ��)       >\n");
	Uart_Printf("< 7: ���ڴ����ļ� �ַ�����������,�ն���ʾ         >\n");
	Uart_Printf("< 8: ���ֲ�����--IIS��Ƶ���ݴ��� UDA1341�ź�ת��  >\n");
	Uart_Printf("< 9: IIC���߿��ƣ�EEPROMд�����ݲ������data����  >\n");
	Uart_Printf("<                                                 >\n");
	Uart_Printf("< By:����ӿ                     Hello2440 VER 1.0 >\n");
	Uart_Printf("<                 http://blog.csdn.net/muyang_ren >\n");
	Uart_Printf("<><><><><><><><><><><><><><><><><><><><><><><><><><\n");
	Uart_Printf("\n");
    Delay( 500 ) ;
}

			

void Main(void)
{   
    U8 num_i;          
    U32 mpll_val = 0,consoleNum;
    Port_Init();
    mpll_val = (92<<12)|(1<<4)|(1);
	
	//init FCLK=400M,
	ChangeMPllValue((mpll_val>>12)&0xff, (mpll_val>>4)&0x3f, mpll_val&3);
	ChangeClockDivider(14, 12);   //the result of rCLKDIVN [0:1:0:1] 3-0 bit
	cal_cpu_bus_clk();            //HCLK=100M   PCLK=50M
   
/*********************************************
��������ʵ��ʱ�����ιٷ����ڳ�ʼ������
**********************************************/
    consoleNum = 0;	  // Uart 0 select for debug.
	Uart_Init( 0,115200 );
	Uart_Select( consoleNum );
	
	Port_Init();
	MMU_Init(); //��ַӳ���ʼ��
	
	Beep(2000, 100); 
	  
//>>>>>>>>>>>>>>�����ǹ��ܴ�������<<<<<<<<<<<<<<<<<<<
    while(1){
        display();                  //����
      
        num_i = Uart_GetIntNum_GJ() ;	
       
        switch(num_i){ 
		 case 1:RTC_time();break;                  //�������ж���ʾʵʱʱ��(���Գɹ���)
		 case 2:RTC_Display_TICK_ALM();break;      //�����ж���ʾʵʱʱ�ӣ������ж�(���Գɹ���)
		 case 3:ADC_read_W1();break;               //ģ���źž���A/Dת�������������LED��ģ���ź�Ƶ�ʵ���ʱ��(���Գɹ���)
		 case 4:LCD_Calculator();break;            //������--LCD��ʾ,�������ж�(���Գɹ���)
		 case 5:DMA_UART();break;                  //DMA��ʽʵ��Uart�����ڣ�ͨ��(���Գɹ���)
		 case 6:watchdog();break;                  //���Ź��жϺ͸�λ����(���Գɹ���)
		 case 7:Uart();break;                      //ʵ�ִ��ڴ����ļ� �ַ����������壬����ʾ�ڴ����ն�(���Գɹ���) 
		 case 8:music_player();break;              //���ֲ�����--IIS��Ƶ���ݴ��� UDA1341оƬDAת���ɶ��߿ڵ����źţ����Գɹ�����
		 case 9:IIC();break;                       //IIC���߿��ƣ�EEPROMд�����ݲ������data����(���Գɹ���)
         default:Delay(300);Uart_Printf("\n������1--9\n"); Delay(1000);
        }
    }
}



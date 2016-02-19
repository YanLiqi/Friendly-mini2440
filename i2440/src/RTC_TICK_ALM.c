//=====================================================================
//  ʵ�ֹ��ܣ�
//          RTC�������жϡ������жϡ�
//          �����жϡ����������ʱ��XXXX��XX��XX��XXʱXX��XX�� �� LED����
//          �����жϡ���beep��  �� LED��  5��         
//                                                      by������ӿ
//=====================================================================

#include "2440addr.h"     //���ź궨��
#include "def.h"          // U8 U32�궨��
#include "2440lib.h"      //ʹ��Uart_Printf,Dalay������Uart_Printf������2440lib.c�ļ�
#include "option.h"
#include "mmu.h"

#define LED_OFF  (0x0f<<5)
#define LED_ON   (~0x0f<<5)

struct Time{          //RTCʱ��ṹ
    U32 year;
    U8 month;
    U8 day;
    U8 week;
    U8 hour;
    U8 mi;
    U8 sec;
}ttime_rtc;

void RTC_set()
{
    rRTCCON |=0x01;

    rBCDSEC    =0x0;   //14��6��12��1��1��0�� ����4
    rBCDMIN    =0x01;
    rBCDHOUR   =0x01;
    rBCDDAY    =0x4;    //����
    rBCDDATE   =0x12;
    rBCDMON    =0x6;
    rBCDYEAR   =0x14;

    rRTCCON &=~0x01;
}

void Led_Init(){
    rGPBCON=0x015400;  //GPB5 GPB6 GPB7 GPB8 ��ʼ��Ϊ���
    rGPBDAT=LED_OFF;   //Ϩ��״̬
}

//��RTC��ȡֵ
void read_for_rtc()
{
    rRTCCON |=0x01;  //RTCCONֻ����BCD�Ĵ�����ALM���ݼĴ����Ͳ���Ҫ��д������

    ttime_rtc.year  =0x2000+rBCDYEAR;
    ttime_rtc.month =rBCDMON;
    ttime_rtc.day   =rBCDDATE;
    ttime_rtc.week  =rBCDDAY;
    ttime_rtc.hour  =rBCDHOUR;
    ttime_rtc.mi    =rBCDMIN;
    ttime_rtc.sec   =rBCDSEC;

    rRTCCON &=~0x01;
}

void ALM_set(){      // ���Ӹ�ֵ
   

    rALMYEAR =0x14;
    rALMMON  =0x06;
    rALMDATE =0x12;
    rALMHOUR =0x01;
    rALMMIN  =0x01;
    rALMSEC  =0x03;
    
}

void RTC_display(){
    Uart_Printf("\n");
    Uart_Printf("\n<><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
	Uart_Printf("**************HELLO RTC�����жϡ������ж�*****************\n\n");
	Uart_Printf("#rTICNT:   (1<<7) | 127   �������ʹ��\n",rTICNT);
	Uart_Printf("#rRTCALM:  0b1000001      ��������ʹ�� �뾫������\n",rRTCALM);
    Uart_Printf("#rINTMSK:  ~(0x1<<8)      ����INT_TICK�ж�Դ����\n");
	Uart_Printf("#rINTMSK:  ~(0x1<<30)     ����INT_RTC �ж�Դ����\n");
	Uart_Printf("#rSRCPND    rINTPND        �жϿ����������Ӧλ�ٹر�\n");
	Uart_Printf("#\n#ͨ��RTC�����жϣ��������������ϵͳʱ��\n");	  
    Uart_Printf("#XXXX��XX��XX��XXʱXX��XX�� | LED����\n");
    Uart_Printf("#���ҿ��������ӣ����ӵ�ʱ��beep��|LED��\n");	
    Uart_Printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n\n\n\n");		
}

void __irq tick_interrupt(){
    rSRCPND|=0x1<<8;   //����жϹ���״̬
    rINTPND|=0x1<<8;
    
	//�жϹ��ܴ����
	
	
	rGPBDAT =~(rGPBDAT>>5)<<5 ;	//LED�� ����״̬�л�(�������㲻��Ӱ�쵽������)
	read_for_rtc();
    Uart_Printf("\n  RTC time: %x��%2x��%02x��--%02x:%02x:%02x   ����%d",ttime_rtc.year,ttime_rtc.month,ttime_rtc.day,ttime_rtc.hour,ttime_rtc.mi,ttime_rtc.sec,ttime_rtc.week);
	 
    //rTICNT  &= ~(1<<7); //��ֻʹ��һ���жϵ�ʱ��ر�ģ��ʹ��
    rSRCPND &=(0x1<<8);
    rINTPND &=(0x1<<8);
    
     }

void __irq alm_interrupt(){   //�ж���ں���
    
    rSRCPND|=0x1<<30;
    rINTPND|=0x1<<30;
    
	//�жϹ��ܴ����
    
    Uart_Printf("\n\n ************************************* ");
    Uart_Printf("\n           ����ʱ�̣�5�� ");
    Uart_Printf("\n ************************************* \n");
    rGPBDAT=LED_ON;        //����LED
    Beep(2000, 4000);      //  ���ڲ����жϻ���һ���ʱ�䣬����ʵ��������4+1������ж�
    
    
                           //  ע��㣨һ��
    rSRCPND &=(0x1<<30);   //  �������жϵ�ʱ�򣬱����ڿ����жϹ��ܺ�ر��ж����
    rINTPND &=(0x1<<30);   //  ���ҹر��ж�����Ŀ��Ʊ����ڴ��ж���ں����ɴ��жϿ���
                           //  (rSRCPND rINTPND����,����û�õ���rSUBSRCPND)��Ӧ�����ж�
                           //  ��ں����������������ں������Ҳ�����˹ر��ж����
                           //  ��rSRCPND rINTPND,rSUBSRCPND���㣩
                           //  ���û�йر��жϣ�������ʵ�������жϹ��ܣ������жϲ��ܽ���
     }


void RTC_TICK(){
    
    rTICNT |= (1<<7) | 127; //�����ж�ʹ�� ��ʱ�Ľ�������1s
    
                          //  ע��㣨����
	rINTMSK &= ~(0x1<<8); //  �ж����ο����ɷ���(���ж�ʹ�ܲ��ܷ����ж���ں����ڣ���
	                      //  Ϊ�ж�ʹ�ܲ������жϵĲ��������ǽ��ж�ǰ��һ������)
	                      
    pISR_TICK=(unsigned)tick_interrupt; //�����жϴ����жϺ�������ڵ�ַ                                                   	
	}
	
void RTC_ALM(){

	rRTCALM = 0x41;        //ȫ������ʹ�ܣ�������ʹ��(0b1000001)
	
	rINTMSK &= ~(0x1<<30);
	pISR_RTC =(unsigned)alm_interrupt;
	 		
	}
	
void RTC_Display_TICK_ALM()   //    ��main����
{
    static U32 save_E, save_PE, save_INT, save_TIC, save_RAL, save_RC; 
//**********************�����ֳ�************************** 
    save_E   = rGPECON;  
    save_PE  = rGPEUP;
    save_INT = rINTMSK;
    save_TIC = rTICNT;
    save_RAL = rRTCALM;
    save_RC  = rRTCCON;
//********************************************************
    
    //MMU_EnableICache();��һ��������Ҫ�� ��Ҫ�ر�mmu�������жϲ�������ʹ��
	MMU_Init();
	Led_Init();        //led1 2 3 4�������ʼ��Ϩ��״̬
    RTC_display();     //��ӡ��Ӧ��Ϣ
    
    RTC_set();         //rtc��ֵ
    ALM_set();         // ALM���Ӹ�ֵ    
 
    RTC_TICK();        //�����ж�
    RTC_ALM();         //�����ж�      
    
    //while(1)
    Delay(100);Uart_Printf("\n\nq:�˳�\n");
    while(Uart_GetKey() != 'q'){           
    }
    
    
//**********************��ԭ�ֳ�**************************       
    rGPEUP  = save_PE;  
    rGPECON = save_E;
    rINTMSK = save_INT;
    rTICNT  = save_TIC;
    rRTCALM = save_RAL;
    rRTCCON = save_RC;
//********************************************************  
}


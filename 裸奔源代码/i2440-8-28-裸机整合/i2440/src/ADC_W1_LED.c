//====================================================================
//  ʵ�ֹ��ܣ�
//          ͨ���������ϵĻ������������Ƶ�ģ���źž���A/Dת����
//          ת��Ϊ�����ź���uart_printf()��ӡ�ڴ����ϡ����һ���
//          ������ͬʱ����ָʾ��led����˸�ٶ�(ʱ��)��
//                                                   by������ӿ
//====================================================================

#include "2440addr.h"     //���ź궨��
#include "def.h"          // U8 U32�궨��
#include "2440lib.h"      //ʹ��Uart_Printf,Dalay������Uart_Printf������2440lib.c�ļ�

/* 
��PRSCENΪ1ʱ������PRSCVL��ֵ�������ADת����ʱ�䡣ADCʹ�õ�ʱ����ϵͳ��PCLK
������main��������mpllΪ400MHZ,����PCLK = 50MHz������PRSCVL��ֵΪ49����ô��

A/D ת����Ƶ�� = 50MHz / ��49+1�� = 1 MHz

A/D ת����ʱ�� = 1 /��1MHz / 5cycles��=5 uS
*/
/********ADC********/
#define PRSC_EN     0x1           //����Ԥ��Ƶ
#define PRSCVL      0x19          //Ԥ��Ƶֵ
#define SEL_MUX     ~0x7          //ģ��ͨ��ѡ��AIN0ͨ��
#define STDBM       0x0           //��������ģʽ
#define READ_START  0x0           //����ʱ������A/Dת��
#define ADC_STAR    0x1           //ADC����

/********LED*********/
#define LED1_out  (1<<(5*2))
#define LED2_out  (1<<(6*2))
#define LED3_out  (1<<(7*2))
#define LED4_out  (1<<(8*2))
#define LED4_ON   (0x0E<<8)       //LED1+LED2+LED3+LED4 ON
#define LED_OFF   (0x0F<<5)

void ADC_display(){
    Uart_Printf("\n");
    Uart_Printf("\n");
    Uart_Printf("\n");
	Uart_Printf("---------HELLO ADC\n");
	Uart_Printf("PRSC_EN:  %5x  ����Ԥ��Ƶ\n",PRSC_EN);
	Uart_Printf("PRSCVL:   %5x  Ԥ��Ƶֵ\n",PRSCVL);
	Uart_Printf("SEL_MUX:   ~0x7  ģ��ͨ��ѡ��AIN0ͨ��\n",SEL_MUX);
    Uart_Printf("STDBM:    %5x  ��������ģʽ\n",STDBM);
	Uart_Printf("READ_START:%4x  ����ʱ������A/Dת��\n",READ_START);
	Uart_Printf("ADC_STAR:  %4x  ADC����\n",ADC_STAR);
	Uart_Printf("���������Ƶ�ģ���źž���A/Dת����ת��Ϊ\n");	  
    Uart_Printf("�����ź��ô�����ʾ,���һ���������ͬʱ��\n");
    Uart_Printf("��ָʾ��led����˸�ٶ�(ʱ��)\n");	
    Uart_Printf("#########################################\n\n");		
}

//��ʼ��AD���ƼĴ���
void ADC_INIT(){
   rADCCON&=(~((0x1<<14)|(0xff<<6)|(0x7<<3)|(0x1<<2)|(0x1<<1)|(0x1<<0)));
   rADCCON=(PRSC_EN<<14)|(PRSCVL<<6)|(SEL_MUX<<3)|(STDBM<<2)|(READ_START<<1);
   rADCCON|=(ADC_STAR<<0);          //����ADC������ADC�󣬴�λ���Զ����㣩

}

//��ʼ��LED,�ĸ�LED���
void LED_INIT(){                    
    rGPBCON=LED1_out|LED2_out|LED3_out|LED4_out;
}

//LED����
void W1_LED(int get_data0){         
    rGPBDAT= LED4_ON;     //�����ĸ�LED
    Delay(get_data0);     //ADC��ʮλʱ�Ӻ���
    rGPBDAT= LED_OFF;     //Ϩ��LED
}

//���ڴ�ӡ������ģ���ź���ֵ
void W1_ADC(int *read_xpdata){
	ADC_INIT(); //��ʼ��AD���ƼĴ���
    /*****���ڴ�ӡADC���ݺ���****/
    while(rADCCON & 0x1);                //��Ϊ�ɹ�����A/Dת���󣬸�λ��
                                          //�Զ����㣬�����������ADC�Ƿ���������
    while(!(rADCCON & (1<<15)));            //ʹ�ò�ѯ��ʽ�ȴ�ADCת������
    *read_xpdata=(int)(rADCDAT0&0x3ff);   //��ȡADCDAT0��ʮλ
    Uart_Printf("\n HELLO ADC# W1ֵΪ�� %04d ",*read_xpdata);//�������ADCDAT0��ʮλֵ
    Delay(500);
	}

//��main����
void ADC_read_W1(){                     
    static U32 save_A, save_B, save_BD;
    int read_xpdata0;
     
//**********************�����ֳ�************************** 
    save_A   = rADCCON;  
    save_B   = rGPBCON;
    save_BD  = rGPBDAT;
//********************************************************
    
    ADC_display();  
    LED_INIT();  //��ʼ��LED
   
    //while(1)
    Delay(100);Uart_Printf("\n\nq:�˳�\n");
    while(Uart_GetKey() != 'q')
    {
     /*���ڴ�ӡ������ģ���ź���ֵ*/
     W1_ADC( &read_xpdata0 );//����read_xpdata0��ַ���ı�read_xpdata0��ֵ����W1_LED����ʹ��
    
     /*****LED��������*****/
     W1_LED(read_xpdata0);                 //LED����
     
    }
    
//**********************��ԭ�ֳ�**************************         
    rADCCON = save_A;
    rGPBCON = save_B;
    rGPBDAT = save_BD;
//********************************************************    

}
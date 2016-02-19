#include "2440addr.h" 
#include "def.h"
#include "2440lib.h"
#include <stdio.h>   //�ļ��⺯��

void Uart_init(){

//����ʹ��uart0

    rGPHCON&=~((3<<4)|(3<<6));  //GPH2--TXD0;GPH3--RXD0
    rGPHCON|=((2<<4)|(2<<6));    //����GPH2��GPH3ΪTXD0��RXD0����
//�ֲ���GPH2��GPH3��������:
//GPHCON    	Bit        	Description
//GPH3  	   [7:6]    	00 = Input   	01 = Output   
//	                 	    10 = RXD0   	11 = reserved
//GPH2         [5:4] 	    00 = Input    	01 = Output
//                   	    10 = TXD0  	    11 = Reserved 
    
    rGPHUP=0x00;                //��������ʹ��
    rUFCON0 = 0x0;              //��ֹFIFO
    rUMCON0 = 0x0;              //����nRTS
    rULCON0|=0x03;              //����UART0���ݷ���8������λ
    rUCON0  = 0x5;;           //����ģʽ�ͽ���ģʽ��ʹ�ò�ѯģʽ
  
    
    //���ò����ʣ����в�������Ϊһ���������ݵ��ó�ʼ������
    rUBRDIV0=(int)((50000000/(115200*16))-1);  
    Delay( 10 ) ;
       
}


    

void Uart(){
    int i=0;
    char ch;
    FILE *FP;
   
    static U32 save_H, save_HP, save_U, save_UF0, save_UM0, save_UL0;    
//**********************�����ֳ�************************** 
    save_H   = rGPHCON;
    save_HP  = rGPHUP;
    save_U   = rUCON0;  
    save_UF0 = rUFCON0;
    save_UM0 = rUMCON0;
    save_UL0 = rULCON0;  
//********************************************************                                            //�ļ�ָ��
  
    FP=fopen("uart.txt","w");  //��д��ʽ��uart.txt�ļ�
    Uart_init();                                        //uart��ʼ������

    //while(1)
    Delay(100);Uart_Printf("\n\nq:�˳�\n");
    while(Uart_GetKey() != 'q'){
        if(rUTRSTAT0&1) {            //����������ݻ��������յ���Ч����
    
         ch = rURXH0;            //ch�洢�����ֽ�����
         rUTXH0=ch;              //PC�������յ��ַ�ͨ�����ڵ��Թ�����ʾ����Ļ�� 
		 
		 fputc(ch,FP);                //��ch�ַ����ݴ���ļ���
         i++; if(i>200) fclose(FP);   //�����200���ֽڹر��ļ�       
         }
    }
//**********************��ԭ�ֳ�************************** 
    rGPHCON  = save_H;
    rGPHUP   = save_HP;
    rUCON0   = save_U;  
    rUFCON0  = save_UF0;
    rUMCON0  = save_UM0;
    rULCON0  = save_UL0;  
//******************************************************** 
}
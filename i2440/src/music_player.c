/***********************************
 ʵ�ֹ���     music������  
***********************************/

#include "2440lib.h"
#include "2440slib.h" 
#include "LCD_init.h"
#include "2440addr.h" 

#define   L3C       (1<<4)          //gpb4:L3CLOCK  
#define   L3D       (1<<3)          //gpb3:L3DATA  
#define   L3M       (1<<2)          //gpb2:L3MODE 
#define   rIISFIFO  (*(volatile unsigned long*)0x55000010)	

extern unsigned char music_interface[];
extern unsigned char hellomusic[];  //hellomusic
extern unsigned char music1_bmp[];
extern unsigned char music2_bmp[];
extern unsigned char music3_bmp[];

extern unsigned char button1[];     //��ͣ
extern unsigned char button1_1[];
extern unsigned char button2[];     //��һ��
extern unsigned char button2_2[];
extern unsigned char button3[];     //��һ��
extern unsigned char button3_3[]; 
extern unsigned char button4[];     //����
extern unsigned char button4_4[];

extern unsigned char music1[3704572];
extern unsigned char music2[3704552];
extern unsigned char music3[5644880];
//extern unsigned char WindowsXP_Wav[243552];

//xdata, ydata���ڴ洢��Ļ����(������Ļ���ص���)
volatile static int xdata,     ydata;    //volatile�����ã���Ϊָ��ؼ��֣�ȷ������ָ�������������Ż���ʡ�ԣ���Ҫ��ÿ��ֱ�Ӷ�ֵ.
volatile static int botten1=0, flag=0, flag1, song_num=1, song_num1=1;
volatile static int length,  count, song_num2=1,  i;   
volatile static unsigned char *buffer;

/**************************************************************
 ��ֵ������
**************************************************************/
static void Button_Handle(void){
   
	  
	if( xdata >= 160 && xdata <= 360 && ydata >= 280 && ydata < 420 )       //��һ�ײ�����
	{   Beep(2000, 100); botten1 = 1, song_num--; flag=1;;//xdata = ydata = 0;
	    if(song_num==0) { song_num=3;} song_num1=song_num; 
	    Pait_Bmp( 20, 190, 60, 60, button2_2); Delay(200); Pait_Bmp( 90, 190, 60, 60, button1); Pait_Bmp( 20, 190, 60, 60, button2); }
	 
	if( xdata >= 660 && xdata <= 860 && ydata >= 280 && ydata < 420 )       //��һ�ײ�����
	{   Beep(2000, 100); botten1 = 2, song_num++; flag=1;//xdata = ydata = 0;
	    if(song_num==4) { song_num=1;} song_num1=song_num;  
	    Pait_Bmp(160, 190, 60, 60, button3_3); Delay(200); Pait_Bmp( 90, 190, 60, 60, button1); Pait_Bmp(160, 190, 60, 60, button3); }
	 
	if( xdata >= 410 && xdata <= 610 && ydata >= 280 && ydata < 420 )       //����
	{   Beep(2000, 100); flag++; //xdata = ydata = 0; 
	    //��һ�κܴ��ɣ������(��һ��/��һ��) flag��ֵΪ1ʱ���������Ҳ��ִ��  
	    //����Ӧ����  ydata >= 280 && ydata < 420 ����Ϊ1�� xdata >= 410 && xdata <= 610 && ydata >= 280 && ydata < 420��ֵҲΪ1
	    //�պ�ʵ�����ҵĹ���  
	    if(flag==1){botten1 = 0; flag1=flag;
		Pait_Bmp( 90, 190, 60, 60, button4_4); Delay(200); Pait_Bmp( 90, 190, 60, 60, button1);}  //Ϊ1ʱ���ţ���ʾ��ͣ��ť
		
		else {flag=0; botten1 = 0;
		Pait_Bmp( 90, 190, 60, 60, button1_1); Delay(200); Pait_Bmp( 90, 190, 60, 60, button4);}  //Ϊ0ʱ��ͣ����ʾ���Ű�ť
    }
}

/**************************************************************
 TFT LCD *�������жϺ���*
**************************************************************/

  
static void __irq Adc_Tc_Handler(void)  
{  
  
    rADCTSC|=(1<<3)|(1<<2); //XP����������Ч, �Զ���������X�����Y����.  
    rADCCON|=(1<<0);//ADCת����ʼ  
  
    while(rADCCON&(1<<0));//���ADCת���Ƿ�ʼ��ADCCON[0]�Զ���0  
    while(!(rADCCON&(0x1<<15))); //���ADCCON[15]�Ƿ�Ϊ1,ADCת���Ƿ����,(����)  
    while(!(rINTPND&((U32)0x1<<31)));//���ADC�ж��Ƿ�������  
      
    xdata=rADCDAT0&0x3ff;//��x���� >>xdata���������ص㣬����ģ���ź� 0-1000 
    ydata=rADCDAT1&0x3ff;//��y����  
    //Beep(2000, 100); 
    Button_Handle();           //button������ 
    Uart_Printf("\n       Xdata=%04d,   Ydata=%04d\n", xdata, ydata);
    Uart_Printf("\n        flag=%d,  1:����    0����ͣ", flag);         
    Uart_Printf("\n     botten1=%d,  1:��һ��  2����һ��",botten1);
    Uart_Printf("\n         **���ڲ��ŵ�%d�׸���**  \n\n", song_num);
    
      
    
  
    rSUBSRCPND|=(0x1<<9);   //����ж�
    rSRCPND|=((U32)0x1<<31);  
    rINTPND|=((U32)0x1<<31);  
      
    rADCTSC =0xd3;     //ADC�ȴ��ж�ģʽ    
    rADCTSC|=(0x1<<8);  //ADCTSC[8]=1,����̧���ж��ź�  
      
    while(!(rSUBSRCPND&(0x1<<9)));  //��ⴥ��̧���ж��Ƿ�������   
  
    rADCTSC &=~(0x1<<8);//ADCTSC[8]=0��갴���ж��ź�  
    
    //  ����������δ����������δ����ǿ��������Ƕ������ȥ�ģ����Եõ�
    //  ���ۣ����������ж�ʵ�ִ����жϣ�������������Ǽӿ��жϺ����Ӧ
    //  (�ҵ�ʵ�����ǣ�û�����ʱ�򣬴��������ֵ��ʾʱ��䳤����������
    //  ����Ҳ�䳤��������һ�ĵĸо�)
   
    rSUBSRCPND|=(0x1<<9);     
    rSRCPND|=((U32)0x1<<31);  
    rINTPND|=((U32)0x1<<31);    
}   
  
static void Touch_Init(void)  
{  
    rADCCON=((1<<14)|(9<<6));    //A/D��Ƶʱ����Ч����ֵΪ9  
    rADCTSC=0xd3;  //��갴���ж��ź�,YM��Ч��YP��Ч��XM��Ч��XP��Ч��XP�������裬��ͨADCת�����ȴ��ж�ģʽ  
    rADCDLY=50000; //����ת��ģʽת����ʱ��ԼΪ(1/3.6864M)*50000=13.56ms  
      
    rINTSUBMSK &=~(0x1<<9);//TC�ж�ʹ��  
    rINTMSK &=~((U32)0x1<<31);//ADC���ж�ʹ��  
      
    pISR_ADC=(U32)Adc_Tc_Handler;//ָ���ж�������  
      
} 

/**************************************************************
 IIS��ʼ��
**************************************************************/
void iis_init(void){

    //����IIS�ӿ�
    rGPEUP = rGPEUP & ~(0x1f) | 0x1f;         //������Ч��GPE[4:0] 1 1111
    rGPECON = rGPECON & ~(0x3ff) | 0x2aa;
    
    //����s3c2440��IIS�Ĵ���
    //Ԥ��Ƶ��Ϊ2������CDCLK=PCLK/(2+1)=16.66666kHz
    rIISPSR = 2<<5|2;
    //��ЧDMA��������У�Ԥ��Ƶ����Ч������IIS
    rIISCON  = (0<<5)|(0<<4)|(0<<3)|(1<<2)|(1<<1);   
    //PCLKΪʱ��Դ�����ģʽ��IISģʽ��ÿ������16λ��CODECLK=384fs��SCLK=32fs
    rIISMOD  = (0<<9)|(0<<8)|(2<<6)|(0<<5)|(0<<4)|(1<<3)|(1<<2)|(1<<0);     
    rIISFCON = (0<<15)|(1<<13);           //���FIFO����ģʽ�����FIFOʹ��
    
    
}

/**************************************************************
     UA1341����  
**************************************************************/  
//ͨ��io��ģ��L3����д����  
//mode��1Ϊ��ַģʽ��0Ϊ����ģʽ  
//���ڵ�ַģʽ������ģʽ�Լ�����ʱ��ע��ο������ֲ�  
static void write_UA1341(U8 data, U8 address)  
{  
    int i,j;  
    if(address == 1)  
    {  
        rGPBDAT = rGPBDAT&(~(L3D | L3M |L3C)) |L3C;  //��ַģʽ�������ֲ�L3MΪLOW,L3CΪhigh  
      
    }  else {  
      
    rGPBDAT = rGPBDAT & (~(L3D |L3M |L3C)) |(L3M|L3C);   //����ģʽ L3MΪ��  
      
    }  
    Delay(1);  
    //��������  
   for(i=0;i<8;i++)    
       {
              if(data & 0x1)                      // H
              {
                     rGPBDAT &= ~L3C;            //L3C=L
                     rGPBDAT |= L3D;                //L3D=H            
                     for(j=0;j<5;j++)
;                    //�ȴ�һ��ʱ��
                     rGPBDAT |= L3C;                //L3C=H
                     rGPBDAT |= L3D;                //L3D=H
                     for(j=0;j<5;j++)
;                    //�ȴ�һ��ʱ��
               }
              else                       // L
              {
                     rGPBDAT &= ~L3C;            //L3C=L
                     rGPBDAT &= ~L3D;            //L3D=L
                     for(j=0;j<5;j++)
;                    //�ȴ�һ��ʱ��
                     rGPBDAT |= L3C;                //L3C=H
                     rGPBDAT &= ~L3D;            //L3D=L
                     for(j=0;j<5;j++)
;                    //�ȴ�һ��ʱ��          
              }
              data >>= 1;
        }
    rGPBDAT = rGPBDAT & ~(L3D | L3M | L3C) | (L3C | L3M);          //L3M=H,L3C=H  
  
}  

/**************************************************************
 UDA1341��ʼ��
**************************************************************/
void UDA1341_init(void){
    //UDA1341��ʼ��
    //����L3�ӿ����ߣ�GPB2:L3MODE, GPB3:L3DATA, GPB4:L3CLOCK
    rGPBCON = 0x015550;               //���
    rGPBUP  = 0x7ff;               //������Ч
    rGPBDAT = 0x1e4;
    
    rGPBDAT = rGPBDAT & (~(L3M |L3C |L3D)) |(L3M|L3C);   //��L3CLOCK��L3MODE�øߣ�׼����ʼ����  
    ////����UDA1341TS�����ֲ�14ҳ�еĲ���˳�������ڵ�ַģʽ�£�  
    //ѡ�������ַ000101xx +10(STATUS)=0X16  
    write_UA1341(0x16,1) ;    
    write_UA1341(0x60,0); // 0��1 ��10��000��0  ��λ  
      
    write_UA1341(0x16,1) ;  
    write_UA1341(0x10,0);  //0,0,01, 000,0 : ״̬0, 384fs,IIS,no DC-filtering  
      
    write_UA1341(0x16,1) ;  
    write_UA1341(0xc1,0);           //1,0,0,0, 0,0,01:״̬1,                                    
                                //Gain of DAC 6 dB,Gain of ADC 0dB,ADC non-inverting,  
                                //DAC non-inverting,Single speed playback,ADC-Off DAC-On   
}

/**************************************************************
 music     *��main����*
**************************************************************/
 
void music_player(void)
{	
    static U32 save_A, save_C, save_CP, save_D, save_DP,save_G, save_GP,save_INTS,save_INT,
    save_E, save_EP, save_B, save_LCD1, save_LCD2, save_LCD3, save_LCD4, save_LCD5 ,
    save_SR, save_S, save_SM, save_SF, save_LCDINT,  save_BD;
     
//**********************�����ֳ�************************** 
    save_SR  = rIISPSR;
    save_S   = rIISCON;
    save_SM  = rIISMOD;
    save_SF  = rIISFCON;
    save_A   = rADCCON;
    save_B   = rGPBCON;
    save_BD  = rGPBDAT;
    save_EP  = rGPEUP;
    save_E   = rGPECON;
    save_G   = rGPGCON;
    save_GP  = rGPGUP;  
    save_C   = rGPCCON;  
    save_CP  = rGPCUP;
    save_D   = rGPDCON;
    save_DP  = rGPDUP;
    save_LCD1 = rLCDCON1;
    save_LCD2 = rLCDCON2;
    save_LCD3 = rLCDCON3;
    save_LCD4 = rLCDCON4;
    save_LCD5 = rLCDCON5;
    save_LCDINT = rLCDINTMSK;
    save_INTS   = rINTSUBMSK;
    save_INT    = rINTMSK;
//********************************************************    
    
    Lcd_Port_Init(); //�˿ڳ�ʼ��
    LCD_Init();  //TFT LCD����ģ���ʼ��
	Lcd_PowerEnable(0, 1);  // TFT LCD ��Դ��������ʹ��
    Lcd_EnvidOnOff(1);		//LCD��Ƶ�Ϳ����ź��������ֹͣ��1������Ƶ���
   
    
    
    /*��(255:0:0);��(0:255:0);��(0:0:255);��(0:0:0);��(255,255,255)*/
	
    /*����Ļ����ʾ����ɫ*/
		
    Lcd_ClearScr((0x1f<<11) | (0x00<<5) | (0x00));			//red
    Delay(500);
		
    Lcd_ClearScr((0x00<<11) | (0x3f<<5) | (0x00));			//green
    Delay(500);
    
    Lcd_ClearScr((0x00<<11) | (0x00<<5) | (0x1f));			//blue
    Delay(500);
    
    Lcd_ClearScr( (0x1f<<11) | (0x3f<<5) | (0x1f)  )  ;		//clear screen white
    Delay(500);
    
    Pait_Bmp(  0,  0, 240, 320, music_interface);
    Pait_Bmp( 70, 70, 100, 100, hellomusic);
    Pait_Bmp( 20, 190, 60, 60, button2);   //��һ��
    Pait_Bmp(160, 190, 60, 60, button3);   //��һ��
    Pait_Bmp( 90, 190, 60, 60, button4);   //����
    
    Touch_Init();           //���������ж�
    
    iis_init();             // IIS��ʼ�� 
    
	UDA1341_init();         //UDA1341��ʼ��
    
    
    buffer=music1;              //��ʼ��bufferָ��music1���������ַ
    
    //while(1)
    Delay(100);Uart_Printf("\n\nq:�˳�\n");
    while(Uart_GetKey() != 'q')
    {
        if(flag==1){ rIISCON |= 0x1;  //����������(flag==1)  ����IIS  song_num1��ֵΪ1  ���ŵ�һ��
    
            //����������ʱ����ͼƬ����ʾ(�������ʱ flag1=1)
            if(flag1==1 && song_num==1 ){flag1=0, Pait_Bmp( 70, 70, 100, 100, music1_bmp);}
            if(flag1==1 && song_num==2 ){flag1=0, Pait_Bmp( 70, 70, 100, 100, music2_bmp);}
            if(flag1==1 && song_num==3 ){flag1=0, Pait_Bmp( 70, 70, 100, 100, music3_bmp);}
         
            // ���ֱ�־song_num1��Ϊ��ʱ(����������song_num1��ֵ����ͨ����ֵ(��һ��/��һ��)��song_num1��ֵ) 
            // buffer���ֲ��ŵ�ַ����ֵ  length���ֳ������¸�ֵ ����ʾ����ͼƬ  ��һFIFO�ֽ�λ��count��ʼ��  song_num1����
            if(song_num1==1) { buffer=music1; length=3704572; count=0; Pait_Bmp( 70, 70, 100, 100, music1_bmp); song_num1=0;}
            if(song_num1==2) { buffer=music2; length=3704552; count=0; Pait_Bmp( 70, 70, 100, 100, music2_bmp); song_num1=0;}
	        if(song_num1==3) { buffer=music3; length=5644880; count=0; Pait_Bmp( 70, 70, 100, 100, music3_bmp); song_num1=0;}
        
            if((rIISCON & (1<<7))==0)               //������FIFO�Ƿ�Ϊ��
            {    //FIFO�е�����Ϊ16λ�����Ϊ32
            //�����FIFOΪ��ʱ��һ������FIFOд��32��16λ����
                for(i=0;i<32;i++){
                //rIISFIFO=(buffer[i+count]);
		        rIISFIFO=(buffer[2*i+count])+(buffer[2*i+1+count]<<8);    //  һ��ѭ����FIFO�洢16λ������FIFO  
                }                                                       
                                                                                
                count+=64;                                                //  64Ϊ32��ѭ����ÿ��ѭ��ָ��������ͬ�ֽڵ��ֽ��ܺ�                                   
 
        //���ֲ�����ɺ�  ���ֱ�־song_num1ָ����һ�׸�
            if(count>length){   
                song_num2=song_num2+1; 
                if(song_num2==4){song_num2=1;} 
                song_num1=song_num2;
                }
            }
            
        }
        
        //��ͣʱ�ر�IIS  ��ʾ��ӭͼƬ
        if(flag==0) { rIISCON |= 0x0; Pait_Bmp( 70, 70, 100, 100, hellomusic); }  //�ر�IIS;
            
    }
    
      
        Lcd_ClearScr( (0x1f<<11) | (0x3f<<5) | (0x1f)  );		//��ɫ
//**********************��ԭ�ֳ�************************** 
    rIISPSR  = save_SR;
    rIISCON  = save_S;
    rIISMOD  = save_SM;
    rIISFCON = save_SF;
    rADCCON  = save_A;
    rGPBCON  = save_B;
    rGPBDAT  = save_BD;
    rGPEUP   = save_EP;
    rGPECON  = save_E;
    rGPGCON  = save_G;
    rGPGUP   = save_GP ;  
    rGPCCON  = save_C ;  
    rGPCUP   = save_CP;
    rGPDCON  = save_D;
    rGPDUP   = save_DP;
    rLCDCON1 = save_LCD1;
    rLCDCON2 = save_LCD2;
    rLCDCON3 = save_LCD3;
    rLCDCON4 = save_LCD4;
    rLCDCON5 = save_LCD5;
    rLCDINTMSK = save_LCDINT;
    rINTSUBMSK = save_INTS;
    rINTMSK    = save_INT;
//********************************************************
}
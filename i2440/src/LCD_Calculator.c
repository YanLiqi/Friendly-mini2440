/***********************************
 ʵ�ֹ���      LCD������   
***********************************/

#include "2440lib.h"
#include "2440slib.h" 
#include "LCD_init.h"
#include "2440addr.h" 
#include "mmu.h" 

extern void show_picturu(int num);
void numTOpic(int m ,int n);

extern unsigned char my1_240x320[];
extern unsigned char Calculator[];
extern unsigned char clean[];
extern unsigned char error[];
extern unsigned char numerror[];
extern unsigned char num_[]; //����
extern unsigned char num0[];
extern unsigned char num1[];
extern unsigned char num2[];
extern unsigned char num3[];
extern unsigned char num4[];
extern unsigned char num5[];
extern unsigned char num6[];
extern unsigned char num7[];
extern unsigned char num8[];
extern unsigned char num9[];

volatile static int xdata, ydata;    //volatile�����ã� ��Ϊָ��ؼ��֣�ȷ������ָ�������������Ż���ʡ�ԣ���Ҫ��ÿ��ֱ�Ӷ�ֵ.
volatile static int flag1=0 , flag2=0 ;
volatile static long long int sum1=0 , sum2=0;
volatile static int  botten1; 

/**************************************************************
 TFT LCD *�������жϺ���*
**************************************************************/

  
void __irq Adc_Tc_Handler(void)  
{  
  
    rADCTSC|=(1<<3)|(1<<2); //XP����������Ч, �Զ���������X�����Y����.  
    rADCCON|=(1<<0);//ADCת����ʼ  
  
    while(rADCCON&(1<<0));//���ADCת���Ƿ�ʼ��ADCCON[0]�Զ���0  
    while(!(rADCCON&(0x1<<15))); //���ADCCON[15]�Ƿ�Ϊ1,ADCת���Ƿ����,(����)  
    while(!(rINTPND&((U32)0x1<<31)));//���ADC�ж��Ƿ�������  
      
    xdata=rADCDAT0&0x3ff;//��x���� >>xdata���������ص㣬����ģ���ź� 0-1000 
    ydata=rADCDAT1&0x3ff;//��y����  
      
    Uart_Printf("\n       Xdata=%04d, Ydata=%04d\n", xdata, ydata);  
  
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
  
void Touch_Init(void)  
{  
    rADCCON=((1<<14)|(9<<6));    //A/D��Ƶʱ����Ч����ֵΪ9  
    rADCTSC=0xd3;  //��갴���ж��ź�,YM��Ч��YP��Ч��XM��Ч��XP��Ч��XP�������裬��ͨADCת�����ȴ��ж�ģʽ  
    rADCDLY=50000; //����ת��ģʽת����ʱ��ԼΪ(1/3.6864M)*50000=13.56ms  
      
    rINTSUBMSK &=~(0x1<<9);//TC�ж�ʹ��  
    rINTMSK &=~((U32)0x1<<31);//ADC���ж�ʹ��  
      
    pISR_ADC=(U32)Adc_Tc_Handler;//ָ���ж�������  
      
}  

/**************************************************************
             ------------��ȡ��ֵ--------------
**************************************************************/ 
void get_key(void){
    //���ֿ�
    if( xdata >= 70 && xdata <= 285 && ydata >= 100 && ydata < 262 )       //0
	{   Beep(2000, 100);botten1 = 0;flag1=1;xdata = ydata = 0;  }
	else if( xdata >= 70 && xdata <= 285 && ydata >= 263 && ydata < 425 )    //1 
	{   Beep(2000, 100);botten1 = 1;flag1=1;xdata = ydata = 0;  }
	else if( xdata >= 285 && xdata <= 500 && ydata >= 263 && ydata < 425 )    //2 
	{   Beep(2000, 100);botten1 = 2;flag1=1;xdata = ydata = 0;  }
	else if( xdata >= 500 && xdata <= 715 && ydata >= 263 && ydata < 425 )     //3
	{   Beep(2000, 100);botten1 = 3;flag1=1;xdata = ydata = 0;  }
	else if( xdata >= 70 && xdata <= 285 && ydata >= 425 && ydata < 587 )     //4  
	{   Beep(2000, 100);botten1 = 4;flag1=1;xdata = ydata = 0;  }
	else if( xdata >= 285 && xdata <= 500 && ydata >= 425 && ydata < 587 )     //5
	{   Beep(2000, 100);botten1 = 5;flag1=1;xdata = ydata = 0;  }
	else if( xdata >= 500 && xdata <= 715 && ydata >= 425 && ydata < 587 )     //6
	{   Beep(2000, 100);botten1 = 6;flag1=1;xdata = ydata = 0;  }
	else if( xdata >= 70 && xdata <= 285 && ydata >= 587 && ydata < 750 )     //7
	{   Beep(2000, 100);botten1 = 7;flag1=1;xdata = ydata = 0;  }
	else if( xdata >= 285 && xdata <= 500 && ydata >= 587 && ydata < 750 )     //8
	{   Beep(2000, 100);botten1 = 8;flag1=1;xdata = ydata = 0;  }
	else if( xdata >= 500 && xdata <= 715 && ydata >= 587 && ydata < 750 )     //9
	{   Beep(2000, 100);botten1 = 9;flag1=1;xdata = ydata = 0;  }
	else
	//���������
	if( xdata >= 715 && xdata <= 930 && ydata >= 100 && ydata < 262 )          //��
	{   Beep(2000, 100);botten1 = '/';flag1=1;flag2=1;xdata = ydata = 0;  }
	else if( xdata >= 715 && xdata <= 930 && ydata >= 263 && ydata < 425 )     //����
	{   Beep(2000, 100);botten1 = '*';flag1=1;flag2=2;xdata = ydata = 0;  }
	else if( xdata >= 715 && xdata <= 930 && ydata >= 425 && ydata < 587 )     //����
	{   Beep(2000, 100);botten1 = '-';flag1=1;flag2=3;xdata = ydata = 0;  }
	else if( xdata >= 715 && xdata <= 930 && ydata >= 587 && ydata < 750 )     //�ӷ�
	{   Beep(2000, 100);botten1 = '+';flag1=1;flag2=4;xdata = ydata = 0;  }
	else
	if( xdata >= 500 && xdata <= 715 && ydata >= 100 && ydata < 262 )          //����
	{   Beep(2000, 100);botten1 = '=';flag1=1;xdata = ydata = 0;  }
	else
	if( xdata >= 285 && xdata <= 500 && ydata >= 100 && ydata < 262 )         // ���
	{   Beep(2000, 100);botten1 = 'c';flag1=1;xdata = ydata = 0;  }	

}

/**************************************************************
             ------------�����ֵ--------------
**************************************************************/
void resout(){
	
	if(flag2==4)               {sum1=sum1+sum2; show_picturu(sum1);  sum1=0;sum2=0;flag2=0;  }   //���ݱ�־flag2 ������Ӧ��  sum1=sum1 �� sum2  ������,������sum1 sum2����
		 else if(flag2==3)     {sum1=sum1-sum2; show_picturu(sum1);  sum1=0;sum2=0;flag2=0;  } 
		      else if(flag2==2){sum1=sum1*sum2; show_picturu(sum1);  sum1=0;sum2=0;flag2=0;  } 
		           else if(flag2==1){
		                    if(sum2==0) {sum1=0;sum2=0;flag2=0;  Pait_Bmp(1, 33, 236, 30, error);Delay(500);show_picturu(0);}  //�Գ���Ϊ0����
		                    else {sum1=sum1/sum2;show_picturu(sum1);  sum1=0;sum2=0;flag2=0; }}       //ע��:��������������ȥС������
		

}
void cleaning(){
  sum1=0,sum2=0;flag2=0;show_picturu(0);
}
void show_num(void){ 
    while(flag1){  
	flag1=0;   //��־����
	     switch(botten1){
		 case 0:
		 case 1:
		 case 2:
		 case 3:
		 case 4:
		 case 5:
		 case 6:
		 case 7:
		 case 8:
		 case 9:{sum2=sum2*10+botten1;
		            if(sum2>999999999){
				        sum2=0;Pait_Bmp(2, 33, 236, 30, numerror);
					    Delay(500);show_picturu(sum2);
				    } 
				    else show_picturu(sum2);
				} break;       //����ֵ���� sum2
		 
		 case '+':
		 case '-':
		 case '*':
		 case '/': sum1=sum2,sum2=0; break;                                //sum2ֵת����sum1��sum2����
		 
		 case '=': resout(); break;
		 
		 //֮ǰ���������η�������۲��������sum1�ͺ�sum2�������������(����=����Զ����0)������֮�����԰��жϷŽ�һ������ resout(); �￴��������ɹ��ˣ�����
		/* if(flag2==4)          {show_picturu(sum1+sum2);  sum2=0;flag2=0;}   //���ݱ�־flag2 ������Ӧ��  sum1=sum1 �� sum2  ������
		 else if(flag2==3)     {show_picturu(sum1-sum2);  sum2=0;flag2=0;  } 
		      else if(flag2==2){show_picturu(sum1*sum2);  sum2=0;flag2=0;  } 
		           else if(flag2==1){
		                    if(sum2==0) {sum1=0;sum2=0;flag2=0;  Pait_Bmp(1, 33, 236, 30, error);Delay(500);show_picturu(0);}  //�Գ���Ϊ0����
		                    else {show_picturu(sum1/sum2);  sum2=0;flag2=0; }}       //ע��:��������������ȥС������
		*/
		
		case 'c': cleaning(); break;
		 }
    
    }
}
/**************************************************************
             ------------����ͼƬ����--------------
**************************************************************/
void show_picturu(int sum){
 int k=0,j=0;
    Pait_Bmp(1, 33, 238, 30, clean);
	if(sum>0){                          //��ֵ����0ʱ
	    for(;sum>0;){
		    k=sum%10;                   //k����ȡ����
            sum=sum/10;                 //num��ȥ���һλ
			++j;                        //j������־k��������ԭ��ֵ�е�ʵ��λ��
            numTOpic(k,j);			
		    }
		}
	else if(sum==0){                    //��ֵ����0ʱ
	         numTOpic(0,1);
	    }                     
	else if(sum<0){                    //��ֵС��0ʱ
	         sum=-sum;
	         for(;sum>0;){
		         k=sum%10;                   //k����ȡ����
                 sum=sum/10;                 //num��ȥ���һλ
			     ++j;                        //j������־k��������ԭ��ֵ�е�ʵ��λ��
                 numTOpic(k,j);			
		        }
			Pait_Bmp(238-13*(j+1), 33, 13, 30, num_);
	    }

}
void numTOpic(int m ,int n){
switch(m){
    case 0:Pait_Bmp(238-13*n, 33, 13, 30, num0);break;
	case 1:Pait_Bmp(238-13*n, 33, 13, 30, num1);break;
	case 2:Pait_Bmp(238-13*n, 33, 13, 30, num2);break;
	case 3:Pait_Bmp(238-13*n, 33, 13, 30, num3);break;
	case 4:Pait_Bmp(238-13*n, 33, 13, 30, num4);break;
	case 5:Pait_Bmp(238-13*n, 33, 13, 30, num5);break;
	case 6:Pait_Bmp(238-13*n, 33, 13, 30, num6);break;
	case 7:Pait_Bmp(238-13*n, 33, 13, 30, num7);break;
	case 8:Pait_Bmp(238-13*n, 33, 13, 30, num8);break;
	case 9:Pait_Bmp(238-13*n, 33, 13, 30, num9);break;
}
}
/**************************************************************
 TFT LCD     *��main����*
**************************************************************/
 
void LCD_Calculator(void)
{	
    static U32 save_A, save_C, save_CP, save_D, save_DP,save_G, save_GP,save_INTS,save_INT,
    save_LCD1, save_LCD2, save_LCD3, save_LCD4, save_LCD5 ,save_LCDINT;
     
//**********************�����ֳ�************************** 
    save_A   = rADCCON;
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
	
    /*��ʾһ��ͼƬ����Ļ��*/
    Pait_Bmp(0, 0, 240, 320, my1_240x320);
    Delay(1000);
    Pait_Bmp(0, 0, 240, 320, Calculator);
    Delay(1000);
    Pait_Bmp(225, 33, 13, 30, num0);
    
    //MMU_EnableICache();��һ��������Ҫ�� ��Ҫ�ر�mmu�������жϲ�������ʹ��
	
    Touch_Init();           //���������ж�
    
    //while(1)
    Delay(100);Uart_Printf("\n\nq:�˳�\n");
    while(Uart_GetKey() != 'q'){
    get_key();      //��ȡ��ֵ
    show_num();     //�����ֵ
    }
    
    Lcd_ClearScr( (0x1f<<11) | (0x3f<<5) | (0x1f)  );		//��ɫ
//**********************��ԭ�ֳ�************************** 
    rADCCON  = save_A;
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

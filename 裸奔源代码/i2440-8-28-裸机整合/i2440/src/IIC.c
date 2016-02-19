#include "def.h"  
#include "2440addr.h"  
#include "2440lib.h" 


static unsigned int i,j;  
static U8 data[256];  
static volatile int flag;   //���ڱ�ʶ�Ƿ��յ�Ӧ���źţ��ı�ʶ���ն˴�������б���0 

/**************************************************************
IIC ��ʼ������
**************************************************************/
void iic_init(void){
    rGPECON |= 0xa0000000;                 //GPE15:IICSDA , GPE14:IICSCL (rGPEUP[14:15]û������ѡ��)
   
    //[7]=1����Ӧ����  [6]=0 IICCLK = fPCLK /16  PCLK 50MHz, IICCLK = 3.17MHz, Tx Clock = 0.198MHz
    // ����IIC����Tx/Rx�ж�ʹ��   IICCON[3:0]=16       
    rIICCON = (1<<7) | (0<<6) | (1<<5) | (0xf); 
    
    rIICADD  = 0x10;     //�ӵ�ַ    ��ʾ2440��Ϊ���豸��ʱ��ĵ�ַ��������2440����Ϊһ�����豸���ڵģ�����û�����á�   
    rIICSTAT = 0x10;         //IIC�����������ʹ�ܶ�д  
    rIICLC = (1<<2)|(1);     //ʹ���˲��� ��ʱ���ʱ��  

}

/**************************************************************
 IIC�жϺ���
**************************************************************/
void __irq Iic_isr(void)  
{  
    flag = 0; 
    rSRCPND = 0x1<<27;          //����ж�  
    rINTPND = 0x1<<27;    
}

void iic_isr(void){
    rINTMSK &= ~(0x1<<27);
    pISR_IIC = (unsigned)Iic_isr;//�ж����  
}

/**************************************************************
    дEEPROM�е�����
**************************************************************/  
void Wr24C080(U32 slvAddr, U32 addr, U8 data)  
{  
    flag=1;            //���¶���Ӧ���־ ��־λ��1 
    rIICDS = slvAddr;  //**�豸��ַ��EEPROM�ĵ�ַΪ1010�� 
    rIICSTAT = 0xf0;   // ���豸����ģʽ��д��������ʼ�ź�  
    rIICCON &= ~0x10;  //���¶������жϱ�־  
    while(flag == 1)   //�����ʹӵ�ַ���֮����յ�ACK�źţ����жϴ������н��ñ�־��Ϊ0  
    Delay(1);  
       
      
    flag =1 ;          
    rIICDS = addr;     //ƫ�Ƶ�ַ
    rIICCON &= ~0x10;             
    while(flag == 1)   //�����ʹӵ�ַ���֮����յ�ACK�źţ����жϴ������н��ñ�־��Ϊ0  
    Delay(1);  
       
    flag =1 ;          
    rIICDS = data;     //Ҫд��ľ�������
    rIICCON &= ~0x10;           
    while(flag == 1)   //�����ʹӵ�ַ���֮����յ�ACK�źţ����жϴ������н��ñ�־��Ϊ0  
    Delay(1);  
       
    rIICSTAT = 0xd0;                    //Stop MasTx condition   
    rIICCON  = 0xaf;                    //Resumes IIC operation.   
    Delay(1);  
      
} 
 
/**************************************************************
    ��EEPROM�е�����
**************************************************************/  
void Rd24C080(U32 slvAddr, U32 addr, U8 *data)  
{  
    unsigned char temp;  
    flag=1;                            
    rIICDS = slvAddr;   //�豸��ַ��EEPROM�ĵ�ַΪ1010��
    rIICSTAT = 0xf0;    // ���豸����ģʽ��������slvAddr��addr,������  
    rIICCON &= ~0x10;               
    while(flag == 1)    // �����ʹӵ�ַ���֮����յ�ACK�źţ����жϴ������н��ñ�־��Ϊ0  
    Delay(1);  
       
    flag =1 ;                           
    rIICDS = addr;       //ƫ�Ƶ�ַ
    rIICCON &= ~0x10;                
    while(flag == 1)    // �����ʹӵ�ַ���֮����յ�ACK�źţ����жϴ������н��ñ�־��Ϊ0  
    Delay(1);  
          
    flag=1;                         
    rIICDS = slvAddr;   //���豸��ַ�������ض�Ҫ���������͵ġ��������������豸�ᱻ����Ϊ����״̬����
                        //��˷����豸��ַ������ͬʱ�������豸״̬�ġ�
    rIICSTAT = 0xb0;    // ���豸����ģʽ�����������ݣ�����  
    rIICCON &= ~0x10;             
    while(flag == 1)    // �����ʹӵ�ַ���֮����յ�ACK�źţ����жϴ������н��ñ�־��Ϊ0  
     Delay(1);  
      
    // ע�⣺��ȡ��������ֽڱ�����У���Ϊ�ڷ��ʹ��ж�����Ĵ��豸��ַ��  
    // AT24C02A���ٷ���һ�����豸��ַ��Ϣ����豸�ڴ��ַ��Ϣ��ΪӦ������һ��Ҫ�Ѹ��ֽڶ�ȡ����������Ϊ������������Ҫ��ȡ����Ϣ��  
    flag =1 ;                          // ��־λ��1  
    temp = rIICDS;      // ������һ�Լ�  
    rIICCON &= ~0x10;   // ���жϱ�־  
    while(flag)  
     Delay(1);  
       
    if(i==256-1)  rIICCON &= ~0x80;//��������һ������ ������Ӧ
    else          rIICCON = 0x2f;     //Resumes IIC operation with NOACK.   
    *data = rIICDS;   
    Delay(1);  
    rIICSTAT = 0x90;   //Stop MasTx condition   
    rIICCON  = 0xaf;   //Resumes IIC operation.  
          
    Delay(1);  
      
}  
  
/**************************************************************
 IIC ��main����
**************************************************************/
void IIC(void)  
{  
    static U32 save_E, save_PE, save_C, save_CA, save_CS, save_CLC, save_INT, save_CDS, save_CTA; 
//**********************�����ֳ�************************** 
    save_E   = rGPECON;  
    save_PE  = rGPEUP;
    save_C   = rIICCON;
    save_CA  = rIICADD;
    save_CS  = rIICSTAT;
    save_CLC = rIICLC;
    save_INT = rINTMSK;
    save_CDS = rIICDS;
    save_CTA = rIICSTAT;   
//******************************************************** 
          
    Uart_Printf("\n  ***  IIC TEST  ***  \n"); // Delay(1000); //1
      
    iic_init(); //IIC��ʼ��
    iic_isr();  //IIC�ж�
   
    Uart_Printf("д�����ݵ�EEPROM\n"); //��EEPROMд����i  
    for(i=0;i<256;i++){ Wr24C080(0xa0,(U8)i,i); Delay(1);}   //ע�������ʱ�����٣����������Щ�����޷�д�������           
    
    for(i=0;i<256;i++) data[0] = 0; //��ʼ��data[i]  
   
    Uart_Printf("��EEPROM�е�����\n"); //EEPROM��ȡ���ݵ�data[i]    
    for(i=0;i<256;i++){ Rd24C080(0xa0,(U8)i,&(data[i])); Delay(1);}
    
    for(i=0;i<16;i++)  //��ӡdata[i]����
    {    Uart_Printf("\n");
         for(j=0;j<16;j++)   Uart_Printf(" %2x",data[i*16+j]);  } 
      
    
    Delay(100);Uart_Printf("\n\nq:�˳�\n");
    while(Uart_GetKey() != 'q'){
    } 

//**********************�����ֳ�************************** 
    rGPECON  = save_E;  
    rGPEUP   = save_PE;
    rIICCON  = save_C;
    rIICADD  = save_CA;
    rIICSTAT = save_CS;
    rIICLC   = save_CLC;
    rINTMSK  = save_INT;
    rIICDS   = save_CDS;
    rIICSTAT = save_CTA;    
//********************************************************
}  



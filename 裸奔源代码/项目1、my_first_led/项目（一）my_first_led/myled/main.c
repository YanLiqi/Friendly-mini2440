//
//**********************************************************
//*****************�˺���Ϊʵ��*****************************
//**led1��>>>led1��>>>                                    **
//**>>>led1��led2��>>>led1��led2��>>>                     **
//**>>>led1��led2��led3��>>>ed1��led2��led3��             **
//**>>>led1��led2��led3��led4��>>>Led1��led2��led3��led4��**
//
//
//
#define rGPBCON  (*(volatile unsigned long*)0x56000010)  //rGPBCON��ָ��0x56000010�����ַ�ռ��һ����
#define rGPBDAT  (*(volatile unsigned long*)0x56000014)
#define LED_OFF  (0x0f<<5)
#define U8       unsigned char
#define U32      unsigned int

//ʵ��ʱ��������
void daly()
{
    U32 t;
    U32 i;
    for(t=888;t>0;t--){
    for(i=0;i<10000;i++){}
    }
}

int Main()
{
U8  i;
U8  datastar;
U32 GPBDAT;
datastar=0x0f;     //0x0f=00001111
rGPBCON=0x015400;  //GPB5 GPB6 GPB7 GPB8 ��ʼ��Ϊ���
while(1){
    GPBDAT=(datastar<<5);
	for(i=0;i<4;i++){
	    //����LED
	    rGPBDAT=(GPBDAT<<1);//�ĸ�ѭ��������һλʵ�����ֵ���ģʽ
	    GPBDAT=rGPBDAT;
	    daly();
	    //Ϩ��LED
	    rGPBDAT=LED_OFF;
	    daly();
	}

}

return 0;
}

//������k1������Ƶ�ʽ���,
//������k2������Ƶ������,
//������k3�˳�����������
//������k4��������������

#define GPBCON    (*(volatile unsigned long*)0x56000010)  //rGPBCON��ָ��0x56000010�����ַ�ռ��һ����
#define GPBDAT    (*(volatile unsigned long*)0x56000014)  //led GPIO

#define GPGCON    (*(volatile unsigned long*)0x56000060)  //KEY GPIO
#define GPGDAT    (*(volatile unsigned long*)0x56000064)

#define TCFG0     (*(volatile unsigned long*)0x51000000)  //PWM��ʱ��
#define TCFG1     (*(volatile unsigned long*)0x51000004)
#define TCON      (*(volatile unsigned long*)0x51000008)
#define TCNTB0    (*(volatile unsigned long*)0x5100000C)
#define TCMPB0    (*(volatile unsigned long*)0x51000010)

#define PWM_out   (0x01)                      //����PWMΪ���
#define PWM_TOUT0 (10)                        //����PWMΪTOUT0���ܣ�LED�������
#define LED1_out  (1<<(5*2))
#define LED2_out  (1<<(6*2))
#define LED3_out  (1<<(7*2))
#define LED4_out  (1<<(8*2))

#define BEEP_OFF  (0)        //��������˵1��������������0�����������
#define BEEP_ON   (1)

#define LED_OFF   (0x0F<<5)  //LED                 OFF
#define LED1_ON   (0x0E<<5)  //LED1                ON
#define LED2_ON   (0x0E<<6)  //LED1+LED2           ON
#define LED3_ON   (0x0E<<7)  //LED1+LED2+LED3      ON
#define LED4_ON   (0x0E<<8)  //LED1+LED2+LED3+LED4 ON

#define U8        unsigned char
#define U32       unsigned int


void daly()               //ʱ���ӳٺ���
{
    U32 t;
    U32 i;
    for(t=888;t>0;t--)
    for(i=0;i<10000;i++);

}

void led_key_init()            //��ʼ��LED��KEY
{
    GPBCON=LED1_out|LED2_out|LED3_out|LED4_out|PWM_TOUT0;                                  //led PWM ����Ϊ���
    GPGCON= ~( (3<<(2*0))|(3<<(2*3))|(3<<(2*5))|(3<<(2*6)) );                              //GPG0 3 5  6����  ��������1 2 3 4����
    GPBDAT=LED_OFF|BEEP_ON;
}

void PWM_init()                //��ʼ��PWM��ʱ������
{
    TCFG0 &= 0xFFFF00;         //��ʼ��Ԥ��Ƶֵ
    TCFG0 |= 0x31;             //prescal=49
    TCFG1 &= ~0xF;             //mux=1/2    ��PCLKΪ50MHz,��ʱ������ʱ��Ƶ�� = PCLK / {Ԥ��Ƶֵ+1} / {��Ƶֵ}������50MHz/(49+1)/2=500kHz
    TCNTB0 = 5000;             //��ʼ��˫������TCNTB0��TCMPB0
    TCMPB0 = 2500;
    TCON &= ~0x1F;             //TCON����
    TCON |= 0xf;               //������Ч���Զ�װ�أ���ƽ��ת���ֶ����£���ʱ������
    TCON &= ~0x2 ;             //�ֶ�����λ���㣬PWM��ʼ����
}

void ESC_PWM()
{
    GPBCON = ~0x3;             //�ر�GPB0��TOUT0���ܣ�
    GPBCON = PWM_out;          //����GPB0Ϊ01�������
    GPBDAT = BEEP_OFF;         //����GPB0���ݼĴ���Ϊ0���Է�������˵1��������������0�����������
}

void OPEN_PWM()
{
    GPBCON=LED1_out|LED2_out|LED3_out|LED4_out|PWM_TOUT0;  //����GPB�����������ݼĴ���
    GPBDAT=LED_OFF|BEEP_ON;
}


void led2_1_on()                              //LED1 2��1����
{
    GPBDAT=LED2_ON; daly();
    GPBDAT=LED_OFF; daly();
    GPBDAT=LED1_ON; daly();
    GPBDAT=LED_OFF;
}

void led3_4_on()                              //LED3 ; 3 4����
{
    GPBDAT=LED3_ON; daly();
    GPBDAT=LED_OFF; daly();
    GPBDAT=LED4_ON; daly();
    GPBDAT=LED_OFF;
}

int Main()
{
    U32  freq=2500;
    led_key_init();         //��ʼ��LED��KEY
    PWM_init();             //��ʼ��PWM��ʱ��

    while(1)
    {
        if( !(GPGDAT &( 1<<0 )) ){      // K1  PWMֵ����
            freq =freq+300;
            if(freq>5000) freq=2500;

            TCMPB0 = freq;             //�����ֲ���˵��PWM���ܿ���ͨ��ʹ��TCMPBnʵ�֡�PWMƵ����TCNTBn������
                                       //����ͨ���ı�TCMPB0���ı�PWM��ֵ��
                                       //��СTCMPBn�������PWMֵ������TCMPBn���Խ���PWMֵ��
            led2_1_on();
            } else

        if(!(GPGDAT &( 1<<3 )) ){      // K2   PWMֵ����
            freq =freq-300;
            if(freq<1000) freq=2500;
            TCMPB0 = freq;
            led3_4_on();
            } else

        if(!(GPGDAT &( 1<<5 )) ){      // K3   �رշ��������
            ESC_PWM();
            } else

        if(!(GPGDAT &( 1<<6 )) ){      // K4   �򿪷��������
            OPEN_PWM();
            }
    }
    return 0;
}

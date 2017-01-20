#define _BSP_

#include "bsp.h"

/*******************************************************************************
* Macro Definition
*******************************************************************************/
// IO ��������
#define IIC_SDA_IN()	{GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=0<<2*11;}	//PF11����ģʽ
#define IIC_SDA_OUT()	{GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=1<<2*11;}	//PF11���ģʽ

// IO ��������
#define IIC_SCL			PBout(0)	//SCL
#define IIC_SDA			PFout(11)	//SDA
#define IIC_READ_SDA	PFin(11)	//����SDA

// ��ʱ
#define IIC_Delay()		delay_us(2)

/*******************************************************************************
* Function Name  : touchI2CInit
*******************************************************************************/
void touchI2CInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOB,Fʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//PB0����Ϊ�������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//��ʼ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//PF11�����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���ģʽ
	GPIO_Init(GPIOF, &GPIO_InitStructure);			//��ʼ��
}

/*******************************************************************************
* Function Name  : touchI2CStart
* ����IIC��ʼ�ź�
*******************************************************************************/
void touchI2CStart(void)
{
	IIC_SDA_OUT();     //sda�����
	IIC_SDA=1;
	IIC_SCL=1;
	delay_us(30);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low
	IIC_Delay();
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ��������
}

/*******************************************************************************
* Function Name  : touchI2CStop
* ����IICֹͣ�ź�
*******************************************************************************/
void touchI2CStop(void)
{
	IIC_SDA_OUT();//sda�����
	IIC_SCL=1;
	delay_us(30);
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	IIC_Delay();
	IIC_SDA=1;//����I2C���߽����ź�
}

/*******************************************************************************
* Function Name  : touchI2CWaitAck
* �ȴ�Ӧ���źŵ���
* ����ֵ��1������Ӧ��ʧ��
*         0������Ӧ��ɹ�
*******************************************************************************/
u8 touchI2CWaitAck(void)
{
	u8 ucErrTime=0;
	IIC_SDA_IN();      //SDA����Ϊ����
	IIC_SDA=1;
	IIC_SCL=1;
	IIC_Delay();
	while(IIC_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			touchI2CStop();
			return 1;
		}
		IIC_Delay();
	}
	IIC_SCL=0;//ʱ�����0
	return 0;
}

/*******************************************************************************
* Function Name  : touchI2CAck
* ����ACKӦ��
*******************************************************************************/
void touchI2CAck(void)
{
	IIC_SCL=0;
	IIC_SDA_OUT();
	IIC_Delay();
	IIC_SDA=0;
	IIC_Delay();
	IIC_SCL=1;
	IIC_Delay();
	IIC_SCL=0;
}

/*******************************************************************************
* Function Name  : touchI2CNAck
* ������ACKӦ��
*******************************************************************************/
void touchI2CNAck(void)
{
	IIC_SCL=0;
	IIC_SDA_OUT();
	IIC_Delay();
	IIC_SDA=1;
	IIC_Delay();
	IIC_SCL=1;
	IIC_Delay();
	IIC_SCL=0;
}

/*******************************************************************************
* Function Name  : touchI2CSendByte
* IIC����һ���ֽ�
* ���شӻ�����Ӧ��
* 1����Ӧ��
* 0����Ӧ��
*******************************************************************************/
void touchI2CSendByte(u8 txd)
{
    u8 t;
	IIC_SDA_OUT();
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	IIC_Delay();
	for(t=0;t<8;t++)
    {
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1;
		IIC_SCL=1;
		IIC_Delay();
		IIC_SCL=0;
		IIC_Delay();
    }
}

/*******************************************************************************
* Function Name  : touchI2CReadByte
* ��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
*******************************************************************************/
u8 touchI2CReadByte(unsigned char ack)
{
	u8 i,receive=0;
 	IIC_SDA_IN();//SDA����Ϊ����
	delay_us(30);
	for(i=0;i<8;i++ )
	{
		IIC_SCL=0;
		IIC_Delay();
		IIC_SCL=1;
		receive<<=1;
		if(IIC_READ_SDA)receive++;
	}
	if (!ack)touchI2CNAck();//����nACK
	else touchI2CAck(); //����ACK
 	return receive;
}

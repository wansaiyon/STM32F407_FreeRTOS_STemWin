#define _BSP_

#include "bsp.h"

/*******************************************************************************
* Macro Definition
*******************************************************************************/
// IO 方向设置
#define IIC_SDA_IN()	{GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=0<<2*11;}	//PF11输入模式
#define IIC_SDA_OUT()	{GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=1<<2*11;}	//PF11输出模式

// IO 操作函数
#define IIC_SCL			PBout(0)	//SCL
#define IIC_SDA			PFout(11)	//SDA
#define IIC_READ_SDA	PFin(11)	//输入SDA

// 延时
#define IIC_Delay()		delay_us(2)

/*******************************************************************************
* Function Name  : touchI2CInit
*******************************************************************************/
void touchI2CInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOB,F时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//PB0设置为推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//PF11设置推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//输出模式
	GPIO_Init(GPIOF, &GPIO_InitStructure);			//初始化
}

/*******************************************************************************
* Function Name  : touchI2CStart
* 产生IIC起始信号
*******************************************************************************/
void touchI2CStart(void)
{
	IIC_SDA_OUT();     //sda线输出
	IIC_SDA=1;
	IIC_SCL=1;
	delay_us(30);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low
	IIC_Delay();
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据
}

/*******************************************************************************
* Function Name  : touchI2CStop
* 产生IIC停止信号
*******************************************************************************/
void touchI2CStop(void)
{
	IIC_SDA_OUT();//sda线输出
	IIC_SCL=1;
	delay_us(30);
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	IIC_Delay();
	IIC_SDA=1;//发送I2C总线结束信号
}

/*******************************************************************************
* Function Name  : touchI2CWaitAck
* 等待应答信号到来
* 返回值：1，接收应答失败
*         0，接收应答成功
*******************************************************************************/
u8 touchI2CWaitAck(void)
{
	u8 ucErrTime=0;
	IIC_SDA_IN();      //SDA设置为输入
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
	IIC_SCL=0;//时钟输出0
	return 0;
}

/*******************************************************************************
* Function Name  : touchI2CAck
* 产生ACK应答
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
* 不产生ACK应答
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
* IIC发送一个字节
* 返回从机有无应答
* 1，有应答
* 0，无应答
*******************************************************************************/
void touchI2CSendByte(u8 txd)
{
    u8 t;
	IIC_SDA_OUT();
    IIC_SCL=0;//拉低时钟开始数据传输
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
* 读1个字节，ack=1时，发送ACK，ack=0，发送nACK
*******************************************************************************/
u8 touchI2CReadByte(unsigned char ack)
{
	u8 i,receive=0;
 	IIC_SDA_IN();//SDA设置为输入
	delay_us(30);
	for(i=0;i<8;i++ )
	{
		IIC_SCL=0;
		IIC_Delay();
		IIC_SCL=1;
		receive<<=1;
		if(IIC_READ_SDA)receive++;
	}
	if (!ack)touchI2CNAck();//发送nACK
	else touchI2CAck(); //发送ACK
 	return receive;
}

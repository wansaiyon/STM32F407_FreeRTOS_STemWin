#define _DRIVER_

#include "driver.h"

/*******************************************************************************
* Macro Definition
*******************************************************************************/
// IO 操作函数
#define GT_RST			PCout(13)	//GT9147复位引脚
#define GT_INT			PBin(1)		//GT9147中断引脚

// GT9147 地址
#define GT_CMD_WR		0X28		//写命令
#define GT_CMD_RD		0X29		//读命令

// GT9147 部分寄存器定义
#define GT_CTRL_REG		0X8040		//GT9147控制寄存器
#define GT_CFGS_REG		0X8047		//GT9147配置起始地址寄存器
#define GT_CHECK_REG	0X80FF		//GT9147校验和寄存器
#define GT_PID_REG		0X8140		//GT9147产品ID寄存器
#define GT_GSTID_REG	0X814E		//GT9147当前检测到的触摸情况
#define GT_TP1_REG		0X8150		//第一个触摸点数据地址
#define GT_TP2_REG		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG		0X8170		//第五个触摸点数据地址

// 触摸模拟按键个数
#define BSP_TOUCH_KEY_NUM	(18)


/*******************************************************************************
* 常量定义
*******************************************************************************/
// GT9147配置参数表
// 第一个字节为版本号(0X60),必须保证新的版本号大于GT9147内部
// flash原有版本号,才会更新配置.
static const u8 GT9147_CFG_TBL[] =
{
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,
};

// 读取屏幕触摸点的寄存器地址列表
static const u16 GT9147_TPX_TBL[5]=
{
	GT_TP1_REG,
	GT_TP2_REG,
	GT_TP3_REG,
	GT_TP4_REG,
	GT_TP5_REG
};

// 屏幕虚拟按键坐标定义
static const struct
{
	u16 x, y;
	u16 w, h;
}touchKeyRectTbl[BSP_TOUCH_KEY_NUM] =
{
	{5, 190, 40, 40,},
	{5, 290, 40, 40,},
	{5, 340, 40, 40,},
	{5, 390, 40, 40,},
	{70, 435, 40, 40,},
	{130, 435, 40, 40,},
	{190, 435, 40, 40,},
	{250, 435, 40, 40,},
	{310, 435, 40, 40,},
	{755, 190, 40, 40,},
	{755, 290, 40, 40,},
	{755, 340, 40, 40,},
	{755, 390, 40, 40,},
	{450, 435, 40, 40,},
	{510, 435, 40, 40,},
	{570, 435, 40, 40,},
	{630, 435, 40, 40,},
	{690, 435, 40, 40,},
};

/*******************************************************************************
* Function Name  : touchKeyRectContant
* 判断点是否在指定按键的区域内
* 1-是，0-否
*******************************************************************************/
static u8 touchKeyRectContant(u16 key_id, u16 x, u16 y)
{
	if(    (x >= touchKeyRectTbl[key_id].x)
		&& (x <= touchKeyRectTbl[key_id].x + touchKeyRectTbl[key_id].w - 1)
		&& (y >= touchKeyRectTbl[key_id].y)
		&& (y <= touchKeyRectTbl[key_id].y + touchKeyRectTbl[key_id].h - 1))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
* Function Name  : touchGt9147WrReg
* 向GT9147写入一次数据
* reg:起始寄存器地址
* buf:数据缓缓存区
* len:写数据长度
* 返回值:0,成功;1,失败.
*******************************************************************************/
static u8 touchGt9147WrReg(u16 reg, u8 *buf, u8 len)
{
	u8 i;
	u8 ret=0;

	touchI2CStart();
 	touchI2CSendByte(GT_CMD_WR);   	//发送写命令
	touchI2CWaitAck();
	touchI2CSendByte(reg>>8);   	//发送高8位地址
	touchI2CWaitAck();
	touchI2CSendByte(reg&0XFF);   	//发送低8位地址
	touchI2CWaitAck();
	for(i=0;i<len;i++)
	{
    	touchI2CSendByte(buf[i]);  	//发数据
		ret = touchI2CWaitAck();
		if(ret)break;
	}
    touchI2CStop();					//产生一个停止条件

	return ret;
}

/*******************************************************************************
* Function Name  : touchGt9147RdReg
* 从GT9147读出一次数据
* reg:起始寄存器地址
* buf:数据缓缓存区
* len:读数据长度
*******************************************************************************/
static void touchGt9147RdReg(u16 reg,u8 *buf,u8 len)
{
	u8 i;

 	touchI2CStart();
 	touchI2CSendByte(GT_CMD_WR);	//发送写命令
	touchI2CWaitAck();
 	touchI2CSendByte(reg>>8);		//发送高8位地址
	touchI2CWaitAck();
 	touchI2CSendByte(reg&0XFF);		//发送低8位地址
	touchI2CWaitAck();
 	touchI2CStart();
	touchI2CSendByte(GT_CMD_RD);	//发送读命令
	touchI2CWaitAck();
	for(i=0;i<len;i++)
	{
    	buf[i] = touchI2CReadByte(i==(len-1)?0:1); //发数据
	}
    touchI2CStop();					//产生一个停止条件
}

/*******************************************************************************
* Function Name  : touchGt9147SendCfg
* 发送GT9147配置参数
*******************************************************************************/
static void touchGt9147SendCfg(void)
{
	u8 buf[2];
	u8 i=0;

	buf[0] = 0;
	buf[1] = 1;	//将配置写入到GT9147 FLASH,  即掉电保存

	for(i=0; i<sizeof(GT9147_CFG_TBL); i++)
	{
		buf[0] += GT9147_CFG_TBL[i];//计算校验和
	}
    buf[0] = (~buf[0])+1;

	touchGt9147WrReg(GT_CFGS_REG, (u8*)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL));//发送寄存器配置
	touchGt9147WrReg(GT_CHECK_REG, buf, 2);//写入校验和,和配置更新标记
}

/*******************************************************************************
* Function Name  : touchGt9147Init
* 初始化GT9147触摸屏
*******************************************************************************/
void touchGt9147Init(void)
{
	u8 temp[5];
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);	//使能GPIOB,C时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			//PB1设置为上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			//PC13设置为推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);				//初始化

	touchI2CInit();      	//初始化电容屏的I2C总线
	GT_RST = 0;				//复位
	delay_ms(10);
 	GT_RST = 1;				//释放复位
	delay_ms(10);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			//PB1设置为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//初始化

	delay_ms(100);

	temp[0] = 0x02;
	touchGt9147WrReg(GT_CTRL_REG,temp,1);				//软复位GT9147
	touchGt9147RdReg(GT_CFGS_REG,temp,1);				//读取GT_CFGS_REG寄存器
	if(temp[0] < GT9147_CFG_TBL[0])						//若当前版本低于程序中定义版本,则更新flash配置
	{
		touchGt9147SendCfg();							//更新并保存配置
	}
	delay_ms(10);
	temp[0]=0X00;

	touchGt9147WrReg(GT_CTRL_REG,temp,1);				//结束复位
}

/*******************************************************************************
* Function Name  : touchGt9147Scan
* 触摸屏扫描
*******************************************************************************/
u32 touchGt9147Scan(void)
{
	u16	i, j;
	u16	x, y;
	u8	val;
	u8	buf[4];
	u32 key = 0;

	touchGt9147RdReg(GT_GSTID_REG, &val, 1);	// 读取触摸点的状态
	if((val&0x80) && ((val&0x0F)<6))
	{
		// 如果有数据可读取，且点数不超过5个，则读取数据
		// 清标志
		buf[0] = 0;
		touchGt9147WrReg(GT_GSTID_REG, buf, 1);

		val &= 0x0F;	// 获取触点数
		for(i=0; i<val; i++)
		{
			// 读取触点坐标
			touchGt9147RdReg(GT9147_TPX_TBL[i], buf, 4);
#if BSP_LCD_SWAP_XY
			y = ((u16)buf[1]<<8) + buf[0];
			x = ((u16)buf[3]<<8) + buf[2];
#else
			x = ((u16)buf[1]<<8) + buf[0];
			y = ((u16)buf[3]<<8) + buf[2];
#endif

// 经实验验证，触摸屏坐标与显示屏坐标方向相反
// 所以屏幕不做X、Y镜像的时候，触摸屏要镜像
#if !BSP_LCD_MIRROR_X
			x = LCD_XSIZE - x - 1;
#endif
#if !BSP_LCD_MIRROR_Y
			y = LCD_YSIZE - y - 1;
#endif

			// 判断触点对应的按键
			for(j=0; j<BSP_TOUCH_KEY_NUM; j++)
			{
				if(touchKeyRectContant(j, x, y))
				{
					key |= (1<<j);
					break;
				}
			}
		}
	}

	return key;
}

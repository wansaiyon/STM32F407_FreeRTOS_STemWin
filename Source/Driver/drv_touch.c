#define _DRIVER_

#include "driver.h"

/*******************************************************************************
* Macro Definition
*******************************************************************************/
// IO ��������
#define GT_RST			PCout(13)	//GT9147��λ����
#define GT_INT			PBin(1)		//GT9147�ж�����

// GT9147 ��ַ
#define GT_CMD_WR		0X28		//д����
#define GT_CMD_RD		0X29		//������

// GT9147 ���ּĴ�������
#define GT_CTRL_REG		0X8040		//GT9147���ƼĴ���
#define GT_CFGS_REG		0X8047		//GT9147������ʼ��ַ�Ĵ���
#define GT_CHECK_REG	0X80FF		//GT9147У��ͼĴ���
#define GT_PID_REG		0X8140		//GT9147��ƷID�Ĵ���
#define GT_GSTID_REG	0X814E		//GT9147��ǰ��⵽�Ĵ������
#define GT_TP1_REG		0X8150		//��һ�����������ݵ�ַ
#define GT_TP2_REG		0X8158		//�ڶ������������ݵ�ַ
#define GT_TP3_REG		0X8160		//���������������ݵ�ַ
#define GT_TP4_REG		0X8168		//���ĸ����������ݵ�ַ
#define GT_TP5_REG		0X8170		//��������������ݵ�ַ

// ����ģ�ⰴ������
#define BSP_TOUCH_KEY_NUM	(18)


/*******************************************************************************
* ��������
*******************************************************************************/
// GT9147���ò�����
// ��һ���ֽ�Ϊ�汾��(0X60),���뱣֤�µİ汾�Ŵ���GT9147�ڲ�
// flashԭ�а汾��,�Ż��������.
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

// ��ȡ��Ļ������ļĴ�����ַ�б�
static const u16 GT9147_TPX_TBL[5]=
{
	GT_TP1_REG,
	GT_TP2_REG,
	GT_TP3_REG,
	GT_TP4_REG,
	GT_TP5_REG
};

// ��Ļ���ⰴ�����궨��
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
* �жϵ��Ƿ���ָ��������������
* 1-�ǣ�0-��
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
* ��GT9147д��һ������
* reg:��ʼ�Ĵ�����ַ
* buf:���ݻ�������
* len:д���ݳ���
* ����ֵ:0,�ɹ�;1,ʧ��.
*******************************************************************************/
static u8 touchGt9147WrReg(u16 reg, u8 *buf, u8 len)
{
	u8 i;
	u8 ret=0;

	touchI2CStart();
 	touchI2CSendByte(GT_CMD_WR);   	//����д����
	touchI2CWaitAck();
	touchI2CSendByte(reg>>8);   	//���͸�8λ��ַ
	touchI2CWaitAck();
	touchI2CSendByte(reg&0XFF);   	//���͵�8λ��ַ
	touchI2CWaitAck();
	for(i=0;i<len;i++)
	{
    	touchI2CSendByte(buf[i]);  	//������
		ret = touchI2CWaitAck();
		if(ret)break;
	}
    touchI2CStop();					//����һ��ֹͣ����

	return ret;
}

/*******************************************************************************
* Function Name  : touchGt9147RdReg
* ��GT9147����һ������
* reg:��ʼ�Ĵ�����ַ
* buf:���ݻ�������
* len:�����ݳ���
*******************************************************************************/
static void touchGt9147RdReg(u16 reg,u8 *buf,u8 len)
{
	u8 i;

 	touchI2CStart();
 	touchI2CSendByte(GT_CMD_WR);	//����д����
	touchI2CWaitAck();
 	touchI2CSendByte(reg>>8);		//���͸�8λ��ַ
	touchI2CWaitAck();
 	touchI2CSendByte(reg&0XFF);		//���͵�8λ��ַ
	touchI2CWaitAck();
 	touchI2CStart();
	touchI2CSendByte(GT_CMD_RD);	//���Ͷ�����
	touchI2CWaitAck();
	for(i=0;i<len;i++)
	{
    	buf[i] = touchI2CReadByte(i==(len-1)?0:1); //������
	}
    touchI2CStop();					//����һ��ֹͣ����
}

/*******************************************************************************
* Function Name  : touchGt9147SendCfg
* ����GT9147���ò���
*******************************************************************************/
static void touchGt9147SendCfg(void)
{
	u8 buf[2];
	u8 i=0;

	buf[0] = 0;
	buf[1] = 1;	//������д�뵽GT9147 FLASH,  �����籣��

	for(i=0; i<sizeof(GT9147_CFG_TBL); i++)
	{
		buf[0] += GT9147_CFG_TBL[i];//����У���
	}
    buf[0] = (~buf[0])+1;

	touchGt9147WrReg(GT_CFGS_REG, (u8*)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL));//���ͼĴ�������
	touchGt9147WrReg(GT_CHECK_REG, buf, 2);//д��У���,�����ø��±��
}

/*******************************************************************************
* Function Name  : touchGt9147Init
* ��ʼ��GT9147������
*******************************************************************************/
void touchGt9147Init(void)
{
	u8 temp[5];
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);	//ʹ��GPIOB,Cʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			//PB1����Ϊ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//��ʼ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			//PC13����Ϊ�������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);				//��ʼ��

	touchI2CInit();      	//��ʼ����������I2C����
	GT_RST = 0;				//��λ
	delay_ms(10);
 	GT_RST = 1;				//�ͷŸ�λ
	delay_ms(10);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			//PB1����Ϊ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//��ʼ��

	delay_ms(100);

	temp[0] = 0x02;
	touchGt9147WrReg(GT_CTRL_REG,temp,1);				//��λGT9147
	touchGt9147RdReg(GT_CFGS_REG,temp,1);				//��ȡGT_CFGS_REG�Ĵ���
	if(temp[0] < GT9147_CFG_TBL[0])						//����ǰ�汾���ڳ����ж���汾,�����flash����
	{
		touchGt9147SendCfg();							//���²���������
	}
	delay_ms(10);
	temp[0]=0X00;

	touchGt9147WrReg(GT_CTRL_REG,temp,1);				//������λ
}

/*******************************************************************************
* Function Name  : touchGt9147Scan
* ������ɨ��
*******************************************************************************/
u32 touchGt9147Scan(void)
{
	u16	i, j;
	u16	x, y;
	u8	val;
	u8	buf[4];
	u32 key = 0;

	touchGt9147RdReg(GT_GSTID_REG, &val, 1);	// ��ȡ�������״̬
	if((val&0x80) && ((val&0x0F)<6))
	{
		// ��������ݿɶ�ȡ���ҵ���������5�������ȡ����
		// ���־
		buf[0] = 0;
		touchGt9147WrReg(GT_GSTID_REG, buf, 1);

		val &= 0x0F;	// ��ȡ������
		for(i=0; i<val; i++)
		{
			// ��ȡ��������
			touchGt9147RdReg(GT9147_TPX_TBL[i], buf, 4);
#if BSP_LCD_SWAP_XY
			y = ((u16)buf[1]<<8) + buf[0];
			x = ((u16)buf[3]<<8) + buf[2];
#else
			x = ((u16)buf[1]<<8) + buf[0];
			y = ((u16)buf[3]<<8) + buf[2];
#endif

// ��ʵ����֤����������������ʾ�����귽���෴
// ������Ļ����X��Y�����ʱ�򣬴�����Ҫ����
#if !BSP_LCD_MIRROR_X
			x = LCD_XSIZE - x - 1;
#endif
#if !BSP_LCD_MIRROR_Y
			y = LCD_YSIZE - y - 1;
#endif

			// �жϴ����Ӧ�İ���
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

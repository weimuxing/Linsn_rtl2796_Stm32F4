
#include "SysConfig.h"

#include "I2C.h"
#include "AD4604.h"
#include "UartDebug.h"



#ifdef USE_ADN4604


//u8 input_table[16] = {6,7,4,5,2,3,0,1,14,15,12,13,10,11,8,9};
u8 input_table[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
AD4604OUT output_table[16];

// ad4604_hardware_xxx 只是硬件连接的序号，在实际输出信号会因为设置而变
ad4604_hardware_wire ad4604_hardware_input[AD4604_IN_SIGNAL_MAX] = 
{
	{AD4604_HDMI_IN,	{AD4604_IN_PIN0,  AD4604_IN_PIN1,	AD4604_IN_PIN2,	   AD4604_IN_PIN3}},
	{AD4604_DVI_IN,		{AD4604_IN_PIN4,  AD4604_IN_PIN5,	AD4604_IN_PIN6,	   AD4604_IN_PIN7}},
	{AD4604_SDI_IN,		{AD4604_IN_PIN12,  AD4604_IN_PIN13,	AD4604_IN_PIN14,   AD4604_IN_PIN15}},
};

//ad4604_hardware_wire ad4604_hardware_input[AD4604_IN_SIGNAL_MAX] = 
//{
//	{.signal=AD4604_HDMI_IN,	.pin = {AD4604_IN_PIN0,  AD4604_IN_PIN1,	AD4604_IN_PIN2,	   AD4604_IN_PIN3}},
//	{.signal=AD4604_DVI_IN,		.pin = {AD4604_IN_PIN4,  AD4604_IN_PIN5,	AD4604_IN_PIN6,	   AD4604_IN_PIN7}},
//	{.signal=AD4604_SDI_IN,		.pin = {AD4604_IN_PIN12,  AD4604_IN_PIN13,	AD4604_IN_PIN14,   AD4604_IN_PIN15}},
//};


ad4604_hardware_wire ad4604_hardware_ouput[AD4604_OUT_SIGNAL_MAX] = 
{
	{AD4604_IT6604_OUT,		{AD4604_OUT_PIN0,  AD4604_OUT_PIN1,	AD4604_OUT_PIN2,	   AD4604_OUT_PIN3}},
	{AD4604_PREVIEW_OUT,	{AD4604_OUT_PIN4,  AD4604_OUT_PIN5,	AD4604_OUT_PIN6,	   AD4604_OUT_PIN7}},
	{AD4604_HDMI_OUT,		{AD4604_OUT_PIN8,  AD4604_OUT_PIN9,	AD4604_OUT_PIN10,	   AD4604_OUT_PIN11}},		// 对应9320 INPUT_HDMI
	{AD4604_DVI_OUT,		{AD4604_OUT_PIN12,  AD4604_OUT_PIN13,	AD4604_OUT_PIN14,  AD4604_OUT_PIN15}},		// 对应9320 INPUT_DVI
};

// 下标是输出通道，值是输入通道
ad6404_input_signal Ad4604_Xpt_Map[AD4604_OUT_SIGNAL_MAX];


void AD4604_Reset(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	DebugMessageAD4604("%s\r\n",__func__);
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(AD4604_RESET_AHB1PERIPH, ENABLE);

	/* Configure  as GPIO out  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = AD4604_RESET_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(AD4604_RESET_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(AD4604_RESET_PORT,GPIO_InitStructure.GPIO_Pin);
	SysDelay1ms(100);
	GPIO_SetBits(AD4604_RESET_PORT,GPIO_InitStructure.GPIO_Pin);
	SysDelay1ms(10);
}


void AD4604_Write(u8 address, u8 value)
{
	static char isCheck = 0;
	BYTE slaveAddress, readValue;
	int i;

	//for(i=0; i<4; i++)
	for(i=3; i<4; i++)// by hibernate
	{
		slaveAddress = 0x90+2*i;
	
		i2cWriteByte(AD4604_I2C_PORT, slaveAddress, address, value);//enable
	}
	
	#if 1
	if(address == 0x80) return;
	//for(i=0; i<4; i++)
	for(i=3; i<4; i++)// by hibernate
	{
		slaveAddress = 0x90+2*i;
		
		readValue = i2cReadByte(AD4604_I2C_PORT, slaveAddress, address);
		
		if(readValue != value)
		{
			// Cancel by hibernate
			//printf("AD4604: 0x%x, 0x%x = 0x%x, expect 0x%x\r\n", slaveAddress, address,readValue,value );
			i2cWriteByte(AD4604_I2C_PORT, slaveAddress, address, value);
		}
	}
	
	#endif
	if(!isCheck)
	{
		int i;
		//for(i=0; i<4; i++)
		for(i=3; i<4; i++)		// by hibernate
		{
			if(i2cReadByte(AD4604_I2C_PORT,0x90+i*2,0xff) != 0x04)
			{
				// Cancel by hibernate
				//printf("AD4604 0x%x read id error\r\n", 0x90+i*2);
			}
			
		}
		isCheck = 1;
	}
}

u8 AD4604_Read(u8 address)
{
	return i2cReadByte(AD4604_I2C_PORT, 0x96, address);		// By hibernate
	//return i2cReadByte(AD4604_I2C_PORT, 0x90, address);
}

/*******************************************************************************
* Function Name  : AD4604_DisableAll
* Description    : disable all output 
* Input          : input 
* Output         : None
* Return         : None
*******************************************************************************/
void AD4604_DisableOutput(u8 output)
{
	output_table[output-1].EnSta = DISABLE;

	AD4604_Write(0x20+output-1, 0x00);//disable

}

char _outputDriver = 0;
void AD4604_SetOutputDriver(int value)
{
	_outputDriver = value & 0x07;
}

int AD4604_GetOutputDriver(void)
{
	return _outputDriver;
}
/*******************************************************************************
* Function Name  : AD4604_EnableAll
* Description    : disable all output 
* Input          : input 
* Output         : None
* Return         : None
*******************************************************************************/
void AD4604_EnableOutput(u8 output)
{
	AD4604_Write(0x20+output-1, 0x30|_outputDriver);//enable
	output_table[output-1].EnSta = ENABLE;
}

int  AD4604_GetEnable(u8 output)
{
	if((output == 0) || (output>(sizeof(output_table)/sizeof(AD4604OUT))))
	{
		return 0;
	}
	return output_table[output-1].EnSta;
}


/*******************************************************************************
* Function Name  : AD4604_DisableAll
* Description    : disable all output 
* Input          : input 
* Output         : None
* Return         : None
*******************************************************************************/
void AD4604_DisableAll(void)
{
	u16 i;
	for(i = 0; i < 16; i++)
	{
		output_table[i].EnSta = DISABLE;

		AD4604_Write(0x20+i, 0x00);//enable
	}

}
/*******************************************************************************
* Function Name  : AD4604_EnableAll
* Description    : disable all output 
* Input          : input 
* Output         : None
* Return         : None
*******************************************************************************/
void AD4604_EnableAll(void)
{
	u16 i;
	for(i = 0; i < 16; i++)
	{
	   AD4604_EnableOutput(i+1);
	}
}


void AD4604_SwitchInputToAll(u8 value)
{
	int i;
	u8 temp = (input_table[value-1])<<4|input_table[value-1];

	for(i = 0; i < 16; i++)
	{	    
		output_table[i].InputCh = input_table[value-1];
	}
	
	AD4604_EnableAll();
	
	AD4604_Write( 0x90,temp);
	AD4604_Write( 0x91,temp);
	AD4604_Write( 0x92,temp);
	AD4604_Write( 0x93,temp);
	AD4604_Write( 0x94,temp);
	AD4604_Write( 0x95,temp);
	AD4604_Write( 0x96,temp);
	AD4604_Write( 0x97,temp);

	AD4604_Write( 0x80, 1);//update
}

void AD4604_SwitchInputToOutput(u8 input,u8 output)  // input = 1 ~ 16, output = 1 ~ 16
{
	u8 temp = 0;
	u8 value = input-1;
	u8 value1 = output -1;


	AD4604_EnableOutput(output);//enable

	output_table[value1].InputCh = input_table[value];
		
	temp = AD4604_Read(0x90+((value1)/2));
	if((value1 & 0x01)==0)
	{
		temp = (temp&0xf0) | input_table[value];
		AD4604_Write( 0x90+((value1)/2), temp);
	}
	else
	{
		temp = (temp&0x0f) | (input_table[value]<<4);
		AD4604_Write( 0x90+((value1)/2), temp);
	}
				
	AD4604_Write( 0x80, 1);//update
}


u8 AD4604_GetInterInputPort(u8 input)
{
	return input_table[input];
}

u8 AD4604_GetInput(u8 output)
{
	output = output - 1;
	if(output < 16)
	{
		int input = output_table[output].InputCh;
		int i;
		for(i=0; i<16; i++)
		{
			if(input_table[i] == input)
			{
				break;
			}
		}
		return i+1;
	}
	else
	{
		return 0;
	}
	
}

void AD4604_writeTest(void)
{
	static u8 testPort = 0,errrorWrite = 0;
	//static u32 testAD44604_Time,
	u32 cnt_total,cnt_false;
	u8 temp,i,readReg;
	//char buffer[30];

	//if(errrorWrite == 1)
	//{
	//	return ;
	//}
	
	//if(!isSysTickTimeout(testAD44604_Time,1000))
	//{
	//	return;
	//}
	//testAD44604_Time = GetSysTick();

	testPort++;
	if(testPort > 15)
	{
		testPort = 0;
	}

	temp = (testPort)<<4|testPort;
	
	for(i = 0; i < 8; i++)
	{
		i2cWriteByte(AD4604_I2C_PORT, 0x96, 0x90+i, temp);
		cnt_total++;
		//printf("AD4604 cnt_total = %d,",cnt_total);
		SysDelay1ms(5);
		readReg = i2cReadByte(AD4604_I2C_PORT, 0x96, 0x90+i);
		if(temp != readReg)
		{
			//printf( "readReg = %x,temp = %x,",readReg,temp);
			cnt_false++;
		}
		//printf("cnt_false = %d\r\n",cnt_false);
		SysDelay1ms(5);
	}	
	
	/*
	if(cnt_false > 0)
	{
		errrorWrite = 1;
	}
	*/
}

void AD4604DisableOutPin(ad6404_ouput_pin outPin)
{
	AD4604_Write(0x20 + outPin, 0x00);//disable
}


void AD4604EnableInPinToOutPin(ad6404_input_pin inPin,ad6404_ouput_pin outPin)
{
	u8 regValue = 0;	

	//DebugMessageAD4604("%s\r\n",__func__);

	if(AD4604_IN_PIN_MAX <= inPin || AD4604_OUT_PIN_MAX <= outPin)
	{
		return;
	}

	regValue = AD4604_Read(0x90 + outPin/2);	
	if((outPin & 0x01) == 0)
	{
		regValue = (regValue&0xf0) | inPin;
	}
	else
	{
		regValue = (regValue&0x0f) | (inPin<<4);
	}
	AD4604_Write(0x90 + (outPin/2), regValue);
	
	AD4604_Write(0x20 + outPin, 0x30);//enable

}

void AD4604EnableInPinToOutPin_II(ad6404_input_pin inPin,ad6404_ouput_pin outPin) //lv@20160708
{
	u8 regValue = 0;	

	//DebugMessageAD4604("%s\r\n",__func__);

	if(AD4604_IN_PIN_MAX <= inPin || AD4604_OUT_PIN_MAX <= outPin)
	{
		return;
	}

	regValue = AD4604_Read(0x90 + outPin/2);	
	if((outPin & 0x01) == 0)
	{
		regValue = (regValue&0xf0) | inPin;
	}
	else
	{
		regValue = (regValue&0x0f) | (inPin<<4);
	}
	AD4604_Write(0x90 + (outPin/2), regValue);
	
	//AD4604_Write(0x20 + outPin, 0x30);//enable

}

u8 Ad4604FindSignalIdx(ad4604_hardware_wire * hardware_wire,u8 signal)
{
	u8 i;
	u8 hardware_wire_size;

	hardware_wire_size = sizeof(hardware_wire);
	for(i = 0;i < hardware_wire_size;i++)
	{
		if(hardware_wire[i].signal == signal)
		{
			break;
		}
	}

	return i;
}

void AD4604EnableInputToOutput(u8 input_ch,u8 output_ch)
{
	u8 i;
	u8 temp = 0;	
	u8 inIdx,outIdx;
	u8 inPin,outPin;
	ad6404_input_signal input;
	ad6404_ouput_signal output;	

	input = (ad6404_input_signal)input_ch;
	output = (ad6404_ouput_signal)output_ch;

	DebugMessageAD4604("Switch Input %d to output %d \r\n",input,output);
	
	if(AD4604_IN_SIGNAL_MAX <= input || AD4604_OUT_SIGNAL_MAX <= output)
	{
		return;
	}

	//if(Ad4604_Xpt_Map[output] == input)
	//{
	//	return;
	//}	

	Ad4604_Xpt_Map[output] = input;

	inIdx = Ad4604FindSignalIdx(ad4604_hardware_input,input);
	outIdx = Ad4604FindSignalIdx(ad4604_hardware_ouput,output);	

	for(i = 0; i < AD4604_SIGNAL_WIRE;i++)
	{
		//inPin = ad4604_hardware_input[inIdx].pin[i];
		outPin = ad4604_hardware_ouput[outIdx].pin[i];

		AD4604DisableOutPin(outPin);
	}

	AD4604_Write( 0x80, 1);//update
	SysDelay1ms(100);
	
	for(i = 0; i < AD4604_SIGNAL_WIRE;i++)
	{
		inPin = ad4604_hardware_input[inIdx].pin[i];
		outPin = ad4604_hardware_ouput[outIdx].pin[i];

		//AD4604EnableInPinToOutPin(inPin,outPin);
		AD4604EnableInPinToOutPin_II(inPin,outPin);//lv@20160708
	}

	AD4604_Write( 0x80, 1);//update	

	for(i = 0; i < AD4604_SIGNAL_WIRE;i++)//lv@20160708
	{
		outPin = ad4604_hardware_ouput[outIdx].pin[i];
		AD4604_Write(0x20 + outPin, 0x30);//enable
	}
	
}

void AD4604SwitchInputToOutput(ad6404_input_signal input,ad6404_ouput_signal * outputList)
{
	u8 i,j;
	u8 temp = 0;	
	u8 inIdx,outIdx;	
	u8 inPin,outPin;
	u8 output_max = outputList[0];

	DebugMessageAD4604("%s\r\n",__func__);

	if(AD4604_IN_SIGNAL_MAX <= input)
	{
		return;
	}
	if(AD4604_OUT_SIGNAL_MAX <= output_max)
	{
		output_max = AD4604_OUT_SIGNAL_MAX;
	}

	for(i = 0; i < output_max;i++)
	{
		if(Ad4604_Xpt_Map[i] == input)
		{
			continue;
		}	
		Ad4604_Xpt_Map[i] = input;

		inIdx = Ad4604FindSignalIdx(ad4604_hardware_input,input);
		for(j = 0; j < AD4604_SIGNAL_WIRE;j++)
		{
			inPin = ad4604_hardware_input[inIdx].pin[j];
			outPin = ad4604_hardware_ouput[outputList[i+1]].pin[j];

			AD4604EnableInPinToOutPin(inPin,outPin);
		}
	}

	AD4604_Write( 0x80, 1);//update		
}

void AD4604SwitchInputToAll(ad6404_input_signal input)
{
	u8 i,j;
	u8 temp = 0;	
	u8 inPin,outPin;
	u8 inIdx;

	DebugMessageAD4604("%s\r\n",__func__);
	if(AD4604_IN_SIGNAL_MAX <= input)
	{
		return;
	}

	AD4604_DisableAll();
	SysDelay1ms(100);

	for(i = 0; i < AD4604_OUT_SIGNAL_MAX;i++)
	{
		if(Ad4604_Xpt_Map[i] == input)
		{
			continue;
		}
		Ad4604_Xpt_Map[i] = input;
		inIdx = Ad4604FindSignalIdx(ad4604_hardware_input,input);
		for(j = 0; j < 4;j++)
		{
			inPin = ad4604_hardware_input[inIdx].pin[j];
			outPin = ad4604_hardware_ouput[i].pin[j];

			AD4604EnableInPinToOutPin(inPin,outPin);
		}
	}

	AD4604_Write( 0x80, 1);//update		
}

u8 AD4604_GetInputPort(ad6404_ouput_signal output)
{
	if(AD4604_OUT_SIGNAL_MAX <= output)
	{
		return AD4604_OUT_SIGNAL_MAX;
	}

	return Ad4604_Xpt_Map[output];
}


void setSwitchToOutputAndOutputEnable(u8 output,u8 input)// output 0-1   input  0-3 
{
	BYTE reg[2],value[2],i,cus[4];
	u8 enable[4] = {0x30,0x30,0x30,0x30};	

	switch(output)
	{
		case 0:
			reg[0] = 0;
			reg[1] = 1;
			break;
		case 1:
			reg[0] = 2;
			reg[1] = 3;
			break;
			
		default:
			return;
	}
	
	i2cBurstReadBytes(AD4604_I2C_PORT,0x96,0x94,cus,4);
	switch(input)
	{
		case 0x00:
			value[0] = 0x10;
			value[1] = 0x32;
			break;
		case 0x01:
			value[0] = 0x54;
			value[1] = 0x76;
			break;
		case 0x02:
			value[0] = 0x98;
			value[1] = 0xba;
			break;
		case 0x03:
			value[0] = 0xdc;
			value[1] = 0xfe;
			break;
			
		default:
			return;
			
	}
	cus[reg[0]] = value[0];
	cus[reg[1]] = value[1];
	
	//SwitchStatus[output] = input;
	
	i2cBurstWriteBytes(AD4604_I2C_PORT,0x96,0x94,cus,4);
	
	AD4604_Write( 0x80, 1);//update
	i2cBurstWriteBytes(AD4604_I2C_PORT,0x96,0x28+(output*4),enable,4);
	
}

#endif		// USE_ADN4604


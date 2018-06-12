/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
 
#include "RotaryEncoder.h"
#include "define.h"
#include <stdio.h>
#include "stdp9320.h"
//#include "mst6m48.h"
#include "uart.h"
#include "UartDebug.h"

#if 0		// Device
#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
#else	// Host
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#endif




/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                    USB_Host;

extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif


/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	printf("HardFault_Handler\r\n");
	while (1)
	{
	}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	printf("MemManage_Handler\r\n");
	while (1)
	{
	  
	}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	printf("BusFault_Handler\r\n");
	while (1)
	{
	}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	printf("UsageFault_Handler\r\n");
	while (1)
	{
	}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void IsrSysTick(void);
void SysTick_Handler(void)
{
	IsrSysTick();
}




extern void IsrMSTV(BYTE value); // mstv.c
extern void IsrReceiveCommand(int from, BYTE value); // rs232command.c
char uart0ToUartx = 0;//bit2- uart2     bit3- uart3   bit6- uart6
extern BYTE UART1Buffer_Receive[512];
//extern int recv_count;
extern DWORD ReceiveCount;


#if 0
void USART1_IRQHandler(void)
{
#if 0
	USB_COM_IRQHandler();	
#else
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* Send the received data to the PC Host*/
		u8 value;
		value = USART_ReceiveData(USART1);
		//IsrMSTV(value);
		USART2->DR = value;
		IsrReceiveCommand(0, value);
		//USART2->DR = value;
		switch(uart0ToUartx)
		{
			case 0x02:
				USART2->DR = value;
				break;
			case 0x04:
				USART3->DR = value;
				break;
			case 0x08:
				UART4->DR = value;
				break;
			case 0x20:
				USART6->DR = value;
				break;
		}
	}
#endif

}
#endif

//extern void Isr_receive_mst6m48(u8 value);
//extern void Isr_receive_video(u8 value);
extern void Isr_receive_2796(u8 value);
void USART2_IRQHandler(void)
{

	while (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		/* Send the received data to the PC Host*/
		u8 value = USART_ReceiveData(USART2);
		//Isr_receive_video(value);
		

		#if 1
		#endif
	}
}



extern void Isr_receive_Input(u8 value);
void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		/* Send the received data to the PC Host*/
		u8 value;
		value = USART_ReceiveData(USART3);
		Isr_receive_2796(value);
		//USART6->DR = value;
	}

}

extern void Isr_receive_led(u8 value);
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		/* Send the received data to the PC Host*/
		u8 value;
		value = USART_ReceiveData(UART4);
		//Isr_receive_led(value);			// Cancell by hibernate
		//IsrReceiveCommand(3, value); //如果这样调用,很容易出现不能正常升级103.
        if(getCommHardVersion() > 0)
        {
            UART1Buffer_Receive[ReceiveCount++] = value;
        }		
		if(uart0ToUartx == 0x08)
		{
			USART1->DR = value;
		}
	}
}

void UART5_IRQHandler(void)
{
	while(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		/* Send the received data to the PC Host*/
//		u8 value;
//		value = USART_ReceiveData(UART5);
		//UartBuf[ReceiveCount++] = value;
		//recv_count++;
	}
}


void GetDebugUartCmd(u8 value);
//###############################################################################################
// 20160413
void USART6_IRQHandler(void)
{
	while(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
	{
		/* Send the received data to the PC Host*/
		u8 value;
		value = USART_ReceiveData(USART6);
		USART3->DR = value;
		GetDebugUartCmd(value);
	}

}


extern vu16 EcInputCountNew;
extern vu16 EcInputCountOld;
extern u8 IsEcInputFinish;
BYTE EcNoInputCount = 0;
extern void menuIsrTimerHandler(void); /* menu.c */
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
		menuIsrTimerHandler();
	}
}


extern void TaskKeyLed(void);

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
		TaskKeyLed();
		#if 0
		if(GetEcInputValueOld() != GetEcInputValueNew())
		{
			SetEcInputValueOld();
			EcNoInputCount = 0;
			SetIsEcInputFinish(0);
			DebugMessageSystem("Ec11 is oprating !  %d\r\n",EcInputCountNew);
		}
		else
		{
			EcNoInputCount++;
			if(EcNoInputCount > 6)
			{
				ClearEcInputValueNew();
				ClearEcInputValueOld();
				EcNoInputCount = 0;
				SetIsEcInputFinish(1);
				DebugMessageSystem("Ec11 is no oprating !  %d\r\n",EcNoInputCount);
			}
		}
		#endif
	}
}

extern void fan_gpio_set(void);
void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);
        fan_gpio_set();
    }
}



/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS  
void OTG_FS_WKUP_IRQHandler(void)
{
#if 0

  if(USB_OTG_Core.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ; 
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_Core);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
 #endif
}
#endif

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS  
void OTG_HS_WKUP_IRQHandler(void)
{
	
#if 0
  if(USB_OTG_Core.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ; 
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_Core);
  }
  EXTI_ClearITPendingBit(EXTI_Line20);
 #endif
}
#endif


extern uint32_t USBH_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS  
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_Core);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
  
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_Core);
}
#endif



/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

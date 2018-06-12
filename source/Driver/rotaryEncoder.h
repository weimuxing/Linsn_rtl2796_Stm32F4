#ifndef _ROTARY_ENCODER_H_
#define _ROTARY_ENCODER_H_

#include "define.h"
typedef enum pulse_port
{
	pulse_A,
	pulse_B,
	pulse_Max,
}pulse_port_typedef; 

/* pluse A io source config*/
//PD15	PULSE_A
//PD14	PULSE_B

#define PULSEA_INT_PORT  GPIOG
#define PULSEA_INT_PIN	 GPIO_Pin_15

#define PULSEA_DIR_PORT  GPIOG
#define PULSEA_DIR_PIN   GPIO_Pin_9

#define PULSEA_EXTI_PORT EXTI_PortSourceGPIOG
#define PULSEA_EXTI_SRC  EXTI_PinSource15
#define PULSEA_EXTI_LINE EXTI_Line15

#define PULSEA_EXTI_IRQ  EXTI15_10_IRQn
#define PULSEA_EXTI_TRG  EXTI_Trigger_Rising




/* pluse B io source config*/
//PC13	PULSE_A
//PD0	PULSE_B
#define PULSEB_INT_PORT  GPIOG
#define PULSEB_INT_PIN	 GPIO_Pin_9

#define PULSEB_DIR_PORT  GPIOG
#define PULSEB_DIR_PIN   GPIO_Pin_15

#define PULSEB_EXTI_PORT EXTI_PortSourceGPIOG
#define PULSEB_EXTI_SRC  EXTI_PinSource9
#define PULSEB_EXTI_LINE EXTI_Line9

#define PULSEB_EXTI_IRQ  EXTI9_5_IRQn
#define PULSEB_EXTI_TRG  EXTI_Trigger_Rising




void init_Pluse(u8 pluse);

int GetpluseValue(u8 pluse);

void PLUSE_VNIC(u8 pluse);

void Pulse_test(void);

#endif // _ROTARY_ENCODER_H_



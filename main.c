/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */
#define _COSMIC_

#include <stm8l10x_conf.h> 
 
 
#define STARTUP_SWIM_DELAY_5S \
{_asm(\
"				PUSHW X				\n"	\
" 			PUSH A				\n"	\
"				LDW X,#0xFFFF	\n"	\
"loop1:	LD 	A,#50			\n"	\
													\
"loop2: DEC A					\n"	\
"				JRNE loop2		\n"	\
													\
"				DECW X				\n"	\
"				JRNE loop1		\n"	\
													\
"				POP A					\n"	\
"				POPW X					");\
}


#define CONFIG_UNUSED_PINS_STM8L001 \
{\
	GPIOA->DDR |= GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5;\
	GPIOB->DDR |= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4;\
	GPIOC->DDR |= GPIO_Pin_5 | GPIO_Pin_6;\
	GPIOD->DDR |= GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;\
}

main()
{
	uint16_t TIM3_Pulse = 0;
	uint16_t Pulse_Width = 200;
	uint16_t Pulse_Period = 10000;
	int16_t Count_Direction = 1;
	CONFIG_UNUSED_PINS_STM8L001;
	STARTUP_SWIM_DELAY_5S;
	
	GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_Out_PP_High_Fast);
	
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3,ENABLE);

	TIM3_DeInit();
	TIM3_TimeBaseInit(TIM3_Prescaler_1,TIM3_CounterMode_Up,Pulse_Period);
	
	TIM3_OC2Init(TIM3_OCMode_PWM1,TIM3_OutputState_Enable,Pulse_Width,TIM3_OCPolarity_High,TIM3_OCIdleState_Set);
	TIM3_OC2PreloadConfig(ENABLE);
	
	TIM3_ARRPreloadConfig(ENABLE);
	TIM3_CtrlPWMOutputs(ENABLE);
	TIM3_Cmd(ENABLE);
	
	GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_Out_PP_High_Fast);
	
	GPIO_Init(GPIOA,GPIO_Pin_2,GPIO_Mode_In_FL_IT);
	EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Rising_Falling);
	
	WFE_WakeUpSourceEventCmd(WFE_Source_EXTI_EV2, ENABLE);
	
	enableInterrupts();

  wfe();
	
	EXTI_ClearITPendingBit(EXTI_IT_Pin2);
	
	while (1){
		int count = 1000;
		while(count --);
		if(Count_Direction == 1 && Pulse_Width > 9800)
				Count_Direction = -1;
		else if(Pulse_Width < 200)
				Count_Direction = 1;
		Pulse_Width += Count_Direction*50;
		TIM3_SetCompare2(Pulse_Width);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)){
			int count = 50000;
			while(count --);
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)){
				TIM3_Cmd(DISABLE);
				while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)){
					
				}
				wfe();
				EXTI_ClearITPendingBit(EXTI_IT_Pin2);
			}
		}else
		{
			TIM3_Cmd(ENABLE);
		}
	}
}
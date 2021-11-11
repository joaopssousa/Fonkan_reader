/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Bleeper board GPIO driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @author  MCD Application Team
  * @brief   manages interrupt
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "stm32f4xx_it.h"
#include "handlers.h"
#include "chafon_4_antennas.h"

extern uint8_t ble_state;
uint8_t count_tim3 = 0;
extern int b;

extern DMA_HandleTypeDef hdma_sdio_rx;
extern DMA_HandleTypeDef hdma_sdio_tx;
extern SD_HandleTypeDef hsd;

extern unsigned char flag_connection;

extern int count_send;


/** @addtogroup SPI_FullDuplex_ComPolling
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
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
  while (1)
  {
    __NOP();
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
void SysTick_Handler(void)
{
  HAL_IncTick();
}

void TIM2_IRQHandler(void)
{
	flags_ble.rfid_send_cmd = SET;

	HAL_TIM_IRQHandler(&htim2);
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

	// Dispara a cada 100ms para teste de conexão
	/*
	 * 	Timer dispara a cada 100 ms e se tiver 10 contagens, sinaliza que
	 * 	o pino de estado se manteve em alta e conexão foi bem sucedida.
	 */

	ble_state = 1; //HAL_GPIO_ReadPin(BLE_STATE_GPIO_Port,BLE_STATE_Pin);
	if (ble_state == 1)
	{
		if (++count_tim3 > 9)
		{
			flags_ble.connection = SET;
			count_tim3 = 0;
		}
	}
	else
	{
		flags_ble.connection = RESET;
		count_tim3 = 0;
	}

//	// Para as requisições de TAG pois a conexão foi quebrada
	if(flags_ble.connection == RESET)
	{
		HAL_TIM_Base_Stop_IT(&htim2);
	}

	if(flags_ble.start == SET){
		if(count_send++ == 50)
		{
			flag_send_timeout = SET;
			count_send = 0;
		}
	}


	HAL_NVIC_ClearPendingIRQ(TIM3_IRQn); // limpa flags de interrupção

	HAL_TIM_IRQHandler(&htim3);

}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{


	HAL_UART_IRQHandler(&huart1);
	if(ble_index>sizeof(message_ble))
		ble_index=0;
	message_ble[ble_index] = rx_byte_uart1[0];
	ble_index++;
	if(ble_index>2){
		if(message_ble[0] == 0xa){
			if(message_ble[ble_index-1] == 0xd)
			{
				// Sinaliza que chegou uma mensagem válida
				ble_index = 0;								// Zera o índice para nova mensagem
				flags_ble.enable_handler = 1;
			}
		}
	}

	HAL_NVIC_ClearPendingIRQ(USART1_IRQn);
	HAL_UART_Abort_IT(&huart1);
	HAL_UART_Receive_IT(&huart1, rx_byte_uart1, 1);

}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

	/*
	 * 	@brief Porta serial do RFID
	 *
	 * 	A cada envio de requisição de leitura pelo timer
	 * 	o módulo responde conforme a leitura ou apenas
	 * 	resposta padrão (caso da versão FW). O fim de toda
	 * 	mensagem é padrão, logo ao se detectar o caracter 0x0D
	 * 	habilitar a flag que permite tratar a mensagem.
	 */

  /* USER CODE END USART2_IRQn 0 */
	HAL_UART_IRQHandler(&huart2);
	/* USER CODE BEGIN USART2_IRQn 1 */


#ifdef USE_FONKAN_1_ANTENNA
	message[message_index] = rx_byte_uart2[0];
	message_index++;
	/*
	 * Testa se recebeu o fim da messagem 0x0D.
	 */
	if (message_index > 3)
	{
		if ((message[message_index - 4] == 0x0A)
				&& ((message[message_index - 3] == 0x55) || (message[message_index - 3] == 0x58))
				&& (message[message_index - 2] == 0x0D)
				&& (message[message_index - 1] == 0x0A))
		{
			flags_ble.tag = SET;// Aciona a flag mostrando que recebeu mensagem válida
			bytes_read_rfid = message_index;
			message_index=0;
		}
	}
	HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
	HAL_UART_Abort_IT(&huart2);
	HAL_UART_Receive_IT(&huart2, rx_byte_uart2, 1);
#endif

#ifdef USE_CHAFON_4_ANTENNAS

	data[count_byte++] =  reciver_buffer[0];
		if(count_byte == data[0]+1)
		{
			count_byte = 0;
			communication_validation_flag = 1;

		}


		HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
		HAL_UART_Abort_IT(&huart2);
		HAL_UART_Receive_IT(&huart2, reciver_buffer, 1);
#endif


	/* USER CODE END USART2_IRQn 1 */
}


/**
  * @brief This function handles SDIO global interrupt.
  */
void SDIO_IRQHandler(void)
{
  /* USER CODE BEGIN SDIO_IRQn 0 */

  /* USER CODE END SDIO_IRQn 0 */
  HAL_SD_IRQHandler(&hsd);
  /* USER CODE BEGIN SDIO_IRQn 1 */

  /* USER CODE END SDIO_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

  /* USER CODE END DMA2_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sdio_rx);
  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

  /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream6 global interrupt.
  */
void DMA2_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream6_IRQn 0 */

  /* USER CODE END DMA2_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sdio_tx);
  /* USER CODE BEGIN DMA2_Stream6_IRQn 1 */

  /* USER CODE END DMA2_Stream6_IRQn 1 */
}

void USARTx_IRQHandler(void)
{
  vcom_IRQHandler();
}

void USARTx_DMA_TX_IRQHandler(void)
{
  vcom_DMA_TX_IRQHandler();
}

void RTC_Alarm_IRQHandler(void)
{
  HW_RTC_IrqHandler();
}

void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}


void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

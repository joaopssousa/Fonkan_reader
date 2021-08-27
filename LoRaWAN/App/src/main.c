/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   this is the main!
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
#include "main.h"
#include "hw.h"
#include "low_power_manager.h"
#include "lora.h"
#include "bsp.h"
#include "timeServer.h"
#include "vcom.h"
#include "version.h"
#include "curral.h"
#include "handlers.h"
#include "fw_update_app.h"
#include "com.h"


#define DEBUG_SD 1
#define PRINT_SD_CARD(X) do{ if(DEBUG_SD>0) { X } }while(0);


/* Variaveis curral */

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;




// Verificação de conexão para envio ao gateway
_Bool flag_lora_joined = RESET;

// Contador de teste
int8_t t = 0;
uint8_t flag_send_to_lora = LORA_RESET;
Model_TAG  tag_to_lora;
Model_TAG pack_to_lora[10];

// Tamanho e vetor de dados para usar a serial para testes.
uint16_t size;
char Data[256];

// Tamanho do buffer de dados para envio
#define DATA_BUFF_SIZE                  64

// User application data
uint8_t AppDataBuff[DATA_BUFF_SIZE];

// Estrutura do dados para envio
lora_AppData_t AppData = { AppDataBuff,  0, 0 };

// Buffer de envio
char Buffer_to_send[sizeof(Model_TAG)] = { 0 };

// Estrutura de tempo e data para RTC
RTC_TimeTypeDef currTime;
RTC_DateTypeDef currDate;
uint8_t b = 0;


_Bool delay_flag = RESET;
char buffer_tag[50];
#define LOG_FILE "STORE.TXT"


uint8_t pack_position = 0;
//#define SDCARD_IN_USE


/*--------------------*/


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define LORAWAN_MAX_BAT   254


#define USED_POWER TX_POWER_0

#define LOW_POWER_DISABLE
/*!
 * Defines the application data transmission duty cycle. 5s, value in [ms].
 */
#define APP_TX_DUTYCYCLE                            10000
/*!
 * LoRaWAN Adaptive Data Rate
 * @note Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_STATE LORAWAN_ADR_OFF //modified - old - ON
/*!
 * LoRaWAN Default data Rate Data Rate
 * @note Please note that LORAWAN_DEFAULT_DATA_RATE is used only when ADR is disabled
 */
#define LORAWAN_DEFAULT_DATA_RATE DR_3 //modified - old - DR_0
/*!
 * LoRaWAN application port
 * @note do not use 224. It is reserved for certification
 */
#define LORAWAN_APP_PORT                            2
/*!
 * LoRaWAN default endNode class port
 */
#define LORAWAN_DEFAULT_CLASS                       CLASS_C
/*!
 * LoRaWAN default confirm state
 */
#define LORAWAN_DEFAULT_CONFIRM_MSG_STATE           LORAWAN_UNCONFIRMED_MSG
/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_BUFF_SIZE                           64
/*!
 * User application data
 */
//static uint8_t AppDataBuff[LORAWAN_APP_DATA_BUFF_SIZE];

/*!
 * User application data structure
 */
//static lora_AppData_t AppData={ AppDataBuff,  0 ,0 };
//lora_AppData_t AppData = { AppDataBuff,  0, 0 };

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* call back when LoRa endNode has received a frame*/
static void LORA_RxData(lora_AppData_t *AppData);

/* call back when LoRa endNode has just joined*/
static void LORA_HasJoined(void);

/* call back when LoRa endNode has just switch the class*/
static void LORA_ConfirmClass(DeviceClass_t Class);

/* call back when server needs endNode to send a frame*/
static void LORA_TxNeeded(void);

/* callback to get the battery level in % of full charge (254 full charge, 0 no charge)*/
static uint8_t LORA_GetBatteryLevel(void);

/* LoRa endNode send request*/
static void Send(void *context);

/* start the tx process*/
static void LoraStartTx(TxEventType_t EventType);

/* tx timer callback function*/
static void OnTxTimerEvent(void *context);

/* tx timer callback function*/
static void LoraMacProcessNotify(void);


/**** SD CArd Function prototypes *****/
static void mount_sd_card();
static void REMOVE_and_OPENAGAIN(const char* arq);
static void VERIFY_OPEN(const char* arq);
static void SAVE_ON_CARD();
static void REMOVE_FROM_CARD();


/* Private variables ---------------------------------------------------------*/
/* load Main call backs structure*/
static LoRaMainCallback_t LoRaMainCallbacks = { LORA_GetBatteryLevel,
                                                HW_GetTemperatureLevel,
                                                HW_GetUniqueId,
                                                HW_GetRandomSeed,
                                                LORA_RxData,
                                                LORA_HasJoined,
                                                LORA_ConfirmClass,
                                                LORA_TxNeeded,
                                                LoraMacProcessNotify
                                              };
LoraFlagStatus LoraMacProcessRequest = LORA_RESET;
LoraFlagStatus AppProcessRequest = LORA_RESET;
/*!
 * Specifies the state of the application LED
 */
static uint8_t AppLedStateOn = RESET;

static TimerEvent_t TxTimer;

/* !
 *Initialises the Lora Parameters
 */
static  LoRaParam_t LoRaParamInit = {LORAWAN_ADR_STATE,
                                     LORAWAN_DEFAULT_DATA_RATE,
                                     LORAWAN_PUBLIC_NETWORK
                                    };

/* Private functions ---------------------------------------------------------*/

/************************
 * SD Card Functions
 ************************/

int size_list(FIL File){
	char frase_f[10] = { 0 };
	int index = 0;
	while(f_gets(frase_f, bytesread, &File) != 0){
		index++;
	}
	return index;
}

static void mount_sd_card(){
	if(f_mount(&SDFatFS, (const TCHAR *)&SDPath, 1) != FR_OK)
	{
		// TODO Acionar flag ou alerta de ausencia de cartão ou erro de montagem
		PRINT_SD_CARD(PRINTF("Erro ao montar o cartao\r\n");)
		// Error_Handler();
	}
}

static void REMOVE_and_OPENAGAIN(const char* arq){

	res = f_unlink(arq);
	if(res == FR_LOCKED){
		f_close(&SDFile); 		// Fecha
		f_unlink(arq);			// Depois apaga
	}
	else if(res == FR_NO_FILE){
		return; 	// Não há nem arquivo existente com o nome informado
	}

	if(f_open(&SDFile, arq, FA_OPEN_APPEND | FA_READ | FA_WRITE) != FR_OK)
	{
		// TODO Imprimir os erros e tratar na uart
	  Error_Handler();
	}

	f_sync(&SDFile);
}

static void VERIFY_OPEN(const char* arq){
	res = f_open(&SDFile, arq, FA_OPEN_APPEND | FA_READ | FA_WRITE) != FR_OK;
	if(res == FR_OK){
		PRINT_SD_CARD(PRINTF("FR_OK \n");)
		return;
	}
	else if(res == FR_LOCKED){
		PRINT_SD_CARD(PRINTF("FR_LOCKED \n");)
		return;
	}
	else if(res == FR_DISK_ERR){
		PRINT_SD_CARD(PRINTF("FR_DISK_ERR \n");)
		Error_Handler();
	}
	else{
		PRINT_SD_CARD(PRINTF("Error to open the log file on the SD Card \n Reset the board \n");)
		Error_Handler();
	}
}

static void SAVE_ON_CARD(){
	delayed_store_flag++; 	// Contagem de TAGs atrasadas ao envio

	// Se não há conexão entre o gateway, armazena no cartão SD para envio posterior
	PRINT_SD_CARD(PRINTF("===> Escrita no cartao. Count = %d\r\n", delayed_store_flag);)
	f_write(&SDFile, store_TAG[last_TAG].N_TAG, sizeof(store_TAG[last_TAG].N_TAG), (void *)&byteswritten);
//	f_sync(&SDFile);	// Um ou outro
	f_close(&SDFile);
}

static void REMOVE_FROM_CARD(){
	// Remove do cartão SD e armazena estrutura para envio da Lora
	//TODO Generalizar a função colocando um argumento para receber o dado que estava no cartão

	f_gets(buffer_tag, bytesread, &SDFile);
	memcpy(tag_to_lora.N_TAG, buffer_tag, sizeof(buffer_tag));
	delayed_store_flag--;
	PRINT_SD_CARD(PRINTF("===> Removida do cartão. Count = %d\r\n", delayed_store_flag);)
}

/************* End of Sd card functions *****************/


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  flags_ble.all_flags=RESET;
  /* STM32 HAL library initialization*/
  HAL_Init();

  /* Configure the system clock*/
  SystemClock_Config();

  /* Configure the debug mode*/
  //DBG_Init();

  /* Configure the hardware*/
  HW_Init();

  // Mount and prepare SD Card
  mount_sd_card();

  /*Disbale Stand-by mode*/
  LPM_SetOffMode(LPM_APPLI_Id, LPM_Disable);

  PRINTF("APP_VERSION= %02X.%02X.%02X.%02X\r\n", (uint8_t)(__APP_VERSION >> 24), (uint8_t)(__APP_VERSION >> 16), (uint8_t)(__APP_VERSION >> 8), (uint8_t)__APP_VERSION);
  PRINTF("MAC_VERSION= %02X.%02X.%02X.%02X\r\n", (uint8_t)(__LORA_MAC_VERSION >> 24), (uint8_t)(__LORA_MAC_VERSION >> 16), (uint8_t)(__LORA_MAC_VERSION >> 8), (uint8_t)__LORA_MAC_VERSION);

  /* Configure the Lora Stack*/
  //LORA_Init(&LoRaMainCallbacks, &LoRaParamInit);

  //LORA_Join();

  //LoraStartTx(TX_ON_TIMER);
  //uint32_t prim; //tratar depois as interrupcoes
  in_use_TAG = EMPTY_QUEUE;
  while (1)
  {

	if (flags_ble.enable_handler){
		flags_ble.enable_handler = 0;
		ble_handler((uint8_t*)&message_ble);					// Aciona o handler para selecionar a mensagem de resposta.
	}

	if (flags_ble.update_mode){

		//prim = __get_PRIMASK();
		//__disable_irq();
		flags_ble.update_mode = RESET;
		HAL_NVIC_DisableIRQ(USART1_IRQn);
		HAL_UART_AbortReceive_IT(&huart1);
		HAL_UART_DeInit(&huart1);
		HAL_Delay(1);
		COM_Init();
		HAL_Delay(1);
		COM_Flush();
		//HAL_Delay(1);
		FW_UPDATE_Run();
		MX_USART1_UART_Init();
		HAL_UART_Receive_IT(&huart1, rx_byte_uart1, 1);
	}

	if ((flags_ble.start == SET) && (flags_ble.connection == SET))
	{

		//Envia Comando de leitura de brinco
		if(flags_ble.rfid_send_cmd == SET){
			flags_ble.rfid_send_cmd = RESET;
			HAL_UART_Transmit(&huart2, (uint8_t *)READ_MULTIPLE_TAG, MSG_MULTI_TAG_SIZE, 50);
		}


		if(flags_ble.tag == SET)
		{
			if(bytes_read_rfid>4)
				b  = message_handler((uint8_t*)&message, bytes_read_rfid);
			flags_ble.tag = RESET;
		}

		if (last_TAG >= 0)
		{
			// Variavel auxiliar para fazer envios sequenciais das TAGs sem mexer no indice original
			PRINTF("====> indices: IN: %d LS: %d\r\n", in_use_TAG, last_TAG);

			if(in_use_TAG<0)
				in_use_TAG=0;

			//	Tratamento de indice
			if (last_TAG == 0)
			{
				in_use_TAG = 0;
			}

			if (in_use_TAG>last_TAG)
			{
				in_use_TAG = last_TAG;
			}

			// 	Envio ao app via bluetooth
			if(in_use_TAG>=0)
			{

//				PRINTF("%d Brinco: ", in_use_TAG);
//				for (uint8_t i = 0; i <= TAG_SIZE-1;i++)
//				{
//					PRINTF("%X ", (store_TAG[in_use_TAG].N_TAG[i]));
//				}
//				PRINTF("\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t*) store_TAG[in_use_TAG].N_TAG, TAG_SIZE-1, 1000);
				HAL_Delay(TIMEOUT_BETWEEN_RESEND_TAG);


				if ((in_use_TAG<last_TAG) && (flags_ble.confirm == SET))
				{
					flags_ble.confirm = RESET;
					PRINTF("NewTag = %d \n\r", pack_position);
					count_send=0;
					flag_send_timeout = RESET;
					flag_send_to_lora++;
					if(pack_position >= 10)
						pack_position=0;
					memcpy(pack_to_lora[pack_position++].N_TAG, store_TAG[in_use_TAG].N_TAG, TAG_SIZE);
					in_use_TAG++;
				}
				else if (in_use_TAG>=last_TAG)
				{
					PRINTF("FilaVazia \n\r");
					clear_buffers();
				}

			}


//			PRINTF("Flag_send_to_lora = %d", flag_send_to_lora);
//			flag_confirm = RESET;
			//Send(NULL);
		}
//		else
//		{
//			last_TAG = EMPTY_QUEUE;// Restaura o identificador ao ponto padrão sem armazenamento
//			clear_buffers();
//			// TODO Procurar um método mais fácil para limpar o armazenamento das TAGs
//		}


	}


#define form1
	if (flag_send_timeout == SET)
	{
		flag_send_timeout = RESET;
		if (AppProcessRequest == LORA_SET)
		{
			AppProcessRequest = LORA_RESET;
			if(flag_send_to_lora > 0 ){
				flag_send_to_lora--;
			//	Send(NULL);
				PRINTF("\n Envio pelo tempo configurado \n");
			}
		}
	}

    if (LoraMacProcessRequest == LORA_SET)
    {
      LoraMacProcessRequest = LORA_RESET;
      LoRaMacProcess();
    }
    //If a flag is set at this point, mcu must not enter low power and must loop
    //DISABLE_IRQ();

     /*if an interrupt has occurred after DISABLE_IRQ, it is kept pending
     * and cortex will not enter low power anyway
     * */
    if ((LoraMacProcessRequest != LORA_SET) && (AppProcessRequest != LORA_SET))
    {
#ifndef LOW_POWER_DISABLE
      LPM_EnterLowPower();
#endif
    }

    //ENABLE_IRQ();

  }
}

void LoraMacProcessNotify(void)
{
  LoraMacProcessRequest = LORA_SET;
}


static void LORA_HasJoined(void)
{
  LORA_RequestClass(LORAWAN_DEFAULT_CLASS);
}

#ifdef form1
static void Send(void *context)
{
	if (LORA_JoinStatus() != LORA_SET)
	{
		/*Not joined, try again later*/
		LORA_Join();

		flag_lora_joined = RESET;
		SAVE_ON_CARD();
		return;
	}

	if (delayed_store_flag > 0){
		delayed_store_flag--;
		REMOVE_FROM_CARD();
	}

	flag_lora_joined = SET;

	TVL1(PRINTF("SEND REQUEST\n\r");) //Aqui

	AppData.Port = LORAWAN_APP_PORT;

	//muda_buffer(&AppData, Buffer_to_send); //JP
	AppData.BuffSize = sizeof(Model_TAG);
	//memcpy(AppData.Buff,&tag_to_lora,sizeof(Model_TAG));

	if(pack_position > 0){
		memcpy(AppData.Buff, &pack_to_lora[--pack_position], sizeof(Model_TAG));
	}
	else{
		char none_msg[] = "1111111111";
		memcpy(AppData.Buff, &none_msg, sizeof(none_msg));
	}
	PRINTF("\n Pack_position = %d \n\r", pack_position);

	HAL_TIM_Base_Stop_IT(&htim2);
	LORA_send(&AppData, LORAWAN_DEFAULT_CONFIRM_MSG_STATE);
	HAL_TIM_Base_Start_IT(&htim2);

}
#endif //form1

#ifdef form2
static void Send(void *context)
{
	if (LORA_JoinStatus() != LORA_SET)
	{
		/*Not joined, try again later*/
		LORA_Join();

		flag_lora_joined = RESET;
		SAVE_ON_CARD();
		return;
	}

	if (delayed_store_flag > 0){
		delayed_store_flag--;
		REMOVE_FROM_CARD();
	}

	flag_lora_joined = SET;

	PRINTF("SEND REQUEST\n\r"); //Aqui

	AppData.Port = LORAWAN_APP_PORT;

	//muda_buffer(&AppData, Buffer_to_send); //JP
	AppData.BuffSize = sizeof(Model_TAG);
	memcpy(AppData.Buff,&tag_to_lora,sizeof(Model_TAG));

	HAL_TIM_Base_Stop_IT(&htim2);
	LORA_send(&AppData, LORAWAN_DEFAULT_CONFIRM_MSG_STATE);
	HAL_TIM_Base_Start_IT(&htim2);

}
#endif //form2


static void LORA_RxData(lora_AppData_t *AppData)
{

  PRINTF("PACKET RECEIVED ON PORT %d\n\r", AppData->Port);

  switch (AppData->Port)
  {
    case 3:
      /*this port switches the class*/
      if (AppData->BuffSize == 1)
      {
        switch (AppData->Buff[0])
        {
          case 0:
          {
            LORA_RequestClass(CLASS_A);
            break;
          }
          case 1:
          {
            LORA_RequestClass(CLASS_B);
            break;
          }
          case 2:
          {
            LORA_RequestClass(CLASS_C);
            break;
          }
          default:
            break;
        }
      }
      break;
    case LORAWAN_APP_PORT:
      if (AppData->BuffSize == 1)
      {
        AppLedStateOn = AppData->Buff[0] & 0x01;
        if (AppLedStateOn == RESET)
        {
          PRINTF("LED OFF\n\r");
          LED_Off(LED_BLUE) ;
        }
        else
        {
          PRINTF("LED ON\n\r");
          LED_On(LED_BLUE) ;
        }
      }
      break;
//    case LPP_APP_PORT:
//    {
//      AppLedStateOn = (AppData->Buff[2] == 100) ?  0x01 : 0x00;
//      if (AppLedStateOn == RESET)
//      {
//        PRINTF("LED OFF\n\r");
//        LED_Off(LED_BLUE) ;
//
//      }
//      else
//      {
//        PRINTF("LED ON\n\r");
//        LED_On(LED_BLUE) ;
//      }
//      break;
//    }
    default:
      break;
  }
}

static void OnTxTimerEvent(void *context)
{
  /*Wait for next tx slot*/
  TimerStart(&TxTimer);

  AppProcessRequest = LORA_SET;
}

static void LoraStartTx(TxEventType_t EventType)
{
  if (EventType == TX_ON_TIMER)
  {
    /* send everytime timer elapses */
    TimerInit(&TxTimer, OnTxTimerEvent);
    TimerSetValue(&TxTimer,  APP_TX_DUTYCYCLE);
    OnTxTimerEvent(NULL);
  }

}

static void LORA_ConfirmClass(DeviceClass_t Class)
{
  PRINTF("switch to class %c done\n\r", "ABC"[Class]);

  /*Optionnal*/
  /*informs the server that switch has occurred ASAP*/
  AppData.BuffSize = 0;
  AppData.Port = LORAWAN_APP_PORT;

  LORA_send(&AppData, LORAWAN_UNCONFIRMED_MSG);
}

static void LORA_TxNeeded(void)
{
  AppData.BuffSize = 0;
  AppData.Port = LORAWAN_APP_PORT;

  LORA_send(&AppData, LORAWAN_UNCONFIRMED_MSG);
}

/**
  * @brief This function return the battery level
  * @param none
  * @retval the battery level  1 (very low) to 254 (fully charged)
  */
uint8_t LORA_GetBatteryLevel(void)
{
  uint16_t batteryLevelmV;
  uint8_t batteryLevel = 0;

  batteryLevelmV = HW_GetBatteryLevel();


  /* Convert batterey level from mV to linea scale: 1 (very low) to 254 (fully charged) */
  if (batteryLevelmV > VDD_BAT)
  {
    batteryLevel = LORAWAN_MAX_BAT;
  }
  else if (batteryLevelmV < VDD_MIN)
  {
    batteryLevel = 0;
  }
  else
  {
    batteryLevel = (((uint32_t)(batteryLevelmV - VDD_MIN) * LORAWAN_MAX_BAT) / (VDD_BAT - VDD_MIN));
  }

  return batteryLevel;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

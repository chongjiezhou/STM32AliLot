/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "lwip.h"
#include "api.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "lcd_log.h"
#include "app_ethernet.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myTaskTcpHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void BSP_Config(void);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTaskTcp(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 4096);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTaskTcp */
  osThreadDef(myTaskTcp, StartTaskTcp, osPriorityIdle, 0, 1024);
  myTaskTcpHandle = osThreadCreate(osThread(myTaskTcp), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	BSP_Config();
	User_notification(&gnetif);

#ifdef USE_DHCP
	osThreadDef(DHCP,DHCP_thread,osPriorityBelowNormal,0,256);
	osThreadCreate(osThread(DHCP), &gnetif);
#endif
	osThreadTerminate(NULL);
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskTcp */
/**
 * @brief Function implementing the myTaskTcp thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTaskTcp */
void StartTaskTcp(void const * argument)
{
  /* USER CODE BEGIN StartTaskTcp */
	struct netconn *conn;
	int ret;
	ip4_addr_t ipaddr;
	ip4_addr_t ipaddr2;
	uint8_t DEST_IP_ADDR0 = 192;
	uint8_t DEST_IP_ADDR1 = 168;
	uint8_t DEST_IP_ADDR2 = 1;
	uint8_t DEST_IP_ADDR3 = 6;
	uint16_t DEST_PORT = 8080;
	uint16_t LOCAL_PORT;

	uint8_t send_buf[] = "This is a TCP Client test...\r\n";

	while(DHCP_state != DHCP_ADDRESS_ASSIGNED);

	printf("DEST IP:%d.%d.%d.%d \t PORT:%d \n\n", DEST_IP_ADDR0, DEST_IP_ADDR1,
			DEST_IP_ADDR2, DEST_IP_ADDR3, DEST_PORT);

	while (1)
	{
		conn = netconn_new(NETCONN_TCP);
		if (conn == NULL)
		{
			printf("creat conn failed!\n");
			osDelay(10);
			continue;
		}
		IP4_ADDR(&ipaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2,
				DEST_IP_ADDR3);
		ret = netconn_connect(conn, &ipaddr, DEST_PORT);

		if (ret == -1)
		{
			printf("Connect failed!\n");
			netconn_close(conn);
			continue;
		}
		printf("Connect successful!\n");
		netconn_getaddr(conn, &ipaddr2, &LOCAL_PORT, 1);
		while (1)
		{
			ret = netconn_write(conn, send_buf, sizeof(send_buf), 0);
			osDelay(1000);
		}
	}
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartTaskTcp */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
  * @brief  Initializes the STM327546G-Discovery's LCD  resources.
  * @param  None
  * @retval None
  */
static void BSP_Config(void)
{
  /* Initialize the LCD */
  BSP_LCD_Init();

  /* Initialize the LCD Layers */
  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

  /* Set LCD Foreground Layer  */
  BSP_LCD_SelectLayer(1);

  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

  /* Initialize LCD Log module */
  LCD_LOG_Init();

  /* Show Header and Footer texts */
  LCD_LOG_SetHeader((uint8_t *)"Webserver Application Netconn API");
  LCD_LOG_SetFooter((uint8_t *)"STM32746G-DISCO board");

  LCD_UsrLog ((char *)"  State: Ethernet Initialization ...\n");
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

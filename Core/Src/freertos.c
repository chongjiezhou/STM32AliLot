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
osThreadId TaskListHandle;
osThreadId TaskTcpEchoHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void tcp_echoserver_init(void);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTaskList(void const * argument);
void StartTaskTcpEcho(void const * argument);

extern void MX_FATFS_Init(void);
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

  /* definition and creation of TaskList */
  osThreadDef(TaskList, StartTaskList, osPriorityNormal, 0, 1024);
  TaskListHandle = osThreadCreate(osThread(TaskList), NULL);

  /* definition and creation of TaskTcpEcho */
  osThreadDef(TaskTcpEcho, StartTaskTcpEcho, osPriorityLow, 0, 1024);
  TaskTcpEchoHandle = osThreadCreate(osThread(TaskTcpEcho), NULL);

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

	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskList */
/**
 * @brief Function implementing the TaskList thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTaskList */
void StartTaskList(void const * argument)
{
  /* USER CODE BEGIN StartTaskList */
	while(1)
	{
		printf("Usart Test\r\n");
		osDelay(500);
	}
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartTaskList */
}

/* USER CODE BEGIN Header_StartTaskTcpEcho */
/**
 * @brief Function implementing the TaskTcpEcho thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTaskTcpEcho */
void StartTaskTcpEcho(void const * argument)
{
  /* USER CODE BEGIN StartTaskTcpEcho */
//	struct netconn *conn, *newconn;
//	err_t err, accept_err;
//	struct netbuf *buf;
//	void *data;
//	u16_t len;
//	err_t recv_err;
//
//	LWIP_UNUSED_ARG(argument);
//
//	/* Create a new connection identifier. */
//	conn = netconn_new(NETCONN_TCP);
//
//	if (conn != NULL)
//	{
//		/* Bind connection to well known port number 7. */
//		err = netconn_bind(conn, NULL, 7);
//
//		if (err == ERR_OK)
//		{
//			/* Tell connection to go into listening mode. */
//			netconn_listen(conn);
//
//			while (1)
//			{
//				/* Grab new connection. */
//				accept_err = netconn_accept(conn, &newconn);
//
//				/* Process the new connection. */
//				if (accept_err == ERR_OK)
//				{
//					recv_err = netconn_recv(newconn, &buf);
//					while (recv_err == ERR_OK)
//					{
//						do
//						{
//							netbuf_data(buf, &data, &len);
//							netconn_write(newconn, data, len, NETCONN_COPY);
//						} while (netbuf_next(buf) >= 0);
//
//						netbuf_delete(buf);
//						recv_err = netconn_recv(newconn, &buf);
//					}
//
//					/* Close connection and discard connection identifier. */
//					netconn_close(newconn);
//					netconn_delete(newconn);
//				}
//			}
//		}
//		else
//		{
//			netconn_delete(newconn);
//			printf(" can not bind TCP netconn");
//		}
//	}
//	else
//	{
//		printf("can not create TCP netconn");
//	}
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartTaskTcpEcho */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static struct tcp_pcb *tcp_echoserver_pcb;
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb,
		struct pbuf *p, err_t err);
void tcp_echoserver_init(void)
{
	err_t err;

	tcp_echoserver_pcb = tcp_new();

	if (tcp_echoserver_pcb != NULL)
	{
		err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, 7);

		if (err == ERR_OK)
		{
			tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);
			tcp_accept(tcp_echoserver_pcb, tcp_echoserver_accept);
		}

		else
		{
			memp_free(MEMP_TCP_PCB, tcp_echoserver_pcb);
		}
	}
}
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	tcp_recv(newpcb, tcp_echoserver_recv);

	return ERR_OK;
}

static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb,
		struct pbuf *p, err_t err)
{
	tcp_write(tpcb, p->payload, p->len, 1);
	pbuf_free(p);

	return ERR_OK;
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

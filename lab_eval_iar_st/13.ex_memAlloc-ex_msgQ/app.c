/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : app.c
* Version       : V1.10
* Programmer(s) : BAN
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512

static  OS_STK         App_TaskStartStk[APP_TASK_START_STK_SIZE];


static	OS_STK TaskPrio3Stk[1000];
static	OS_STK TaskPrio5Stk[1000];
static	OS_STK TaskPrio7Stk[1000];



/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskStart        (void        *p_arg);

void TaskPrio3 (void *pdata);
void TaskPrio5 (void *pdata);
void TaskPrio7 (void *pdata);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

#ifndef SOO

#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"

#define TXE     7
#define RXNE    5

void PutChar(char c)
{
  while((USART1->SR & (1<<TXE)) == 0);
  USART1->DR = c;
  if(c == '\n') PutChar('\r');
}

char GetChar(void)
{
  char data;
  
  while(!(USART1->SR & (1<<RXNE)));
  data = USART1->DR;
  if(data == '\r') PutChar('\n');   //echo
  else PutChar(data);   //echo
  
  return data;
}

void Puts(char *str)
{
  while(*str) {
    PutChar(*str);
    str++;
  }
}

void Gets(char *buff)
{
  char data;
  
  while(1) {
    data = GetChar();
    if(data == '\r' || data == '\n') break;
    *buff = data;
    buff++;
  }
  *buff = '\0';
}


void USART1_Init(void)
{
    USART_InitTypeDef USART_InitStructure;


    
    /* USARTx configuration ------------------------------------------------------*/
    /* USARTx configured as follow:
     - BaudRate = 115200 baud  
     - Word Length = 8 Bits
     - One Stop Bit
     - No parity
     - Hardware flow control disabled (RTS and CTS signals)
     - Receive and transmit enabled
     */
    USART_InitStructure.USART_BaudRate   = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl
                                         = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure the USARTx */ 
    USART_Init(USART1, &USART_InitStructure);

    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
	/* Enable GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /* Enable USART1 clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    

    // UART configuration ...

    /* Configure USARTx_Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USARTx_Rx as input floating */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
   
    //USART_SendData() 사용 시 필요함
    //GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    //GPIO_Init(GPIOB, &GPIO_InitStructure);
}

#endif


....... *msgQ;
.............
//INT32U TxData1=0;
//INT32U TxData2=0;

INT32U DataBuf[10];
....... *MemPart1;

void my_delay(int loop)
{
	int i, j;
	
	for(i=0; i<loop; i++)
	{
		for(j=0; j<10000; j++)
		{
			j = j;
		}
	}
}


int  main (void)
{
    CPU_INT08U  os_err;

	//added by SOO
    GPIO_Configuration();
    USART1_Init(); 

    Puts("\nmain start!\n");
    
    BSP_IntDisAll();   	/* Disable all ints until we are ready to accept them.  */
	Puts("\nBSP_IntDisAll() done!\n");
	
    OSInit();  			/* Initialize "uC/OS-II, The Real-Time Kernel".         */
	Puts("\nOSInit() done!\n");
	
#if 0
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskStart,  /* Create the start task.                               */
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO,
                             (INT16U          ) APP_TASK_START_PRIO,
                             (OS_STK        * )&App_TaskStartStk[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#endif
	os_err = OSTaskCreate((void (*)(void *)) App_TaskStart,  /* Create the start task.                               */
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                             //(INT8U           ) APP_TASK_START_PRIO
                             (INT8U           ) 2
                             );
#if (OS_TASK_NAME_SIZE >= 11)
    OSTaskNameSet(APP_TASK_START_PRIO, (CPU_INT08U *)"Start Task", &os_err);
#endif

    OSStart();
    Puts("\nOSStart() done!\n");                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */

    return (0);
}



/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
	char buff[128];
	INT8U err;
	
    (void)p_arg;

    //CPU_INT08U  os_err;
	
	Puts("\nApp_TaskStart() start!\n");
	
    BSP_Init(); 		/* Initialize BSP functions.                            */
    
    //added by SOO
    GPIO_Configuration();
    USART1_Init(); 

    //Puts("\nBSP_Init() done!\n");
    sprintf(buff,"\nBSP_Init() done_%d!\n", 1);
    Puts(buff);
        
                                                
    OS_CPU_SysTickInit();            /* Initialize the SysTick.                              */
	
    Puts("\nOS_CPU_SysTickInit() done!\n");       


#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                    /* Determine CPU capacity.  
    Puts("\nOSStatInit() done!\n");                              */
#endif


    BSP_LED_Off(0);
    //OSTimeDlyHMSM(0, 0, 0, 1000);


	msgQ = ..............
	
	MemPart1 = ..............
	
	OSTaskCreate(TaskPrio7, (void *)0,&TaskPrio7Stk[999], 7);
	OSTaskCreate(TaskPrio5, (void *)0,&TaskPrio5Stk[999], 5);
	OSTaskCreate(TaskPrio3, (void *)0,&TaskPrio3Stk[999], 3);
	
    while (1) { /* Task body, always written as an infinite loop. */
        sprintf(buff,"\nRunning Task : %5d\n", OSTaskCtr);     /* Display #tasks running                  */
		Puts(buff);
		sprintf(buff,"Cpu Usage : %3d\n", OSCPUUsage);       /* Display CPU usage in %                  */
		Puts(buff);	
												
		/* Display #context switches per second    */
		sprintf(buff,"Context Switches per Sec : %5d\n",(int)OSCtxSwCtr);  
		Puts(buff);
		OSCtxSwCtr = 0;
		OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                         */
	}

}


/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/


void TaskPrio3 (void *pdata)
{
	char buff[128];
	INT8U err;
	INT32U *RxData;
	
	pdata = pdata;
	while(1) {
		Puts("Task3_Start...!!\n");
		RxData = OSQPend(msgQ, 0, &err);
		sprintf(buff,"Task3:msgQ_Rx(data:%d)...!!\n", *RxData);
		Puts(buff);
		...............
		//OSTimeDlyHMSM(0, 0, 1, 0);
		//Puts("Task3_End...!!\n");
	}
}

void TaskPrio5 (void *pdata)
{
	char buff[128];
	INT32U *TxData1;
	INT32U cnt=0;
	INT8U err;
	
	pdata = pdata;	
	while(1) {
		Puts("Task5_Start...!!\n");
		TxData1 = ...................		
		*TxData1 = cnt++;
		sprintf(buff,"Task5:msgQ_Tx(data:%d)...!!\n", *TxData1);
		Puts(buff);
		..............	
		OSTimeDlyHMSM(0, 0, 0, 800);
		//Puts("Task5_End...!!\n");
	}
}

void TaskPrio7 (void *pdata)
{
	char buff[128];
	INT32U *TxData2;
	INT32U cnt=500;
	INT8U err;
	
	pdata = pdata;	
	while(1) {	
		Puts("Task7_Start...!!\n");	
		TxData2 = .............
		*TxData2 = cnt++;
		sprintf(buff,"Task7:msgQ_Tx(data:%d)...!!\n", *TxData2);
		Puts(buff);
		................
		OSTimeDlyHMSM(0, 0, 0, 500);
		//Puts("Task7_End...!!\n");
	}
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if ((APP_OS_PROBE_EN   == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN == DEF_ENABLED))
    //OSProbe_TaskCreateHook(ptcb);
#endif
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if ((APP_OS_PROBE_EN   == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN == DEF_ENABLED))
    //OSProbe_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                        TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if ((APP_OS_PROBE_EN   == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN == DEF_ENABLED))
    //OSProbe_TickHook();
#endif
}
#endif
#endif


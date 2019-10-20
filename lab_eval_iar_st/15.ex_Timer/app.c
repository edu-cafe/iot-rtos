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

static  OS_STK         Task1_Stk[TASK_STK_SIZE];
static  OS_STK         Task2_Stk[TASK_STK_SIZE];



/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskStart        (void        *p_arg);

void   Task1(void *data); 
void   Task2(void *data);

void func_led(void *ptmr, void* parg);

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


....... *TmrLed1, *TmrLed2, *TmrLed3;

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
                             (INT8U           ) 5
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
	
    (void)p_arg;

    INT8U  err;
	
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


	//1Sec Timer Set (Timer Tick : 100ms)
	//TmrLed1 = OSTmrCreate(0, 10, OS_TMR_OPT_ONE_SHOT, 
	TmrLed1 = OSTmrCreate(0, 10, OS_TMR_OPT_PERIODIC, 
					func_led, (void*)1, "led1_timer", &err);

	//2Sec Timer Set
	TmrLed2 = .........................
					
	//3Sec Timer Set
	TmrLed3 = ........................ 

                    
    err = OSTaskCreate(Task1,"Task1",&Task1_Stk[TASK_STK_SIZE - 1],8);
	sprintf(buff,"\nTask1 Create(err:%d)\n", err);
	Puts(buff);
	err = OSTaskCreate(Task2,"Task2",&Task2_Stk[TASK_STK_SIZE - 1],9);
	sprintf(buff,"\nTask2 Create(err:%d)\n", err);
	Puts(buff);
	
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


void func_led(void *ptmr, void* parg)
{
	char buff[128];
	
	sprintf(buff,"\n-------->func_led(parg:%d)..\n", (int)parg);
	Puts(buff);
	//BSP_LED_On(1);
	switch((int)parg) {
		case 1:	BSP_LED_Toggle(1);	break;
		case 2:	BSP_LED_Toggle(2);	break;
		case 3:	BSP_LED_Toggle(3);	break;
	}
}

void Task1(void *data)
{
	char buff[128];
	INT8U err;
	
	Puts("\nTask1 Start!!\n");
	..................	
	sprintf(buff,"TmerLed1 Start(err:%d)!\n", err);
	Puts(buff);
	..................	
	sprintf(buff,"TmerLed3 Start(err:%d)!\n", err); 
	Puts(buff);
	for (;;) {
		Puts((char *)data);
        OSTimeDly(5);                          
	}
}

void Task2(void *data)
{
	char buff[128];
	INT8U err;
	
	Puts("\nTask2 Start!!\n");
	................	
	sprintf(buff,"TmerLed2 Start(err:%d)!\n", err); 
	Puts(buff);
   	for (;;) {
		Puts((char *)data);
		OSTimeDly(2);     
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


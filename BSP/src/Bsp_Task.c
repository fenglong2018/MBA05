/*************************************************************************
*  	文件名称:  	key.c
*  	文件描述:  	按键读取
*  	历史记录:
*  	创建时间:  	2024-08-05
*  	创建人:  	何凤龙
*  	修改时间:
*	修改人:
*   描述:
*************************************************************************/

/*************************************************************************
头文件引用声明
*************************************************************************/
//#include 	"FreeRTOS.h"
//#include 	"task.h"
#include 	"Bsp_task.h"


/************************************************************************
宏定义
*************************************************************************/

/*************************************************************************
内部结构、枚举、公用体等结构声明
*************************************************************************/

/*************************************************************************
内部静态变量定义

全局变量定义

extern 外部变量声明

*************************************************************************/


/*************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/*************************************************************************
函数定义
*************************************************************************/

/**********************************************************************
*  函数名称:  getkey_ir_p
*  功能描述:  IR+按键读取
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    0代表按下，1代表松开
***********************************************************************/
void Bsp_Task(void)
{
#ifdef USEFREERTOS	
    /* Enter critical section */
    taskENTER_CRITICAL();	
	// 设置 ADC 任务
	setupADCTask();
	setupBatTask();
	setupMagKeyTask();
	setupPortInforGetTask();
	setupNorSOSTask();
	setupPullSendCtrlTask();
	setupBatVolDisplaytask();
	setupSysStateCtrltask();
	setupBspVbatChagCtrltask();
	setupErrorLedDisplaytask();
	// 启动调度器
	vTaskStartScheduler();
    /* Exit critical section */
    taskEXIT_CRITICAL();	
#else
	// 执行其他程序
#endif	
}
/**********************************************************************
*  函数名称:  setupADCTask
*  功能描述:  ADC任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupADCTask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )adc_task_function,       // 任务函数
                    (const char*    )"adc_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&adc_task) != pdPASS)    // 任务句柄
    {
        printf("ADConvert task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("ADConvert task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  adc_task_function
*  功能描述:  ADC执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void adc_task_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        ADConvert();
        vTaskDelay(pdMS_TO_TICKS(10)); // 10毫秒延迟
    }
}
/**********************************************************************
*  函数名称:  setupBatTask
*  功能描述:  电池电量更新任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupBatTask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )Bat_Percent_function,       // 任务函数
                    (const char*    )"adc_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&Bat_Percent) != pdPASS)    // 任务句柄
    {
        printf("Bat_Percent_function task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("Bat_Percent_function task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  Bat_Percent_function
*  功能描述:  电池电量更新执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void Bat_Percent_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        Bsp_Updata_Bat_Percent();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1秒延迟
    }
}
/**********************************************************************
*  函数名称:  setupBatTask
*  功能描述:  磁吸按键任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupMagKeyTask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )MagKey_Scan_function,       // 任务函数
                    (const char*    )"MagKey_Scan_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&MagKey_Scan) != pdPASS)    // 任务句柄
    {
        printf("MagKey_Scan_function task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("MagKey_Scan_function task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  MagKey_Scan_function
*  功能描述:  磁吸按键执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void MagKey_Scan_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        Bsp_MagKey_Scan();
        vTaskDelay(pdMS_TO_TICKS(10)); // 10毫秒延迟
    }
}
/**********************************************************************
*  函数名称:  setupPortInforGetTask
*  功能描述:  接口读取任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupPortInforGetTask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )Port_InforGet_function,       // 任务函数
                    (const char*    )"Port_InforGet_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&Port_InforGet) != pdPASS)    // 任务句柄
    {
        printf("Port_InforGet_function task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("Port_InforGet_function task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  Port_InforGet_function
*  功能描述:  接口读取执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void Port_InforGet_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        Bsp_Port_InforGet_Ctrl();
        vTaskDelay(pdMS_TO_TICKS(100)); // 100毫秒延迟
    }
}
/**********************************************************************
*  函数名称:  setupPortInforGetTask
*  功能描述:  接口读取任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupNorSOSTask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )Nor_SOS_function,       // 任务函数
                    (const char*    )"Nor_SOS_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&Nor_SOS) != pdPASS)    // 任务句柄
    {
        printf("Nor_SOS_function task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("Nor_SOS_function task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  Nor_SOS_function
*  功能描述:  接口读取执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void Nor_SOS_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        Bsp_Nor_SOS_SendCtrl();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 100毫秒延迟
    }
}		
/**********************************************************************
*  函数名称:  setupPortInforGetTask
*  功能描述:  接口读取任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupPullSendCtrlTask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )Pull_SendCtrl_function,       // 任务函数
                    (const char*    )"PullSendCtrl_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&Pull_SendCtrl) != pdPASS)    // 任务句柄
    {
        printf("Pull_SendCtrl_function task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("Pull_SendCtrl_function task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  Nor_SOS_function
*  功能描述:  接口读取执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void Pull_SendCtrl_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        Bsp_Pull_SendCtrl();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 100毫秒延迟
    }
}		
/**********************************************************************
*  函数名称:  setupPortInforGetTask
*  功能描述:  接口读取任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupBatVolDisplaytask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )Bat_Vol_Display_function,       // 任务函数
                    (const char*    )"BatVolDisplay_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&Bat_Vol_Display) != pdPASS)    // 任务句柄
    {
        printf("Pull_SendCtrl_function task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("Pull_SendCtrl_function task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  Nor_SOS_function
*  功能描述:  执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void Bat_Vol_Display_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        Bsp_Bat_Vol_Display();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 100毫秒延迟
    }
}
/**********************************************************************
*  函数名称:  setupSysStateCtrltask
*  功能描述:  任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupSysStateCtrltask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )Sys_State_Ctrl_function,       // 任务函数
                    (const char*    )"SysStateCtrl_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&Sys_State_Ctrl) != pdPASS)    // 任务句柄
    {
        printf("Pull_SendCtrl_function task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("Pull_SendCtrl_function task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  Sys_State_Ctrl_function
*  功能描述:  执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void Sys_State_Ctrl_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        Bsp_Sys_State_Ctrl();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 100毫秒延迟
    }
}
/**********************************************************************
*  函数名称:  setupSysStateCtrltask
*  功能描述:  任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupBspVbatChagCtrltask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )Vbat_Chag_Ctrl_function,       // 任务函数
                    (const char*    )"VbatChagCtrl_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&Vbat_Chag_Ctrl) != pdPASS)    // 任务句柄
    {
        printf("Pull_SendCtrl_function task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("Pull_SendCtrl_function task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  Sys_State_Ctrl_function
*  功能描述:  执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void Vbat_Chag_Ctrl_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        Bsp_Vbat_Chag_Ctrl();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 100毫秒延迟
    }
}
/**********************************************************************
*  函数名称:  setupSysStateCtrltask
*  功能描述:  任务创建
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void setupErrorLedDisplaytask(void)
{
    /* 创建 ADC 任务 */
    if (xTaskCreate((TaskFunction_t )Error_Led_Display_function,       // 任务函数
                    (const char*    )"ErrorLedDisplay_task",              // 任务名称
                    (uint16_t       )configMINIMAL_STACK_SIZE,// 任务堆栈大小
                    (void*          )NULL,                    // 传递给任务的参数 (NULL)
                    (UBaseType_t    )tskIDLE_PRIORITY + 1,    // 任务优先级
                    (TaskHandle_t*  )&Error_Led_Display) != pdPASS)    // 任务句柄
    {
        printf("Pull_SendCtrl_function task could not be created as there was insufficient heap memory remaining.\r\n");
    }
    else
    {
        printf("Pull_SendCtrl_function task was created successfully.\r\n");
    }
}
/**********************************************************************
*  函数名称:  Sys_State_Ctrl_function
*  功能描述:  执行程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void Error_Led_Display_function(void *pvParameters)
{
    (void)pvParameters;  // 忽略未使用的参数

    for (;;)
    {
        Bsp_Error_Led_Display();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 100毫秒延迟
    }
}



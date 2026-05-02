#include "main.h"
#include "schedul.h"
/* multi task test */
void SystemClock_Config(void);

  void task_1_func( void ){
      f1:HAL_GPIO_WritePin( GPIOC, GPIO_PIN_14, 0);
      HAL_Delay( 1000 );

      HAL_GPIO_WritePin( GPIOC, GPIO_PIN_14, 1);
      HAL_Delay( 1000 );
      goto f1;
  }
  void task_2_func( void ){
    f1:HAL_GPIO_WritePin( GPIOC, GPIO_PIN_13, 0);
    HAL_Delay( 1000 );

    HAL_GPIO_WritePin( GPIOC, GPIO_PIN_13, 1);
    HAL_Delay( 1000 );
    goto f1;
  }
void gpio_init( void ){
    __HAL_RCC_GPIOC_CLK_ENABLE();
        GPIO_InitTypeDef led ;
        led.Pin = GPIO_PIN_14 | GPIO_PIN_13;
        led.Mode = GPIO_MODE_OUTPUT_PP;
        led.Pull = GPIO_NOPULL;
        led.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init( GPIOC, &led);


        HAL_GPIO_WritePin( GPIOC, GPIO_PIN_14, 0);
        HAL_GPIO_WritePin( GPIOC, GPIO_PIN_13, 0);
}
int main(void)
{
    SystemClock_Config();

    gpio_init();


    struct Task_t task_1 = {0};
    struct Task_t task_2 = {0};
    Schedul_createTaskAndAppend( task_1_func, 0, &task_1 );
    Schedul_createTaskAndAppend( task_2_func, 0, &task_2 );


    PendSV_init();

    Schedul_init();

    HAL_Init(); /* NVIC_SetPriority( Systick_Irqn ) */



    while(1);

  /* USER CODE END 3 */
}

void SystemClock_Config(void)
{
RCC_OscInitTypeDef RCC_OscInitStruct = {0};
RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
RCC_OscInitStruct.HSIState = RCC_HSI_ON;
RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
{
    Error_Handler();
}

RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
{
    Error_Handler();
}
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

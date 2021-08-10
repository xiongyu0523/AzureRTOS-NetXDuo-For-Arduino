#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32l4xx_hal.h"

#define REG32(x)                            (*(volatile unsigned int *)(x))

/* Define RCC register.  */
#define STM32L4_RCC                         0x40021000
#define STM32L4_RCC_AHB2ENR                 REG32(STM32L4_RCC + 0x4C)
#define STM32L4_RCC_AHB2ENR_RNGEN           0x00040000

/* Define RNG registers.  */
#define STM32_RNG                           0x50060800
#define STM32_RNG_CR                        REG32(STM32_RNG + 0x00)
#define STM32_RNG_SR                        REG32(STM32_RNG + 0x04)
#define STM32_RNG_DR                        REG32(STM32_RNG + 0x08)

#define STM32_RNG_CR_RNGEN                  0x00000004
#define STM32_RNG_SR_DRDY                   0x00000001

void hardware_rand_initialize(void)
{
  /* Enable clock for the RNG.  */
  STM32L4_RCC_AHB2ENR |= STM32L4_RCC_AHB2ENR_RNGEN;

  /* Enable the random number generator.  */
  STM32_RNG_CR = STM32_RNG_CR_RNGEN;
}

int hardware_rand(void)
{
  /* Wait for data ready.  */
  while((STM32_RNG_SR & STM32_RNG_SR_DRDY) == 0);

  /* Return the random number.  */
  return STM32_RNG_DR;
}

int board_setup(void)
{
  hardware_rand_initialize();

  srand(hardware_rand());

  return 0;
}

extern SPI_HandleTypeDef hspi;


void SPI3_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi);
}

void EXTI1_IRQHandler(void)
{
 HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case (GPIO_PIN_1):
    {
      SPI_WIFI_ISR();
      break;
    }

    default:
    {
      break;
    }
  }
}
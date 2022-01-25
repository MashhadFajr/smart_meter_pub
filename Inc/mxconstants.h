/**
  ******************************************************************************
  * File Name          : mxconstants.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * COPYRIGHT(c) 2021 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define OPTO_RL1_STOP_Pin GPIO_PIN_13
#define OPTO_RL1_STOP_GPIO_Port GPIOC
#define OPTO_RL1_RUN_Pin GPIO_PIN_14
#define OPTO_RL1_RUN_GPIO_Port GPIOC
#define buzzer_Pin GPIO_PIN_15
#define buzzer_GPIO_Port GPIOC
#define RADIO_M1_Pin GPIO_PIN_0
#define RADIO_M1_GPIO_Port GPIOA
#define RADIO_M2_Pin GPIO_PIN_1
#define RADIO_M2_GPIO_Port GPIOA
#define EN_485_2_Pin GPIO_PIN_4
#define EN_485_2_GPIO_Port GPIOA
#define KEY_Pin GPIO_PIN_1
#define KEY_GPIO_Port GPIOB
#define EN_485_1_Pin GPIO_PIN_2
#define EN_485_1_GPIO_Port GPIOB
#define EN_485_3_Pin GPIO_PIN_12
#define EN_485_3_GPIO_Port GPIOB
#define en_595_Pin GPIO_PIN_14
#define en_595_GPIO_Port GPIOB
#define LED_CPU_Pin GPIO_PIN_8
#define LED_CPU_GPIO_Port GPIOA
#define OPTO_RL3_RUN_Pin GPIO_PIN_9
#define OPTO_RL3_RUN_GPIO_Port GPIOA
#define OPTO_RL3_STOP_Pin GPIO_PIN_10
#define OPTO_RL3_STOP_GPIO_Port GPIOA
#define OPTO_RL4_RUN_Pin GPIO_PIN_11
#define OPTO_RL4_RUN_GPIO_Port GPIOA
#define OPTO_RL4_STOP_Pin GPIO_PIN_12
#define OPTO_RL4_STOP_GPIO_Port GPIOA
#define CS_SPI1_Pin GPIO_PIN_15
#define CS_SPI1_GPIO_Port GPIOA
#define OPTO_RL2_STOP_Pin GPIO_PIN_8
#define OPTO_RL2_STOP_GPIO_Port GPIOB
#define OPTO_RL2_RUN_Pin GPIO_PIN_9
#define OPTO_RL2_RUN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */


typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
//typedef unsigned long    U32;
typedef unsigned int    U32;
typedef long long       S64;
typedef unsigned long long U64;
typedef unsigned char   BIT;
typedef unsigned int    BOOL;


/* USER CODE END Private defines */

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

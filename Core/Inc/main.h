/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FIR_SA_ME_Pin GPIO_PIN_2
#define FIR_SA_ME_GPIO_Port GPIOE
#define FIR_SA_ME_EXTI_IRQn EXTI2_IRQn
#define HCSR_TRIG_SX_Pin GPIO_PIN_3
#define HCSR_TRIG_SX_GPIO_Port GPIOE
#define HCSR_ECHO_SX_Pin GPIO_PIN_6
#define HCSR_ECHO_SX_GPIO_Port GPIOE
#define IV_TD_Pin GPIO_PIN_4
#define IV_TD_GPIO_Port GPIOF
#define HCSR_TRIG_SY_Pin GPIO_PIN_7
#define HCSR_TRIG_SY_GPIO_Port GPIOF
#define HCSR_ECHO_SY_Pin GPIO_PIN_8
#define HCSR_ECHO_SY_GPIO_Port GPIOF
#define SG90_PWM_Pin GPIO_PIN_9
#define SG90_PWM_GPIO_Port GPIOF
#define IV_FE_Pin GPIO_PIN_2
#define IV_FE_GPIO_Port GPIOC
#define FIR_PWM_ME_Pin GPIO_PIN_0
#define FIR_PWM_ME_GPIO_Port GPIOA
#define FIR_FW_ME_Pin GPIO_PIN_0
#define FIR_FW_ME_GPIO_Port GPIOB
#define IV_PL_Pin GPIO_PIN_1
#define IV_PL_GPIO_Port GPIOB
#define IV_FD_Pin GPIO_PIN_2
#define IV_FD_GPIO_Port GPIOB
#define FIR_SB_MD_Pin GPIO_PIN_12
#define FIR_SB_MD_GPIO_Port GPIOE
#define FIR_SA_MD_Pin GPIO_PIN_14
#define FIR_SA_MD_GPIO_Port GPIOE
#define FIR_SA_MD_EXTI_IRQn EXTI15_10_IRQn
#define FIR_RV_MD_Pin GPIO_PIN_15
#define FIR_RV_MD_GPIO_Port GPIOE
#define FIR_FW_MD_Pin GPIO_PIN_10
#define FIR_FW_MD_GPIO_Port GPIOB
#define FIR_PWM_MD_Pin GPIO_PIN_11
#define FIR_PWM_MD_GPIO_Port GPIOB
#define FIR_SB_ME_Pin GPIO_PIN_11
#define FIR_SB_ME_GPIO_Port GPIOD
#define IV_TE_Pin GPIO_PIN_6
#define IV_TE_GPIO_Port GPIOB
#define FIR_RV_ME_Pin GPIO_PIN_0
#define FIR_RV_ME_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

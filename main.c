/**
  ******************************************************************************
  * @file    GPIO_Toggle\main.c
  * @author  MCD Application Team
  * @version  V2.2.0
  * @date     30-September-2014
  * @brief   This file contains the main function for GPIO Toggle example.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
//#include "stm8s.h"
#include "BB_i2c.h"

/**
  * @addtogroup GPIO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Evalboard I/Os configuration */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t Register_readvalue;
/* Private function prototypes -----------------------------------------------*/
void Delay (uint16_t nCount);


/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  uint16_t optValue = 0;
  uint16_t optAddr = 0x4803;
  uint8_t optData = 0x40;
  
  uint16_t optionByte_status;/*Record the status of activating the optional function Beep/TIM2_CH1*/
  
  optionByte_status=FLASH_ReadOptionByte(optAddr);
  
  if(!(optionByte_status  == 0x40bf))
  {
   /* Define flash programming Time*/
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
  
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    /* Wait until Flash Program area unlocked flag is set*/
    while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
    {}
    
    FLASH_EraseOptionByte(optAddr);
    /* Wait until End of high voltage flag is set*/
    while (FLASH_GetFlagStatus(FLASH_FLAG_HVOFF) == RESET)
    {}

    FLASH_ProgramOptionByte(optAddr, (uint8_t)optData);
    
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    /* Unlock Flash */
    FLASH_Unlock(FLASH_MEMTYPE_PROG);
		
    /* Get Illegal Opcode flag status */
    RST_GetFlagStatus(RESET);
    
    FLASH_Lock(FLASH_MEMTYPE_PROG);
    
    optionByte_status=FLASH_ReadOptionByte(optAddr);
  }

  
  Register_readvalue =0;

  BB_I2C_Init();
	
  /* 	write code here */
  // 	Test read register
  // 	read register 0x08 and copy the result in Register_readvalue
  //	BB_I2C_ReadReg (0x08, &Register_readvalue, 0x50);
  // 	Value read: 0x2011 which is correct

  Delay(500);
  
  // Test Read register
  BB_I2C_ReadReg (0x75, &Register_readvalue, SlaveAddressshiftedByOne);
  // Test pass: I read back 0x68

  Delay(10); // to put breakpoint

  while (1)
  {

  }

}




/**
  * @brief Delay
  * @param nCount
  * @retval None
  */
void Delay(uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

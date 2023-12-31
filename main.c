/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the XMC MCU: CCU8 triggered VADC
               Example for ModusToolbox. This example shows working of VADC 
               triggered by CCU8 peripheral on every period match event.
               The main file has the custom logic to turn on LED when VADC 
               measurement is above the value of ADC_MID_OF_RANGE value and 
               turn off the LED when VADC measurement is below the value of 
               ADC_MID_OF_RANGE. The output of the VADC is printed via debug 
               UART and can be verified with serial monitor.
*
* Related Document: See README.md
*
******************************************************************************
*
* Copyright (c) 2015-2021, Infineon Technologies AG
* All rights reserved.                        
*                                             
* Boost Software License - Version 1.0 - August 17th, 2003
* 
* Permission is hereby granted, free of charge, to any person or organization
* obtaining a copy of the software and accompanying documentation covered by
* this license (the "Software") to use, reproduce, display, distribute,
* execute, and transmit the Software, and to prepare derivative works of the
* Software, and to permit third-parties to whom the Software is furnished to
* do so, all subject to the following:
* 
* The copyright notices in the Software and this entire statement, including
* the above license grant, this restriction and the following disclaimer,
* must be included in all copies of the Software, in whole or in part, and
* all derivative works of the Software, unless such copies or derivative
* works are solely in the form of machine-executable object code generated by
* a source language processor.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*                                                                              
*****************************************************************************/

#include "cybsp.h"
#include "cy_utils.h"
#include "cy_retarget_io.h"

/*******************************************************************************
* Macros
*******************************************************************************/

/* Define macros for XMC14x Boot kits */
#if (UC_SERIES == XMC14)
#define ADC_CONVERSION_EVENT_HANDLER        IRQ_Hdlr_19
#define INTERRUPT_PRIORITY_NODE_ID          IRQ19_IRQn
#endif

/* Define macros for XMC48x, XMC47x and XMC45x Relax kits and for XMC4200 and XMC4400 PLT2GO kits */
#if (UC_FAMILY == XMC4)
#define ADC_CONVERSION_EVENT_HANDLER        VADC0_G0_0_IRQHandler
#define INTERRUPT_PRIORITY_NODE_ID          VADC0_G0_0_IRQn
#endif

#define RES_REG_NUMBER                      (0)
#define ADC_MID_OF_RANGE                    2047

/*******************************************************************************
* Global Variables
*******************************************************************************/
volatile XMC_VADC_RESULT_SIZE_t adc_result = 0;
volatile uint8_t end_of_conversion=0;

/*******************************************************************************
* Function Name: ADC_CONVERSION_EVENT_HANDLER
********************************************************************************
* Summary:
* This is the interrupt handler function for the ADC end of conversion.
* The ADC result is retrieved after conversion inside this function.
* On board LED glows high when ADC counts are more than ADC_MID_OF_RANGE value.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void ADC_CONVERSION_EVENT_HANDLER(void)
{
    /* Retrieve result from result register. */
    adc_result = XMC_VADC_GROUP_GetResult(VADC_GROUP_HW, RES_REG_NUMBER);

    if(adc_result >= ADC_MID_OF_RANGE)
    {
        #if (UC_FAMILY == XMC4)
        XMC_GPIO_SetOutputHigh(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
        #else
        XMC_GPIO_SetOutputLow(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
        #endif
    }
    else
    {
        #if (UC_FAMILY == XMC4)
        XMC_GPIO_SetOutputLow(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
        #else
        XMC_GPIO_SetOutputHigh(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
        #endif
    }

    end_of_conversion=1;
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* 
* The bsp_init sets up a CCU8 to trigger VADC conversion.
* The output of the VADC is printed via UART inside main function.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize retarget-io to use the debug UART port */
    cy_retarget_io_init(CYBSP_DEBUG_UART_HW);

    NVIC_EnableIRQ(INTERRUPT_PRIORITY_NODE_ID);

    while(1)
    {
        if(end_of_conversion)
        {
            printf("ADC VALUE: %d\r\n", adc_result);
            end_of_conversion=0;
        }
    }
}

/* [] END OF FILE */

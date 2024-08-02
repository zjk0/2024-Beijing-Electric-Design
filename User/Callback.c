#include "stm32f4xx_hal.h"
#include "main.h"
#include "AD9959_Outset.h"
#include "dac.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == RotateBottom_1_Pin) {
        while (HAL_GPIO_ReadPin(RotateBottom_1_GPIO_Port, RotateBottom_1_Pin) == GPIO_PIN_RESET);

        if (HAL_GPIO_ReadPin(RotateBottom_2_GPIO_Port, RotateBottom_2_Pin) == GPIO_PIN_RESET) {  // Clockwise: Increase amplitude
            bottom = ClockwiseRotate;
        }
        else if (HAL_GPIO_ReadPin(RotateBottom_2_GPIO_Port, RotateBottom_2_Pin) == GPIO_PIN_SET) {  // Anticlockwise: Decrease amplitude
            bottom = AnticlockwiseRotate;
        }
    }
    else if (GPIO_Pin == Bottom_Pin) {
        while (HAL_GPIO_ReadPin(Bottom_GPIO_Port, Bottom_Pin) == GPIO_PIN_RESET);
        bottom = PushDown;
    }
}

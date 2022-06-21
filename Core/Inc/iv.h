#ifndef INC_IV_H_
#define INC_IV_H_

#include "usart.h"
#include "gpio.h"
#include "tim.h"

struct obstacle_detection_t
{
	GPIO_PinState PL, FD, FE, TD, TE;
};

struct obstacle_detection_t read_obstacles();

#endif /* INC_IV_H_ */

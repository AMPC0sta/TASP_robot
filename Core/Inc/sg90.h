#ifndef INC_SG90_H_
#define INC_SG90_H_

#include "usart.h"
#include "gpio.h"
#include "tim.h"

int SG90_set_ref(int angle);
void SG90_start();
void SG90_stop();
void rise();
void lower();

#endif /* INC_SG90_H_ */

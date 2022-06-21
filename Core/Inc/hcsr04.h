#ifndef INC_HCSR04_H_
#define INC_HCSR04_H_

#include "usart.h"
#include "gpio.h"
#include "tim.h"

struct distSensor_t
{
	uint32_t ic1, ic2, diff;
	uint8_t isFirstCapture, ready;
	float dist;
};

struct coords
{
	float x, y;
};

void proc_distance();
void proc_TRIG_FSM();
void read_ECHO_pulse(char coord);
struct coords get_distance();

#endif /* INC_HCSR04_H_ */

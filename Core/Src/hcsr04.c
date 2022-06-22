#include "hcsr04.h"

struct distSensor_t distSensorX = { 0, 0, 0, 0, 0, 0.0 };
struct distSensor_t distSensorY = { 0, 0, 0, 0, 0, 0.0 };
struct coords distance = { 0.0, 0.0 };
struct coords safeLocation = { 0.0, 0.0 }; // home
static uint8_t state = 0, cnt = 0;

void HCSR_proc_TRIG_FSM()
{
	if(state == 0)														// STATE 0: disable TRIGGER
	{
		state = 1;
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, 0);
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 0);
	}
	else if(state == 1)													// STATE 1: wait 2uS and enable TRIGGER
	{
		if(++cnt == 2)
		{
			cnt = 0;
			state = 2;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, 1);
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 1);
		}
	}
	else if(state == 2)													// STATE 2: wait 10uS and disable TRIGGER
	{
		if(++cnt == 10)
		{
			cnt = 0;
			state = 0;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, 0);
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 0);
			HAL_TIM_Base_Stop_IT(&htim6);
		}
	}
}

void HCSR_read_ECHO_pulse(char coord)
{
	struct distSensor_t* distSensor;
	TIM_HandleTypeDef* htim;
	uint32_t channel;

	if(coord == 'x')
	{
		distSensor = &distSensorX;
		htim = &htim9;
		channel = TIM_CHANNEL_2;
	}
	else
	{
		distSensor = &distSensorY;
		htim = &htim13;
		channel = TIM_CHANNEL_1;
	}

	if(!distSensor->isFirstCapture)														// if positive edge transition
	{
		distSensor->ic1 = HAL_TIM_ReadCapturedValue(htim, channel);						// read first timer counter value
		distSensor->isFirstCapture = 1;
	}
	else   																				// if negative edge transition
	{
		distSensor->ic2 = HAL_TIM_ReadCapturedValue(htim, channel); 					// read second timer counter value

		HAL_TIM_IC_Stop_IT(htim, channel);												// stop timer

		__HAL_TIM_SET_COUNTER(htim, 0);  												// reset the timer counter

		if(distSensor->ic2 > distSensor->ic1)											// calculate timer tick difference
			distSensor->diff = distSensor->ic2 - distSensor->ic1;
		else
			distSensor->diff = (0xFFFFFFFF - distSensor->ic1) + distSensor->ic2;

		distSensor->dist = ((float) distSensor->diff / 1000000.0) * 343.0 / 2.0;		// time convertion to distance

		if (distSensor->diff < 100 || distSensor->diff > 18000)							// object too close or too far
			distSensor->dist = -1;

		distSensor->isFirstCapture = 0; 												// set it back to detect positive edge

		distSensor->ready = 1;															// set flag
	}
}

void HCSR_proc_distance()
{
	HAL_TIM_IC_Start_IT(&htim9, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim13, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim6);
}

struct coords HCSR_get_distance()
{
	if(distSensorX.ready && distSensorY.ready)
	{
		distSensorX.ready = 0;
		distSensorY.ready = 0;

		distance.x = distSensorX.dist;
		distance.y = distSensorY.dist;

		char message[32];
		sprintf(message, "coords = (%0.2f, %0.2f)", distance.x, distance.y);
		send_UART(message);
	}

	return distance;
}

void HCSR_save_coords()
{
	safeLocation = HCSR_get_distance();
}

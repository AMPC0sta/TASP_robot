#include "iv.h"

struct obstacle_detection_t obs_det = { 0, 0, 0, 0, 0 };
struct obstacle_detection_t lst_obs_det;

struct obstacle_detection_t read_obstacles()
{
	obs_det.PL = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);
	obs_det.FD = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
	obs_det.FE = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);
	obs_det.TD = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
	obs_det.TE = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);

	if(lst_obs_det.PL != obs_det.PL ||
	   lst_obs_det.FD != obs_det.FD ||
	   lst_obs_det.FE != obs_det.FE ||
	   lst_obs_det.TD != obs_det.TD ||
	   lst_obs_det.TE != obs_det.TE)
	{
		/*
		char message[32];
		//sprintf(message, "obst = [%d||%d-%d||%d-%d]", obs_det.PL, obs_det.FE, obs_det.FD, obs_det.TE, obs_det.TD);
		sprintf(message, "obst = [x||x-x||%d-%d]", obs_det.TE, obs_det.TD);
		send_UART(message);
		*/
	}

	lst_obs_det = obs_det;
	return obs_det;
}

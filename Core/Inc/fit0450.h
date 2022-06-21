#ifndef INC_FIT0450_H_
#define INC_FIT0450_H_

#include <stdio.h>
#include <string.h>

struct hw_cfg_t
{
	unsigned int value;
	char timeunit[2];
};

struct motor_t
{
	float pos_r, pos_m, pos_m_last, spd_r, spd_m,
		yr, yr_last, sum_e, e_last, u, u_d_last,
		Kp_h, Kd_h, Ki_h;
};

#include "usart.h"
#include "gpio.h"
#include "tim.h"

#define PI 3.1415926

#define RV 0 // '-'
#define FW 1 // '+'

#define U_SAT_MAX 6
#define U_SAT_MIN -6

#define POS_UNIT 0.00654498 // 2*pi / 960

#define a 0.4

void start();
void stop();
void proc_ISR_PID(void);
void proc_ISR_speed(void);
void update_posR(int);
void update_posL(int);
void set_ref(float ref, char mode);
void set_Kh(void);
float calc_hw(void);
float calc_PID(struct motor_t *motor);


#endif /* INC_FIT0450_H_ */

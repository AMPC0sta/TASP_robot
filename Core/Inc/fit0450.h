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
		yr, yr_last, sum_e, e_last, u_d_last,
		Kp_h, Kd_h, Ki_h;
};

struct orientation_t
{
	float sum_e, e_last, u_d_last, Kp_h, Kd_h, Ki_h, Kp;
	int px_m, ang_m, ang_m_last, ang_r;
};

#include "usart.h"
#include "gpio.h"
#include "tim.h"

#define PI 3.1415926

#define RV 0 // '-'
#define FW 1 // '+'

#define U_SAT_MAX_MOTOR 6
#define U_SAT_MIN_MOTOR -6
#define U_SAT_MAX_ANGLE 1
#define U_SAT_MIN_ANGLE -1

#define POS_UNIT 0.00654498 // 2*pi / 960

#define a 0.4

void FIR_start();
void FIR_stop();
void proc_ISR_PID(void);
void proc_ISR_speed(void);
void update_posR(int);
void update_posL(int);
void FIR_set_ref(float, char, char);
void set_Kh(char);
float calc_hw(void);
float calc_PID_motor(struct motor_t *motor);
void ISR_PID_angle();
void set_px(int px);
void set_ang(float ang);
void inc_kp();
void dec_kp();


#endif /* INC_FIT0450_H_ */

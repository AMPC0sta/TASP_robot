#include "fit0450.h"

int n = 0, aut = 1, mode = 0;
float u = 0;

struct hw_cfg_t hw_cfg = { 50, "ms" };
struct motor_t motorL = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
struct motor_t motorR = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
struct orientation_t robotOrientation = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0 };

float KpR = 2.0, KdR = 0.4, KiR = 1.3;
float KpL = 2.0, KdL = 0.4, KiL = 1.3;
float KpO = 0.01665, KdO = 0.4, KiO = 1.3;

float Kp = 1.0;

void inc_kp()
{
	robotOrientation.Kp_h *= 1.5;
	char message[32];
	sprintf(message, "kp=%f", robotOrientation.Kp_h);
	send_UART(message);
}

void dec_kp()
{
	robotOrientation.Kp_h /= 1.5;
	char message[32];
	sprintf(message, "kp=%f", robotOrientation.Kp_h);
	send_UART(message);
}

void set_ang(float ang)
{
	robotOrientation.ang_m = ang;
}

void FIR_start()
{
	set_Kh('M');
	MY_TIM3_Init(hw_cfg);

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);

	HAL_TIM_Base_Start_IT(&htim3);

	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


void FIR_stop()
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_1);

	HAL_TIM_Base_Stop_IT(&htim3);

	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void reset()
{
	memset(&motorL, 0, sizeof(motorL));
	memset(&motorR, 0, sizeof(motorR));
}

void proc_ISR_PID()
{
	float hw = calc_hw();

	if(mode)
	{
		motorL.yr = hw * motorL.spd_r + motorL.yr_last;
		motorR.yr = hw * motorR.spd_r + motorR.yr_last;
	}
	else
	{
		motorL.yr = motorL.pos_r;
		motorR.yr = motorR.pos_r;
	}

	float uL = calc_PID_motor(&motorL);
	float uR = calc_PID_motor(&motorR);

	if(aut)
	{
		if(uR > 0)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, 0);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 1);
		}
		else if(uR < 0)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, 1);

			 uR = -uR;
		}

		if(uL > 0)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, 1);
		}
		else if(uL < 0)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, 0);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);

			 uL = -uL;
		}

		TIM2->CCR4 = 2.5 / 6.0 * 2000;
		TIM5->CCR1 = 2.5 / 6.0 * 2000; //L

		/*
		char message[32];
		sprintf(message, "R=%d, L=%d", uR / 6 * 2000, uL / 6 * 2000);
		send_UART(message);
*/
		if(u > 0)
		{
			float dc = TIM5->CCR1 * (1-u);
			TIM5->CCR1 = dc;
		}
		else if(u < 0)
		{
			float dc = TIM2->CCR4 * (1+u);
			TIM2->CCR4 = dc;
		}


	}
	else
	{
		motorL.e_last = motorL.yr - motorL.pos_m;
		motorL.pos_m_last = motorL.pos_m;

		motorR.e_last = motorR.yr - motorR.pos_m;
		motorR.pos_m_last = motorR.pos_m;
	}

	motorL.yr_last = motorL.yr;
	motorR.yr_last = motorR.yr;
}


void proc_ISR_speed()
{
	float hw = calc_hw();

	motorL.spd_m = (motorL.pos_m - motorL.pos_m_last) / hw;
	motorR.spd_m = (motorR.pos_m - motorR.pos_m_last) / hw;


}


void update_posL(int dir_m)
{
	if(dir_m)
		motorL.pos_m += POS_UNIT;
	else
		motorL.pos_m -= POS_UNIT;
}


void update_posR(int dir_m)
{
	if(dir_m)
		motorR.pos_m += POS_UNIT;
	else
		motorR.pos_m -= POS_UNIT;
}


void FIR_set_ref(float ref, char type, char motor)
{
	if(type == 'P')
	{
		mode = 0;

		if(motor == 'L')
			motorL.pos_r = ref;
		else
			motorR.pos_r = ref;
	}
	else
	{
		mode = 1;

		if(motor == 'L')
			motorL.spd_r = ref;
		else
			motorR.spd_r = ref;
	}
}


void set_Kh(char mode)
{
	if(mode == 'M')
	{
		float hw = calc_hw();
		motorL.Kp_h = KpL;
		motorL.Kd_h = KdL * (1 - a) / hw;
		motorL.Ki_h = KiL * hw;

		motorR.Kp_h = KpR;
		motorR.Kd_h = KdR * (1 - a) / hw;
		motorR.Ki_h = KiR * hw;
	}
	else
	{
		float hw = 0.001;
		robotOrientation.Kp_h = KpO;
		robotOrientation.Kd_h = KdO * (1 - a) / hw;
		robotOrientation.Ki_h = KiO * hw;
	}
}


float calc_hw()
{
	if(!strcmp(hw_cfg.timeunit, "us"))
	{
		return 0.000001 * hw_cfg.value;
	}
	else if(!strcmp(hw_cfg.timeunit, "ms"))
	{
		return 0.001 * hw_cfg.value;
	}
	else if(!strcmp(hw_cfg.timeunit, "s"))
	{
		return hw_cfg.value;
	}

	return 0.0000001;
}


float calc_PID_motor(struct motor_t *motor)
{
	float e = motor->yr - motor->pos_m;

	float sum_e_bkp = motor->sum_e;
	motor->sum_e = motor->sum_e + motor->e_last;
	float u_d = motor->Kd_h * (motor->pos_m - motor->pos_m_last) + a * motor->u_d_last;
	float u = motor->Kp_h * e + motor->Ki_h * motor->sum_e - u_d;
	motor->e_last = e;
	motor->pos_m_last = motor->pos_m;
	motor->u_d_last = u_d;

	if (u > U_SAT_MAX_MOTOR)
	{
		u = U_SAT_MAX_MOTOR;
		motor->sum_e = sum_e_bkp;
	}
	else if(u < U_SAT_MIN_MOTOR)
	{
		u = U_SAT_MAX_MOTOR;
		motor->sum_e = sum_e_bkp;
	}

	return u;
}



void move(float speed_L, float speed_R)
{
	float Kp = 1;

	if(speed_L <= 160.0 && speed_L >= -160.0 && speed_R <= 160.0 && speed_R >= -160.0)
	{
		float spd_r = Kp*(speed_L * 2 * PI / 60);
		FIR_set_ref(spd_r, 'S', 'L');

		spd_r = Kp*(speed_R * 2 * PI / 60);
		FIR_set_ref(speed_R, 'S', 'R');

		//start();
	}
}

void rotate(float angle)
{
	float Kp = 1;

	if(angle <= 360.0 && angle >= -360.0)
	{
		float pos_r = Kp*(angle * PI / 180);

		FIR_set_ref(-pos_r, 'P', 'L');
		FIR_set_ref(pos_r, 'P', 'R');

		//reset();
		//start();
	}
}


float calc_PID_angle(struct orientation_t *robotOrientation)
{
	robotOrientation->ang_r = 90;

	if(robotOrientation->ang_m < 0)
		robotOrientation->ang_m += 180;

	float e = robotOrientation->ang_r - robotOrientation->ang_m;

	float sum_e_bkp = robotOrientation->sum_e;
	robotOrientation->sum_e = robotOrientation->sum_e + robotOrientation->e_last;
	float u_d = robotOrientation->Kd_h * (robotOrientation->ang_m - robotOrientation->ang_m_last) + a * robotOrientation->u_d_last;
	//float u = robotOrientation->Kp_h * e + robotOrientation->Ki_h * robotOrientation->sum_e - u_d; /*robotOrientation->Kp * robotOrientation->px_m*/;
	robotOrientation->e_last = e;
	robotOrientation->ang_m_last = robotOrientation->ang_m;
	robotOrientation->u_d_last = u_d;

	float u = robotOrientation->Kd_h * e;

	if (u > U_SAT_MAX_ANGLE)
	{
		u = U_SAT_MAX_ANGLE;
		robotOrientation->sum_e = sum_e_bkp;
	}
	else if(u < U_SAT_MIN_ANGLE)
	{
		u = U_SAT_MIN_ANGLE;
		robotOrientation->sum_e = sum_e_bkp;
	}


	if(++n == 1000)
	{
		char message[32];
			sprintf(message, "u=%0.2f", u);
			send_UART(message);
		n = 0;
	}


	return u;
}

void ISR_PID_angle()
{
	u = calc_PID_angle(&robotOrientation);
}

void set_px(int px)
{
	robotOrientation.px_m = px;
}

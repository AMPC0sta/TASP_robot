#include "fit0450.h"

int cnt = 0, aut = 1, mode = 0;

float KpR = 2.0, KdR = 0.4, KiR = 1.3;
float KpL = 2.0, KdL = 0.4, KiL = 1.3;

struct hw_cfg_t hw_cfg = { 50, "ms" };
struct motor_t motorL = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
struct motor_t motorR = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };


void start()
{
	set_Kh();
	MY_TIM3_Init(hw_cfg);

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);

	HAL_TIM_Base_Start_IT(&htim3);

	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


void stop()
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

	float uL = calc_PID(&motorL);
	float uR = calc_PID(&motorR);

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

		TIM2->CCR4 = uR / 6 * 2000;
		TIM5->CCR1 = uL / 6 * 2000;
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

	if(++cnt == 5)
	{
		cnt = 0;
		char message[32];
		sprintf(message, "%0.2f|%0.2f || %0.2f|%0.2f", motorL.pos_m, motorL.spd_m, motorR.pos_m, motorR.spd_m);
		send_UART(message);
	}
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


void set_Kh(void)
{
	float hw = calc_hw();

	motorL.Kp_h = KpL;
	motorL.Kd_h = KdL * (1 - a) / hw;
	motorL.Ki_h = KiL * hw;

	motorR.Kp_h = KpR;
	motorR.Kd_h = KdR * (1 - a) / hw;
	motorR.Ki_h = KiR * hw;
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


float calc_PID(struct motor_t *motor)
{
	float e = motor->yr - motor->pos_m;

	float sum_e_bkp = motor->sum_e;
	motor->sum_e = motor->sum_e + motor->e_last;
	float u_d = motor->Kd_h * (motor->pos_m - motor->pos_m_last) + a * motor->u_d_last;
	float u = motor->Kp_h * e + motor->Ki_h * motor->sum_e - u_d;
	motor->e_last = e;
	motor->pos_m_last = motor->pos_m;
	motor->u_d_last = u_d;

	if (u > U_SAT_MAX)
	{
		u = U_SAT_MAX;
		motor->sum_e = sum_e_bkp;
	}
	else if(u < U_SAT_MIN)
	{
		u = U_SAT_MIN;
		motor->sum_e = sum_e_bkp;
	}

	return u;
}



void move(float speed_L, float speed_R)
{
	float Kp;

	if(speed_L <= 160.0 && speed_L >= -160.0 && speed_R <= 160.0 && speed_R >= -160.0)
	{
		float spd_r = Kp*(speed_L * 2 * PI / 60);
		FIR_set_ref(spd_r, 'S', 'L');

		spd_r = Kp*(speed_R * 2 * PI / 60);
		FIR_set_ref(speed_R, 'S', 'R');

		start();
	}
}

void rotate(float angle)
{
	float Kp;

	if(angle <= 360.0 && angle >= -360.0)
	{
		float pos_r = Kp*(angle * PI / 180);

		FIR_set_ref(-pos_r, 'P', 'L');
		FIR_set_ref(pos_r, 'P', 'R');

		reset();
		start();
	}
}

// função controlo motores em função do ângulo de orientação

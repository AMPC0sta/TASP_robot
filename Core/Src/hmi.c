#include "hmi.h"

float pos_r = 0.0, spd_r = 0.0;
char m = 'S';

unsigned char check_command(char* message)
{
    char cmd = INV;

    if((!strncmp((char*) message, "REF", 3)))
		cmd = REF;
    else if((!strncmp((char*) message, "EN", 2)))
        cmd = EN;
	else if((!strncmp((char*) message, "/", 1)))
		cmd = INC;
	else if((!strncmp((char*) message, "\\", 1)))
		cmd = DEC;
	else if((!strncmp((char*) message, "DIST", 4)))
		cmd = DIST;
    else
    	cmd = INV;

    return cmd;
}


void (*exec_command[])(char* message) = {
		proc_inv_cmd,
		proc_en_cmd,
		proc_ref_cmd,
		proc_inc_cmd,
		proc_dec_cmd,
		proc_dist_cmd
};


void proc_inv_cmd(char* message)
{
	send_UART("Invalid instruction. Type '?' for Help.");
}


void proc_en_cmd(char* message)
{
	int val;

	if(sscanf((char*) message, "EN %d", &val) == 1)
	{
		if(val == 0 || val == 1)
		{
			if(val)
			{
				//start();
				send_UART("System enabled with success.");
			}
			else
			{
				//stop();
				send_UART("System disabled with success.");
			}
		}
		else
			send_UART("Invalid Enable instruction argument values.");
	}
	else
		send_UART("Invalid Enable instruction syntax.");
}


void proc_ref_cmd(char* message)
{
	//stop();

	char type, sign;
	int val;

	if(sscanf((char*) message, "REF %c %c%d", &type, &sign, &val) == 3)
	{
		if((sign == '+' || sign == '-') && (type == 'P' || type == 'S') && val >= 0)
		{
			m = type;
			if(type == 'P')
			{
				pos_r = val * PI / 180;

				if(sign == '-')
					pos_r = -pos_r;

				//set_ref(pos_r, type);
			}
			else
			{
				spd_r = val * 2 * PI / 60;

				if(sign == '-')
					spd_r = -spd_r;

				//set_ref(spd_r, type);
			}

			send_UART("Desired reference changed with success.");
		}
		else
			send_UART("Invalid desired reference value.");
	}
	else
		send_UART("Invalid REF instruction syntax.");
}


void proc_inc_cmd(char* message)
{
	pos_r += 0.5;
	spd_r += 1.0;

	//if(m == 'P')
		//set_ref(pos_r, m);
	//else
		//set_ref(spd_r, m);
}

void proc_dec_cmd(char* message)
{
	pos_r -= 0.5;
	spd_r -= 1.0;

	//if(m == 'P')
	//	set_ref(pos_r, m);
	//else
	//	set_ref(spd_r, m);
}

void proc_dist_cmd(char* message)
{
	//proc_distance();
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */

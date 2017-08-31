#include <inttypes.h>
#include "command.h"
#include "hw_config.h"
#include "delay.h"


enum E_ACTION_REQUEST ExecuteCmd(unsigned char *dataIn, int length)
{
	if (length < 2) return REQUEST_NONE;
	if (dataIn[0] == CMD_FUNCTION)	//special functions
	{
		switch (dataIn[1])
		{
		case CMD_FUNCTION_IDENTIFY:		//identify request
			return REQUEST_IDENTIFY;
			break;
		}
	}
	return REQUEST_NONE;
}

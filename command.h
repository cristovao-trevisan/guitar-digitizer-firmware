/** \file
*/

#ifndef COMMAND_H
#define COMMAND_H

/** \brief Passed back type of action to execute
*/
enum E_COMMAND_TYPE
{
  CMD_FUNCTION = 0x05
};

/** \brief Second byte of command if first byte is CMD_FUNCTION */
enum E_SUBCOMMAND_FUNCTION_TYPE
{
    CMD_FUNCTION_IDENTIFY = 0x08
};


enum E_ACTION_REQUEST
{
	REQUEST_NONE = 0,
	REQUEST_IDENTIFY
};

struct S_ARMSCOPE_SETTINGS;

/** \brief Execute single command
	\return Action type to perform by caller
*/
enum E_ACTION_REQUEST ExecuteCmd(unsigned char *dataIn, int length);

#endif // COMMAND_H

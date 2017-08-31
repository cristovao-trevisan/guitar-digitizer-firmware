#include "stm32f10x.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "command.h"
#include <string.h>

int buzz = 0;
static const char szId[] = "guitar digitalizer v0.1";
/** This is data added as header to each frame that isn't actual data but that is reply
 to some host request.
 */
static const unsigned char replyHeader[] = { 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF };

enum E_ACTION_REQUEST action = REQUEST_NONE;

struct Data {
	uint16_t *data;
	int len;
} new_data;

extern uint8_t USART_Rx_Buffer[USART_RX_DATA_SIZE];
extern uint32_t USART_Rx_ptr_in;
extern uint32_t USART_Rx_ptr_out;

int main(void) {
	Set_System();
	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	new_data.data = NULL;
	new_data.len = 0;

	uint16_t *data = NULL;
	uint8_t count = 0;

	while (1) {
		if (action == REQUEST_IDENTIFY) {
			action = REQUEST_NONE;
			// insert reply header
			UsbSendData(replyHeader, sizeof(replyHeader));
			int len = sizeof(replyHeader);
			while (len + strlen(szId) < 4096) {
				UsbSendData((const unsigned char*) szId, strlen(szId));
				len += strlen(szId);
			}
			// pad reply to same size as regular data packet
			for (int i = 0; i < 4096 - len; i++) {
				USART_Rx_Buffer[USART_Rx_ptr_in] = 0;
				USART_Rx_ptr_in++;

				/* To avoid buffer overflow */
				if (USART_Rx_ptr_in == USART_RX_DATA_SIZE) {
					USART_Rx_ptr_in = 0;
				}
			}
		}

		if (new_data.data != data) {
			data = new_data.data;
			int len = new_data.len;

			// send header (0xab 0xcd + count)
			USART_Rx_Buffer[USART_Rx_ptr_in++] = 0xcd;
			if (USART_Rx_ptr_in == USART_RX_DATA_SIZE) USART_Rx_ptr_in = 0;
			USART_Rx_Buffer[USART_Rx_ptr_in++] = 0xab;
			if (USART_Rx_ptr_in == USART_RX_DATA_SIZE) USART_Rx_ptr_in = 0;
			USART_Rx_Buffer[USART_Rx_ptr_in++] = count++;
			if (USART_Rx_ptr_in == USART_RX_DATA_SIZE) USART_Rx_ptr_in = 0;
			USART_Rx_Buffer[USART_Rx_ptr_in++] = 0xef;
			if (USART_Rx_ptr_in == USART_RX_DATA_SIZE) USART_Rx_ptr_in = 0;

			unsigned char *ptr = (unsigned char*) data;
			for (int i = 0; i < 2*len; i++) {
				USART_Rx_Buffer[USART_Rx_ptr_in++] = *ptr; //*(((unsigned char*)&data[i]) + 1);
				ptr += 1;

				/* To avoid buffer overflow */
				if (USART_Rx_ptr_in == USART_RX_DATA_SIZE) {
					USART_Rx_ptr_in = 0;
				}
			}
		}
	}
}

void OnUsbDataRx(uint8_t* dataIn, uint8_t length) {
	if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13))
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, 0);
	else
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, 1);
	if (length >= 3) {
		if (dataIn[2] == 0x00) //end of message detected - correct frame
				{
			enum E_ACTION_REQUEST tmp = ExecuteCmd(dataIn, length);
			if (tmp != REQUEST_NONE) {
				action = tmp;
			}
		}
		length = 0;
	}
}

void OnAdcData(uint16_t *data, int len) {
	new_data.data = data;
	new_data.len = len;
}

void assert_failed(const char* file, uint32_t line) {
	while (1) {
	}
}


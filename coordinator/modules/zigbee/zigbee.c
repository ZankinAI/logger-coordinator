#include <string.h>
#include "main.h"
#include "zigbee.h"
#include "uart_driver/uart_driver.h"

#define HEADER                    0x55;

#define LOCAL_CONFIG_COMMAND_TYPE 0x00

/*   Local configuration commands    */
#define CFG_STATUS                0x00

extern uart_handle_t zigbee_usart;

zigbee_driver_t zigbee_driver;

void init_zigbee(zigbee_driver_t *zigbee, zigbee_callback callback)
{
    zigbee->callback = callback;
}

void set_input_packet(uint8_t *data, uint16_t length)
{
    if (length <= MAX_INPUT_PACKET_SIZE) {
        memcpy(zigbee_driver.input_packet.packet, data, length);
        zigbee_driver.input_packet.length = length;
    }
}

void parse_input_packet(zigbee_driver_t *zigbee)
{
}

void cmd_read_params(zigbee_driver_t *zigbee)
{
  zigbee->output_packet.packet[0] = HEADER;
  zigbee->output_packet.packet[1] = 0x03;	//length of payload
  zigbee->output_packet.packet[2] = LOCAL_CONFIG_COMMAND_TYPE;
  zigbee->output_packet.packet[3] = CFG_STATUS;
  zigbee->output_packet.packet[4] = 0x00;
  zigbee->output_packet.length = 5;

  uart_tx(&zigbee_usart, zigbee->output_packet.packet, zigbee->output_packet.length);
}

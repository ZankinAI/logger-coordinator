#include "zigbee.h"

#include <string.h>

#include "main.h"
#include "uart_driver/uart_driver.h"

extern uart_handle_t zigbee_usart;

void cmd_set_command_mode(zigbee_driver_t *zigbee)
{
	uart_tx(&zigbee_usart, (uint8_t *)"+++", 3);
}

void cmd_reset(zigbee_driver_t *zigbee)
{
    uint8_t index                          = 0;
    zigbee->output_packet.frame_header     = HEADER;
    zigbee->output_packet.frame_length     = 0x07; // length of payload
    zigbee->output_packet.cmd_type         = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->output_packet.cmd_code         = CFG_RESET;
    zigbee->output_packet.payload[index++] = 0x01;
    zigbee->output_packet.payload[index++] = zigbee->pan_id[0];
    zigbee->output_packet.payload[index++] = zigbee->pan_id[1];
    zigbee->output_packet.payload[index++] = zigbee->channel;
    zigbee->output_packet.payload[index++] =
        xor8((uint8_t *)&(zigbee->output_packet.cmd_type), zigbee->output_packet.frame_length - 1);

    uart_tx(&zigbee_usart, (uint8_t *)&zigbee->output_packet, zigbee->output_packet.frame_length + 2);

    zigbee->sended_cmd_type = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->sended_cmd_code = CFG_RESET;
    zigbee->state = wait_answer;
}

void cmd_config_module(zigbee_driver_t *zigbee, uint8_t mode)
{
    uint8_t index                          = 0;
    zigbee->output_packet.frame_header     = HEADER;
    zigbee->output_packet.frame_length     = 0x04; // length of payload
    zigbee->output_packet.cmd_type         = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->output_packet.cmd_code         = CFG_NODE_TYPE;
    zigbee->output_packet.payload[index++] = mode;

    zigbee->output_packet.payload[index++] =
        xor8((uint8_t *)&(zigbee->output_packet.cmd_type), zigbee->output_packet.frame_length - 1);

    uart_tx(&zigbee_usart, (uint8_t *)&zigbee->output_packet, zigbee->output_packet.frame_length + 2);

    zigbee->sended_cmd_type = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->sended_cmd_code = CFG_NODE_TYPE;
    zigbee->state = wait_answer;
}

void cmd_connect_to_nwk(zigbee_driver_t *zigbee)
{
    uint8_t index                          = 0;
    zigbee->output_packet.frame_header     = HEADER;
    zigbee->output_packet.frame_length     = 0x03; // length of payload
    zigbee->output_packet.cmd_type         = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->output_packet.cmd_code         = CFG_OPEN_NET;
    zigbee->output_packet.payload[index++] =
        xor8((uint8_t *)&(zigbee->output_packet.cmd_type), zigbee->output_packet.frame_length - 1);

    uart_tx(&zigbee_usart, (uint8_t *)&zigbee->output_packet, zigbee->output_packet.frame_length + 2);

    zigbee->sended_cmd_type = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->sended_cmd_code = CFG_OPEN_NET;
    zigbee->state = wait_answer;
}

void cmd_read_params(zigbee_driver_t *zigbee)
{
    zigbee->output_packet.frame_header = HEADER;
    zigbee->output_packet.frame_length = 0x03; // length of payload
    zigbee->output_packet.cmd_type     = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->output_packet.cmd_code     = CFG_STATUS;
    zigbee->output_packet.payload[0]   = 0x00;

    uart_tx(&zigbee_usart, (uint8_t *)&zigbee->output_packet, zigbee->output_packet.frame_length + 2);

    zigbee->sended_cmd_type = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->sended_cmd_code = CFG_STATUS;
    zigbee->state = wait_answer;
}

void cmd_set_target_short_addr(zigbee_driver_t *zigbee, uint16_t short_addr)
{
	uint8_t index = 0;
    zigbee->output_packet.frame_header = HEADER;
    zigbee->output_packet.frame_length = 0x08; // length of payload
    zigbee->output_packet.cmd_type     = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->output_packet.cmd_code     = CFG_WRITE_ATTR;
    zigbee->output_packet.payload[index++] = 0x00; //port index
    zigbee->output_packet.payload[index++] = 0x01;
    zigbee->output_packet.payload[index++] = 0x00; //attr id is 0x0001
    memcpy(&zigbee->output_packet.payload[index], (uint8_t *)&short_addr, sizeof(uint16_t)); //data
    index += sizeof(uint16_t);
    zigbee->output_packet.payload[index] =
        xor8((uint8_t *)&(zigbee->output_packet.cmd_type), zigbee->output_packet.frame_length - 1);

    uart_tx(&zigbee_usart, (uint8_t *)&zigbee->output_packet, zigbee->output_packet.frame_length + 2);

    zigbee->sended_cmd_type = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->sended_cmd_code = CFG_WRITE_ATTR;
    zigbee->state           = wait_answer;
}

void cmd_set_transmition_mode(zigbee_driver_t *zigbee)
{
	uint8_t index = 0;
    zigbee->output_packet.frame_header = HEADER;
    zigbee->output_packet.frame_length = 0x07; // length of payload
    zigbee->output_packet.cmd_type     = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->output_packet.cmd_code     = CFG_WRITE_ATTR;
    zigbee->output_packet.payload[index++] = 0x00; //port index
    zigbee->output_packet.payload[index++] = 0x03;
    zigbee->output_packet.payload[index++] = 0x00; //attr id is 0x0003
    zigbee->output_packet.payload[index++] = 0x01; //transmition mode is 0x01
    zigbee->output_packet.payload[index] =
        xor8((uint8_t *)&(zigbee->output_packet.cmd_type), zigbee->output_packet.frame_length - 1);

    uart_tx(&zigbee_usart, (uint8_t *)&zigbee->output_packet, zigbee->output_packet.frame_length + 2);

    zigbee->sended_cmd_type = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->sended_cmd_code = CFG_WRITE_ATTR;
    zigbee->state           = wait_answer;
}

void cmd_set_target_panid(zigbee_driver_t *zigbee, uint16_t panid)
{
    zigbee->output_packet.frame_header = HEADER;
    zigbee->output_packet.frame_length = 0x05; // length of payload
    zigbee->output_packet.cmd_type     = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->output_packet.cmd_code     = CFG_SET_PANID;
    memcpy(&zigbee->output_packet.payload[0], (uint8_t *)&panid, sizeof(uint16_t));
    zigbee->output_packet.payload[2] =
        xor8((uint8_t *)&(zigbee->output_packet.cmd_type), zigbee->output_packet.frame_length - 1);

    uart_tx(&zigbee_usart, (uint8_t *)&zigbee->output_packet, zigbee->output_packet.frame_length + 2);

    zigbee->sended_cmd_type = LOCAL_CONFIG_COMMAND_TYPE;
    zigbee->sended_cmd_code = CFG_SET_PANID;
    zigbee->state = wait_answer;
}

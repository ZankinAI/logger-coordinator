#include "zigbee.h"

#include <string.h>

static uint8_t zigbee_get_command_type(zigbee_packet_t packet);
static uint8_t zigbee_get_command_code(zigbee_packet_t packet);

static uint8_t answer_broken_cmd[] = {0x55, 0x03, 0xFF, 0xFF, 0x00};
static uint8_t answer_verification_not_correct[] = {0x55, 0x03, 0xFF, 0xFE, 0x01};

void parse_input_packet(zigbee_driver_t *zigbee)
{
    zigbee_packet_t temp_packet;

    uint8_t cmd_type;
    uint8_t cmd_code;
    memcpy(&temp_packet, &zigbee->input_packet, sizeof(zigbee_packet_t));

    //is async message
}

void parse_answer_packet(zigbee_driver_t *zigbee)
{
    zigbee_packet_t temp_packet;

    uint8_t cmd_type;
    uint8_t cmd_code;
    memcpy(&temp_packet, &zigbee->input_packet, sizeof(zigbee_packet_t));

    if (strcmp((const char *)&temp_packet, (const char *)&answer_broken_cmd)) {
    	zigbee->answer_state = error;
    }

    if (strcmp((const char *)&temp_packet, (const char *)&answer_verification_not_correct)) {
    	zigbee->answer_state = error;
    }

    cmd_type = zigbee_get_command_type(temp_packet);
    cmd_code = zigbee_get_command_code(temp_packet);

    if ((zigbee->state == wait_answer) && (zigbee->sended_cmd_type == cmd_type) &&
        (zigbee->sended_cmd_code == cmd_code)) {

    }


}



static uint8_t zigbee_get_command_type(zigbee_packet_t packet)
{
    return packet.cmd_type;
}

static uint8_t zigbee_get_command_code(zigbee_packet_t packet)
{
    return packet.cmd_code;
}

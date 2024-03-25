#include "zigbee.h"

#include <string.h>
static uint8_t answer_broken_cmd[]               = {0x55, 0x03, 0xFF, 0xFF, 0x00};
static uint8_t answer_verification_not_correct[] = {0x55, 0x03, 0xFF, 0xFE, 0x01};

static uint8_t zigbee_get_cmd_anwer_state(zigbee_packet_t packet);
static uint8_t zigbee_get_command_type(zigbee_packet_t packet);
static uint8_t zigbee_get_command_code(zigbee_packet_t packet);

static void parse_status_packet(zigbee_driver_t *zigbee);
static void parse_data_packet(zigbee_driver_t *zigbee);
static void parse_nwk_st_change_packet(zigbee_driver_t *zigbee);
static void parse_detect_connection_packet(zigbee_driver_t *zigbee);

static int8_t zigbee_get_devices_index(zigbee_driver_t *zigbee, uint8_t *mac_addr);

void parse_input_packet(zigbee_driver_t *zigbee)
{
    // is async response
    char str[200];
    zigbee_packet_t temp_packet;

    uint8_t cmd_type = 0xff;
    uint8_t cmd_code = 0xff;
    memcpy(&temp_packet, &zigbee->input_packet, sizeof(zigbee_packet_t));
    cmd_type = zigbee_get_command_type(temp_packet);
    cmd_code = zigbee_get_command_code(temp_packet);
    if ((cmd_code == NOTIFY_NET_STATUS) && (cmd_type == SYSTEM_NOTYFY_COMMAND_TYPE)) {
#ifdef COORDINATOR
        sprintf(str, "Network info received\r\n");
        CDC_Transmit_FS((unsigned char *)str, strlen(str));
#endif

        parse_nwk_st_change_packet(zigbee);
    }
    if ((cmd_code == NOTIFY_NODE_JOIN) && (cmd_type == SYSTEM_NOTYFY_COMMAND_TYPE)) {
    	parse_detect_connection_packet(zigbee);
#ifdef COORDINATOR
        sprintf(str, "Device connected\r\n");
        CDC_Transmit_FS((unsigned char *)str, strlen(str));
#endif
    }

    if ((cmd_code == NOTIFY_NODE_ADDR) && (cmd_type == SYSTEM_NOTYFY_COMMAND_TYPE)) {
    	//parse_detect_connection_packet(zigbee);
#ifdef COORDINATOR
        //sprintf(str, "Device connected\r\n");
        //CDC_Transmit_FS((unsigned char *)str, strlen(str));
#endif
    }

    if ((cmd_code == ZCL_CMD) && (cmd_type == TYPE_ZCL_IND)) {
    	parse_data_packet(zigbee);
    }

}

void parse_answer_packet(zigbee_driver_t *zigbee)
{
    zigbee_packet_t temp_packet;

    uint8_t cmd_type;
    uint8_t cmd_code;
    uint8_t answer_state;
    memcpy(&temp_packet, &zigbee->input_packet, sizeof(zigbee_packet_t));

    if (strcmp((const char *)&temp_packet, (const char *)&answer_broken_cmd) == 0) {
        zigbee->answer_state = error;
        zigbee->state        = received;
        return;
    }

    if (strcmp((const char *)&temp_packet, (const char *)&answer_verification_not_correct) == 0) {
        zigbee->answer_state = error;
        zigbee->state        = received;
        return;
    }

    cmd_type = zigbee_get_command_type(temp_packet);
    cmd_code = zigbee_get_command_code(temp_packet);

    if ((zigbee->sended_cmd_type != cmd_type) || (zigbee->sended_cmd_code != cmd_code)) {
        // async response
        parse_input_packet(zigbee);
    }

    if ((zigbee->state == wait_answer) && (zigbee->sended_cmd_type == cmd_type) &&
        (zigbee->sended_cmd_code == cmd_code)) {
        switch (cmd_code) {
        case CFG_STATUS:
            parse_status_packet(zigbee);
            zigbee->answer_state = ok;
            break;

        case CFG_START:
            answer_state = zigbee_get_cmd_anwer_state(temp_packet);
            if (answer_state == 0)
                zigbee->answer_state = ok;
            else
                zigbee->answer_state = error;
            break;

        case CFG_OPEN_NET:
            answer_state = zigbee_get_cmd_anwer_state(temp_packet);
            if (answer_state == 0)
                zigbee->answer_state = ok;
            else
                zigbee->answer_state = error;
            break;
        case CFG_CLOSE_NET:
            answer_state = zigbee_get_cmd_anwer_state(temp_packet);
            if (answer_state == 0)
                zigbee->answer_state = ok;
            else
                zigbee->answer_state = error;
            break;

        case CFG_NODE_TYPE:
            answer_state = zigbee_get_cmd_anwer_state(temp_packet);
            if (answer_state == 0)
                zigbee->answer_state = ok;
            else
                zigbee->answer_state = error;
            break;
        }
        zigbee->state = received;
        return;
    }
}

static void parse_status_packet(zigbee_driver_t *zigbee)
{
    zigbee->network_state = zigbee->input_packet.payload[0];
    zigbee->zigbee_mode = zigbee->input_packet.payload[1];
    memcpy(&zigbee->mac_addr, &zigbee->input_packet.payload[2], sizeof(zigbee->mac_addr));
    if (zigbee->network_state == 0xff)
        return;

    zigbee->channel = zigbee->input_packet.payload[10];
    memcpy(&zigbee->pan_id, &zigbee->input_packet.payload[11], sizeof(zigbee->pan_id));
    memcpy(&zigbee->short_addr, &zigbee->input_packet.payload[13], sizeof(zigbee->short_addr));
    memcpy(&zigbee->expand_pan_id, &zigbee->input_packet.payload[15], sizeof(zigbee->expand_pan_id));
    memcpy(&zigbee->network_key, &zigbee->input_packet.payload[23], sizeof(zigbee->network_key));
}

static void parse_data_packet(zigbee_driver_t *zigbee)
{
	int8_t data_length = zigbee->input_packet.frame_length - 15;
	if (data_length < 0)
		return;

	memcpy(zigbee->received_data, &zigbee->input_packet.payload[12], data_length);
	zigbee->received_data_size = data_length;
}


static void parse_nwk_st_change_packet(zigbee_driver_t *zigbee)
{
    zigbee->network_state = zigbee->input_packet.payload[0];

    switch (zigbee->input_packet.payload[0]) {
    case 0:
    	zigbee->network_state = not_networked;
    	break;
    case 1:
    	zigbee->network_state = networked;
    	break;
    case 2:
    	zigbee->network_state = networking;
    	break;
    }

    memcpy(&zigbee->mac_addr, &zigbee->input_packet.payload[1], sizeof(zigbee->mac_addr));
    zigbee->channel = zigbee->input_packet.payload[9];

    if (zigbee->network_state == 0xff)
        return;

    memcpy(&zigbee->pan_id, &zigbee->input_packet.payload[10], sizeof(zigbee->pan_id));
    memcpy(&zigbee->short_addr, &zigbee->input_packet.payload[12], sizeof(zigbee->short_addr));
    memcpy(&zigbee->expand_pan_id, &zigbee->input_packet.payload[14], sizeof(zigbee->expand_pan_id));
    memcpy(&zigbee->network_key, &zigbee->input_packet.payload[16], sizeof(zigbee->network_key));
}

static void parse_detect_connection_packet(zigbee_driver_t *zigbee)
{
#ifdef COORDINATOR
	int8_t index = 0;
    zigbee_device_info_t connected_device;
    memcpy(&connected_device.mac_addr, &zigbee->input_packet.payload[0], sizeof(connected_device.mac_addr));
    memcpy(&connected_device.short_addr, &zigbee->input_packet.payload[8], sizeof(connected_device.short_addr));
    memcpy(&connected_device.short_addr_for_parent, &zigbee->input_packet.payload[10],
           sizeof(connected_device.short_addr_for_parent));
    connected_device.access_type = zigbee->input_packet.payload[12];
    index = zigbee_get_devices_index(zigbee, (uint8_t *)&connected_device.mac_addr);

    if (index < 0) {
    	memcpy(&zigbee->devices[zigbee->devices_count], &connected_device, sizeof(connected_device));
    	zigbee->devices_count++;
    }

#endif

}

static int8_t zigbee_get_devices_index(zigbee_driver_t *zigbee, uint8_t *mac_addr)
{
    int8_t index = -1;
#ifdef COORDINATOR
    for (int i = 0; i < zigbee->devices_count; i++) {
        if (strcmp((const char *)mac_addr, (const char *)zigbee->devices[i].mac_addr) == 0) {
            index = i;
            return index;
        }
    }
#endif
    return index;

}

static uint8_t zigbee_get_cmd_anwer_state(zigbee_packet_t packet)
{
    return packet.payload[0];
}

static uint8_t zigbee_get_command_type(zigbee_packet_t packet)
{
    return packet.cmd_type;
}

static uint8_t zigbee_get_command_code(zigbee_packet_t packet)
{
    return packet.cmd_code;
}

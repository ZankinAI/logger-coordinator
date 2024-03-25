#include "zigbee.h"

#include <string.h>

#include "main.h"
#include "uart_driver/uart_driver.h"
#ifdef COORDINATOR
#include "usb_device.h"
#endif
zigbee_driver_t zigbee_driver;
extern uart_handle_t zigbee_usart;

static bool zigbee_wait_answer(zigbee_driver_t *zigbee);
static void zigbee_print_info(zigbee_driver_t *zigbee);

void init_zigbee(zigbee_driver_t *zigbee, uint8_t mode)
{
    zigbee->zigbee_mode   = mode;
    zigbee->state         = idle;
    zigbee->network_state = not_networked;
#ifdef COORDINATOR
    zigbee->devices_count = 0;
#endif
}

void zigbee_uart_process(zigbee_driver_t *zigbee, uart_handle_t *uart)
{
    uart_packet_t temp_packet;
    if (ring_get_packets_count(&uart->input_ring_buffer) > 0) {
        ring_get_last_and_clear(&uart->input_ring_buffer, &temp_packet);
        set_input_packet(zigbee, temp_packet.packet, temp_packet.length);
#ifdef COORDINATOR
        /*char str[300];
        char packet[200];
        for (int i = 0; i < temp_packet.length; i++)
        	sprintf(packet + i * 2, "%02x", temp_packet.packet[i]);
        sprintf(str, "Received: %s\r\n", packet);
        CDC_Transmit_FS((unsigned char *)str, strlen(str));*/
#endif

    }
}

void zigbee_process(zigbee_driver_t *zigbee)
{
	char str[50];
	static uint8_t last_dev_count = 0;
	uint16_t connected_short_addr = 0xFFFF;
	if (last_dev_count != zigbee->devices_count) {

		memcpy((uint8_t *)&connected_short_addr, zigbee->devices[zigbee->devices_count - 1].short_addr, 2);
        sprintf(str, "Device's short addr: %d\r\n", connected_short_addr);
        CDC_Transmit_FS((unsigned char *)str, strlen(str));
        last_dev_count = zigbee->devices_count;
		/*cmd_set_target_short_addr(zigbee, 0x0000);
		zigbee_wait_answer(zigbee);*/

	}

}

void zigbee_setting_coordinator(zigbee_driver_t *zigbee)
{
#ifdef COORDINATOR
    char str[200];
    sprintf(str, "Reading parameters\r\n");
    CDC_Transmit_FS((unsigned char *)str, strlen(str));
#endif
    cmd_read_params(zigbee);
    zigbee_wait_answer(zigbee);
    zigbee_print_info(zigbee);

    if (zigbee->network_state == networked)
        return;

    HAL_Delay(50);
    sprintf(str, "Reset network\r\n");
    CDC_Transmit_FS((unsigned char *)str, strlen(str));
    cmd_reset(zigbee);
    if (zigbee_wait_answer(zigbee) == false)
        return;


   HAL_Delay(50);
#ifdef COORDINATOR
    sprintf(str, "Config module as coordinator\r\n");
    CDC_Transmit_FS((unsigned char *)str, strlen(str));
#endif
    if (zigbee->zigbee_mode != coordinator) {
        cmd_config_module(zigbee, coordinator);
        if (zigbee_wait_answer(zigbee) == false)
            return;
    }


    HAL_Delay(50);
#ifdef COORDINATOR
    sprintf(str, "Starting network\r\n");
    CDC_Transmit_FS((unsigned char *)str, strlen(str));
#endif
    cmd_connect_to_nwk(zigbee);
    if (zigbee_wait_answer(zigbee) == false)
        return;
}

void zigbee_setting_end_point(zigbee_driver_t *zigbee)
{
    cmd_read_params(zigbee);
    if (zigbee_wait_answer(zigbee) == false)
        return;

    if (zigbee->zigbee_mode == coordinator) {
        cmd_reset(zigbee);
        if (zigbee_wait_answer(zigbee) == false)
            return;
    }

    cmd_config_module(zigbee, end_point);
    if (zigbee_wait_answer(zigbee) == false)
        return;

    cmd_connect_to_nwk(zigbee);
    if (zigbee_wait_answer(zigbee) == false)
        return;
}

void zigbee_autosend(zigbee_driver_t *zigbee)
{
	uint8_t mas[] = {1, 2, 3, 4, 5};
    static uint32_t ms_period = 500;
    static uint32_t last_tick = 0;
    if (HAL_GetTick() - last_tick >= ms_period){
    	last_tick += ms_period;
    	cmd_send_data(zigbee, 0x0000, mas, 5);
    }
}

static bool zigbee_wait_answer(zigbee_driver_t *zigbee)
{
    char str[200];
    uint32_t tickstart = HAL_GetTick();
    uint32_t curr_tick = HAL_GetTick();
    while (curr_tick - tickstart < ZIGBEE_WAIT_ANSWER_TIME) {
        curr_tick = HAL_GetTick();
        zigbee_uart_process(zigbee, &zigbee_usart);
        if (zigbee->state == received)
            break;
    }

    if ((zigbee->answer_state == ok) && (zigbee->state == received)) {
#ifdef COORDINATOR
        sprintf(str, "Success command!\r\n");
        CDC_Transmit_FS((unsigned char *)str, strlen(str));
#endif
        return true;
    } else {
#ifdef COORDINATOR
        sprintf(str, "Error command!\r\n");
        CDC_Transmit_FS((unsigned char *)str, strlen(str));
#endif
        return false;
    }
}

static void zigbee_print_info(zigbee_driver_t *zigbee)
{
#ifdef COORDINATOR
    char zigbee_state[200];
    char *net_state;
    char *mode;
    char *short_addr = (char *)malloc(4 + 1);;
    char *pan_id = (char *)malloc(4 + 1);
    char *mac    = (char *)malloc(2 * 8 + 1);
    switch (zigbee->network_state) {
    case not_networked:
        net_state = "not networked";
        break;
    case networked:
        net_state = "networked";
        break;
    }

    switch (zigbee->zigbee_mode) {
    case coordinator:
        mode = "coordinator";
        break;
    case end_point:
        mode = "end_point";
        break;
    case router:
        mode = "router";
        break;
    }
    if (zigbee->network_state == networked) {
        for (int i = 0; i < 2; i++)
            sprintf(pan_id + i * 2, "%02x", zigbee->pan_id[i]);
        for (int i = 0; i < 2; i++)
            sprintf(short_addr + i * 2, "%02x", zigbee->short_addr[i]);
        for (int i = 0; i < 8; i++)
            sprintf(mac + i * 2, "%02x", zigbee->mac_addr[i]);

        sprintf(zigbee_state, "Network: %s. Module as: %s. Short addr: %s PANID: %s. MAC: %s.\r\n", net_state, mode, short_addr, pan_id, mac);
    } else
        sprintf(zigbee_state, "Network: %s. Module as: %s. ", net_state, mode);

    CDC_Transmit_FS((unsigned char *)zigbee_state, strlen(zigbee_state));
    free(pan_id);
    free(mac);
    free(short_addr);
#endif
}

void set_input_packet(zigbee_driver_t *zigbee, uint8_t *data, uint16_t length)
{
    if ((length > ZIGBEE_MAX_PAYLOAD_SIZE + 2) || (length < 3))
        return;

    if (data[1] != length - 2)
        return;

    if (data[0] != HEADER)
        return;

    uint8_t hash = xor8(&data[2], data[1] - 1); // last byte of payload is hash

    if (hash != data[data[1] + 1])
        return;

    memcpy((uint8_t *)&zigbee->input_packet, data, length);

    if (zigbee->state == wait_answer)
        parse_answer_packet(zigbee);
    else
        parse_input_packet(zigbee);
}

uint8_t xor8(uint8_t *s, uint16_t length)
{
    uint8_t hash   = 0;
    uint16_t index = 0;

    while (index < length) {
        hash = hash ^ s[index];
        index++;
    }
    return hash;
}

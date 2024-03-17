#include "zigbee.h"

#include <string.h>

#include "main.h"
#include "uart_driver/uart_driver.h"

zigbee_driver_t zigbee_driver;

void init_zigbee(zigbee_driver_t *zigbee, uint8_t mode)
{
    zigbee->zigbee_mode = mode;
    zigbee->state = idle;
}

void zigbee_uart_process(zigbee_driver_t *zigbee, uart_handle_t *uart)
{
    uart_packet_t temp_packet;
    if (ring_get_packets_count(&uart->input_ring_buffer) > 0) {
        ring_get_last_and_clear(&uart->input_ring_buffer, &temp_packet);
        set_input_packet(zigbee, temp_packet.packet, temp_packet.length);
    }
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

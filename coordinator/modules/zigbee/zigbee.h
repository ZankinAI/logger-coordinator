#ifndef ZIGBEE_H_
#define ZIGBEE_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_OUTPUT_PACKET_SIZE 100
#define MAX_INPUT_PACKET_SIZE  100


typedef void (*zigbee_callback)(uint8_t *data, uint16_t length);

typedef struct {
    uint8_t packet[MAX_OUTPUT_PACKET_SIZE];
    uint16_t length;
} zigbee_packet_t;



typedef struct {
    bool network_state;
    uint8_t zigbee_mode;
    uint8_t state;
    zigbee_callback callback;
    zigbee_packet_t input_packet;
    zigbee_packet_t output_packet;
} zigbee_driver_t;

void init_zigbee(zigbee_driver_t *zigbee, zigbee_callback callback);
void set_input_packet(uint8_t *data, uint16_t length);
void cmd_read_params(zigbee_driver_t *zigbee);

#endif /* ZIGBEE_H_ */

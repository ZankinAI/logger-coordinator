#ifndef ZIGBEE_H_
#define ZIGBEE_H_

#include <stdint.h>
#include <stdbool.h>

#define ZIGBEE_MAX_PAYLOAD_SIZE 255
#define MAX_INPUT_PACKET_SIZE  100


#define HEADER                    0x55

#define LOCAL_CONFIG_COMMAND_TYPE 0x00

/*   Local configuration commands    */
#define CFG_STATUS                0x00
#define CFG_START                 0x01
#define CFG_OPEN_NET              0x02
#define CFG_CLOSE_NET             0x03
#define CFG_RESET                 0x04
#define CFG_NODE_TYPE             0x05
#define CFG_CHANNEL               0x06
#define CFG_GET_PANID             0x07
#define CFG_SET_PANID             0x08
#define CFG_ADD_GROUP             0x0A
#define CFG_REMOVE_GROUP          0x0B
#define CFG_READ_ATTR             0x10
#define CFG_WRITE_ATTR            0x11
#define CFG_GET_BIND              0x12
#define CFG_SET_BIND              0x13
#define CFG_FIND_BIND             0x14


typedef enum {
	end_point,
	router,
	coordinator
}zigbee_mode_t;

typedef enum {
	idle,
	wait_answer,
	received
}zigbee_driver_state_t;

typedef enum {
	ok,
	error
}zigbee_answer_state_t;

#pragma pack(push,1)
typedef struct {
	uint8_t frame_header;
	uint8_t frame_length;  //cmd_type + cmd_code + payload_size + check_code
	uint8_t cmd_type;
	uint8_t cmd_code;
    uint8_t payload[ZIGBEE_MAX_PAYLOAD_SIZE]; //payload + check_code
} zigbee_packet_t;
#pragma pack(pop)


typedef struct {
    bool network_state;
    zigbee_mode_t zigbee_mode;
    uint8_t state;
    zigbee_packet_t input_packet;
    zigbee_packet_t output_packet;
    uint8_t sended_cmd_type;
    uint8_t sended_cmd_code;
    uint8_t answer_state;

} zigbee_driver_t;

void init_zigbee(zigbee_driver_t *zigbee, uint8_t mode);
void set_input_packet(zigbee_driver_t *zigbee, uint8_t *data, uint16_t length);
void cmd_read_params(zigbee_driver_t *zigbee);
uint8_t xor8(uint8_t *s, uint16_t length);

#endif /* ZIGBEE_H_ */

#ifndef ZIGBEE_H_
#define ZIGBEE_H_

#include <stdint.h>
#include <stdbool.h>

#include "uart_driver/uart_driver.h"

#define COORDINATOR

#define ZIGBEE_WAIT_ANSWER_TIME    2000 // ms
#define ZIGBEE_MAX_PAYLOAD_SIZE    255
#define MAX_INPUT_PACKET_SIZE      100

#define MAX_CONNECTED_DEVICES      5

#define HEADER                     0x55

#define LOCAL_CONFIG_COMMAND_TYPE  0x00

#define SYSTEM_NOTYFY_COMMAND_TYPE 0x80

#define TYPE_ZCL_IND 0x82

/*   Local configuration commands    */
#define CFG_STATUS                 0x00
#define CFG_START                  0x01
#define CFG_OPEN_NET               0x02
#define CFG_CLOSE_NET              0x03
#define CFG_RESET                  0x04
#define CFG_NODE_TYPE              0x05
#define CFG_CHANNEL                0x06
#define CFG_GET_PANID              0x07
#define CFG_SET_PANID              0x08
#define CFG_ADD_GROUP              0x0A
#define CFG_REMOVE_GROUP           0x0B
#define CFG_READ_ATTR              0x10
#define CFG_WRITE_ATTR             0x11
#define CFG_GET_BIND               0x12
#define CFG_SET_BIND               0x13
#define CFG_FIND_BIND              0x14

/*   System notify commands    */

#define NOTIFY_BOOT                0x00
#define NOTIFY_NET_STATUS          0x01
#define NOTIFY_NET_OPEN            0x02
#define NOTIFY_NODE_JOIN           0x03
#define NOTIFY_NODE_ADDR           0x04
#define NOTIFY_DEVICE_JOIN         0x05
#define NOTIFY_LEAVE               0x06
#define NOTIFY_FIND_BIND           0x10
#define NOTIFY_IDENTIFY            0x11

/*   ZCL sends commands    */

#define ZCL_CMD                    0x0F

typedef enum { coordinator, router, end_point } zigbee_mode_t;

typedef enum { idle, wait_answer, received } zigbee_driver_state_t;

typedef enum { ok, error } zigbee_answer_state_t;

typedef enum { networked, networking, not_networked = 0xff } zigbee_network_state_t;

#pragma pack(push, 1)
typedef struct {
    uint8_t frame_header;
    uint8_t frame_length; // cmd_type + cmd_code + payload_size + check_code
    uint8_t cmd_type;
    uint8_t cmd_code;
    uint8_t payload[ZIGBEE_MAX_PAYLOAD_SIZE]; // payload + check_code
} zigbee_packet_t;
#pragma pack(pop)

#ifdef COORDINATOR
typedef struct {
    uint8_t mac_addr[8];
    uint8_t short_addr[2];
    uint8_t short_addr_for_parent[2];
    uint8_t access_type;
    uint8_t device_type;
} zigbee_device_info_t;
#endif

typedef struct {
    bool is_started;
    zigbee_mode_t zigbee_mode;
    zigbee_driver_state_t state;
    zigbee_packet_t input_packet;
    zigbee_packet_t output_packet;

    uint8_t sended_cmd_type;
    uint8_t sended_cmd_code;
    zigbee_answer_state_t answer_state;

    zigbee_network_state_t network_state;
    uint8_t mac_addr[8];
    uint8_t channel;
    uint8_t pan_id[2];
    uint8_t short_addr[2];
    uint8_t expand_pan_id[8];
    uint8_t network_key[16];
#ifdef COORDINATOR
    uint8_t devices_count;
    zigbee_device_info_t devices[MAX_CONNECTED_DEVICES];

    uint8_t received_data_size;
    uint8_t received_data[ZIGBEE_MAX_PAYLOAD_SIZE];
#endif
} zigbee_driver_t;

void init_zigbee(zigbee_driver_t *zigbee, uint8_t mode);
void set_input_packet(zigbee_driver_t *zigbee, uint8_t *data, uint16_t length);
void zigbee_setting_coordinator(zigbee_driver_t *zigbee);
void cmd_read_params(zigbee_driver_t *zigbee);
void zigbee_uart_process(zigbee_driver_t *zigbee, uart_handle_t *uart);
void zigbee_setting_end_point(zigbee_driver_t *zigbee);
uint8_t xor8(uint8_t *s, uint16_t length);

void cmd_config_module(zigbee_driver_t *zigbee, uint8_t mode);

void cmd_send_data(zigbee_driver_t *zigbee, uint16_t short_addr, uint8_t *data, uint8_t data_length);

void parse_input_packet(zigbee_driver_t *zigbee);
void parse_answer_packet(zigbee_driver_t *zigbee);

#endif /* ZIGBEE_H_ */

#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

#include <stdbool.h>

#include "stm32f1xx_hal.h"

#include "ring_buffer\ring_buffer.h"


#define UART_BUF_SIZE 256
#define UART_TIMEOUT 50


typedef struct {
	uint8_t packet[UART_BUF_SIZE];
	uint8_t length;
} uart_packet_t;

typedef struct {
	UART_HandleTypeDef *huart;
	uint8_t buffer_tx[UART_BUF_SIZE];
	uart_packet_t buffer_rx;
	uint16_t rx_counter;
} uart_hw_t;

typedef struct {
	uart_hw_t uart_hw;
	bool use_dma;
	uart_packet_t input_packets[5];
	ring_buffer_t input_ring_buffer;
} uart_handle_t;

void init_uart(uart_handle_t *uart_handle, UART_HandleTypeDef *huart);
void uart_rx_callback(UART_HandleTypeDef *huart, uint16_t size);
void uart_tx(uart_handle_t *huart, const uint8_t *data, uint16_t size);
void start_rx(uart_handle_t *huart);
void usart_timers_handle();

#endif /* UART_DRIVER_H_ */

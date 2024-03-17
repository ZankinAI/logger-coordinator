#include <string.h>

#include "uart_driver.h"

#define UART_DEVICES_COUNT 1

extern UART_HandleTypeDef huart1;

static uart_handle_t *uart_devices[UART_DEVICES_COUNT];
static uint8_t uart_count = 0;

static bool usart_rxne(uart_handle_t *uart_handle);
static uint8_t usart_receive_byte(uart_handle_t *uart_handle);
static void usart_timer(uart_handle_t *huart);
static void usart_timer_callback(uart_handle_t *huart);

void init_uart(uart_handle_t *uart_handle, UART_HandleTypeDef *huart)
{
    int uart_id = -1;
    for (int i = 0; i < uart_count; i++)
        if (uart_devices[i]->uart_hw.huart == huart)
            uart_id = i;

    uart_handle->uart_hw.huart = huart;
    uart_handle->use_dma       = false;

    init_ring_buffer_t init_ring_buf;
    init_ring_buf.buffer_capasity = sizeof(uart_handle->input_packets);
    init_ring_buf.buffer          = (uint8_t *)uart_handle->input_packets;
    init_ring_buf.data_size       = sizeof(uart_packet_t);
    init_ring(&uart_handle->input_ring_buffer, &init_ring_buf);

    if (uart_id >= 0)
        uart_devices[uart_id] = uart_handle;
    else
        uart_devices[uart_count++] = uart_handle;
}

void uart_rx_callback(UART_HandleTypeDef *huart, uint16_t size)
{
    int uart_id = -1;
    for (int i = 0; i < uart_count; i++)
        if (uart_devices[i]->uart_hw.huart == huart)
            uart_id = i;

    if ((uart_id < 0) || (uart_id >= uart_count))
        return;
    uart_handle_t *uart = uart_devices[uart_id];
    uart_packet_t temp_packet;
    temp_packet.length = size;

    if (uart->use_dma) {
        memcpy(temp_packet.packet, uart->uart_hw.buffer_rx.packet, size);
        temp_packet.length = size;
        ring_set_to_last_pos(&uart->input_ring_buffer, &temp_packet);
    } else {
    	uart->uart_hw.rx_counter = UART_TIMEOUT;
        while (usart_rxne(uart)) {
        	uart->uart_hw.buffer_rx.packet[uart->uart_hw.buffer_rx.length++] = usart_receive_byte(uart);
        }
    }
    start_rx(uart);
}

static bool usart_rxne(uart_handle_t *uart_handle)
{
    return __HAL_UART_GET_FLAG(uart_handle->uart_hw.huart, UART_FLAG_RXNE) ? true : false;
}

static uint8_t usart_receive_byte(uart_handle_t *uart_handle)
{
    return uart_handle->uart_hw.huart->Instance->DR;
}

static int8_t get_usart_id(UART_HandleTypeDef *huart)
{
    int uart_id = -1;
    for (int i = 0; i < uart_count; i++)
        if (uart_devices[i]->uart_hw.huart == huart)
            uart_id = i;

    return uart_id;
}

void usart_timers_handle()
{
	for (int i = 0; i < uart_count; i++) {
		if (uart_devices[i]->use_dma == false)
			usart_timer(uart_devices[i]);
	}
}

static void usart_timer(uart_handle_t *huart)
{
	huart->uart_hw.rx_counter--;
	if (huart->uart_hw.rx_counter == 0 && huart->uart_hw.buffer_rx.length > 0)
		usart_timer_callback(huart);
}

static void usart_timer_callback(uart_handle_t *huart)
{
	__disable_irq();
	ring_set_to_last_pos(&huart->input_ring_buffer, &huart->uart_hw.buffer_rx);
	huart->uart_hw.buffer_rx.length = 0;
	__enable_irq();
	start_rx(huart);
}

void start_rx(uart_handle_t *huart)
{
    if (huart->use_dma) {
        HAL_UARTEx_ReceiveToIdle_DMA(huart->uart_hw.huart, (uint8_t *)huart->uart_hw.buffer_rx.packet, UART_BUF_SIZE);
        __HAL_DMA_DISABLE_IT(huart->uart_hw.huart->hdmarx, DMA_IT_HT);
    } else {

    	  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    	  __HAL_UART_ENABLE_IT(huart->uart_hw.huart, UART_IT_ERR);

    	  /* Enable the UART Data Register not empty Interrupt */
    	  __HAL_UART_ENABLE_IT(huart->uart_hw.huart, UART_IT_RXNE);
    }
}

void uart_tx(uart_handle_t *huart, const uint8_t *data, uint16_t size)
{
    memcpy(huart->uart_hw.buffer_tx, data, size);
    if (huart->use_dma)
        HAL_UART_Transmit_DMA(huart->uart_hw.huart, huart->uart_hw.buffer_tx, size);
    else
        HAL_UART_Transmit(huart->uart_hw.huart, huart->uart_hw.buffer_tx, size, 500);
}
void usart_irq(uart_handle_t *huart)
{
	uart_packet_t temp_packet;
    while (usart_rxne(huart)) {
        temp_packet.packet[temp_packet.length++] = usart_receive_byte(huart);
    }
}

/**
 * @brief This function handles USART1 global interrupt.
 */
void USART1_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */
    int8_t uart_id = get_usart_id(&huart1);

    if (uart_devices[uart_id]->use_dma == false) {
    	uart_rx_callback(&huart1, 0);
    	return;
    }
    /* USER CODE END USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart1);
    /* USER CODE BEGIN USART1_IRQn 1 */

    /* USER CODE END USART1_IRQn 1 */
}

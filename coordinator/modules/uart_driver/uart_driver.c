#include <string.h>

#include "uart_driver.h"

#define UART_DEVICES_COUNT 7

static uart_handle_t *uart_devices[UART_DEVICES_COUNT];
static uint8_t uart_count = 0;

void init_uart(uart_handle_t *uart_handle, UART_HandleTypeDef *huart)
{
    int uart_id = -1;
    for (int i = 0; i < uart_count; i++)
        if (uart_devices[i]->uart_hw.huart == huart)
            uart_id = i;

    uart_handle->uart_hw.huart = huart;

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

    memcpy(temp_packet.packet, uart->uart_hw.buffer_rx, size);
    temp_packet.length = size;

    ring_set_to_last_pos(&uart->input_ring_buffer, &temp_packet);

    /* start the DMA again */
    HAL_UARTEx_ReceiveToIdle_DMA(uart->uart_hw.huart, (uint8_t *)uart->uart_hw.buffer_rx, UART_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(uart->uart_hw.huart->hdmarx, DMA_IT_HT);
}

void start_rx(uart_handle_t *huart)
{
    HAL_UARTEx_ReceiveToIdle_DMA(huart->uart_hw.huart, (uint8_t *)huart->uart_hw.buffer_rx, UART_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(huart->uart_hw.huart->hdmarx, DMA_IT_HT);
}

void uart_tx(uart_handle_t *huart, const uint8_t *data, uint16_t size)
{
    memcpy(huart->uart_hw.buffer_tx, data, size);
    HAL_UART_Transmit_DMA(huart->uart_hw.huart, huart->uart_hw.buffer_tx, size);
}

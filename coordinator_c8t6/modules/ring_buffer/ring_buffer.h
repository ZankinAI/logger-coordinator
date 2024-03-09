#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <stdint.h>

typedef enum {
	ring_ok,
	ring_buffer_full,
	ring_buffer_empty,
	ring_data_is_null,
}ring_error_t;

typedef struct {
	uint8_t *buffer;
	uint16_t buffer_capasity;
	uint16_t data_size;
	uint8_t size;
	uint16_t start_pos;
	uint16_t end_pos;
}ring_buffer_t;

typedef struct {
	uint8_t *buffer;
	uint16_t buffer_capasity;
	uint16_t data_size;
}init_ring_buffer_t;

void init_ring(ring_buffer_t *ring_buffer, init_ring_buffer_t *init);
ring_error_t ring_set_to_last_pos(ring_buffer_t *ring_buffer, const void *input);
ring_error_t ring_get_last_and_clear(ring_buffer_t *ring_buffer,  void *output);
uint16_t ring_get_packets_count(ring_buffer_t *ring_buffer);
#endif /* RING_BUFFER_H_ */

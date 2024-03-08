#include "ring_buffer.h"

#include <string.h>

void init_ring(ring_buffer_t *ring_buffer, init_ring_buffer_t *init)
{
	ring_buffer->buffer = init->buffer;
	ring_buffer->buffer_capasity = init->buffer_capasity;
	ring_buffer->data_size = init->data_size;
	ring_buffer->size = 0;
	ring_buffer->start_pos = 0;
	ring_buffer->end_pos = 0;
}

ring_error_t ring_set_to_last_pos(ring_buffer_t *ring_buffer, const void *input)
{
	ring_error_t err = ring_ok;
	uint16_t pos = 0;


	if (ring_buffer->size == ring_buffer->buffer_capasity)
		return ring_buffer_full;
	pos = ring_buffer->end_pos;

	memcpy(ring_buffer->buffer + pos, input, ring_buffer->data_size);

	if (ring_buffer->end_pos + ring_buffer->data_size > ring_buffer->buffer_capasity * ring_buffer->data_size)
		ring_buffer->end_pos = 0;
	else ring_buffer->end_pos += ring_buffer->data_size;

	ring_buffer->size++;

	return err;
}

ring_error_t ring_get_last_and_clear(ring_buffer_t *ring_buffer,  void *output)
{
	ring_error_t err = ring_ok;
	uint16_t pos = 0;

	if (ring_buffer->size == 0)
		return ring_buffer_empty;
	if (output == NULL)
		return ring_data_is_null;

	if (ring_buffer->end_pos == 0)
		pos = (ring_buffer->size - 1) * ring_buffer->data_size;
	else
		pos = ring_buffer->end_pos - ring_buffer->data_size;

	memcpy(output, ring_buffer->buffer + pos, ring_buffer->data_size);
	ring_buffer->size--;
	ring_buffer->end_pos = pos;
	return err;
}

uint16_t ring_get_packets_count(ring_buffer_t *ring_buffer)
{
	return ring_buffer->size;
}

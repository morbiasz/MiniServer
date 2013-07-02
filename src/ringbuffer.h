#ifndef RINGBUFFER_H_INCLUDED
#define RINGBUFFER_H_INCLUDED


#include <inttypes.h>

#define BUFF_SIZE 80


void ring_buffer_push(uint8_t dane);
uint8_t ring_buffer_pop(void);
void ring_buffer_init(void);
uint8_t buffer_count(void);
uint8_t is_empty(void);
void decrement_end(void);

#endif // RINGBUFFER_H_INCLUDED

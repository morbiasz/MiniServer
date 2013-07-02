#include "ringbuffer.h"

uint8_t buffer[BUFF_SIZE], is_full = 0;
uint8_t start = 0, end = 0, buff_count = 0;

void ring_buffer_push(uint8_t dane) {

	if (buff_count == BUFF_SIZE)
		return;
	
  // aby nie przekroczyc zakresu indeksow bufora
  end = (end+1) % BUFF_SIZE;

  if (start == end)
  {
    // jezeli najechaly nasiebie start i end to cofamy end do tylu w sensie kolowym
    end = (end!=0) ? (end-1):(BUFF_SIZE-1);
    return;
  }
    buffer[end] = dane;
    buff_count++;
}

uint8_t ring_buffer_pop(void) {
  uint8_t temp=0;

  // wtedy bufer pusty
  if ( (start==end) | (buff_count ==0)) return 0;
	
  start=(start+1) % BUFF_SIZE ;
  temp=buffer[start];
  buffer[start]=0;
  buff_count--;

  return temp;

}

uint8_t buffer_count(void) {

    return buff_count;
}

uint8_t is_empty(void) {

    if(start==end)
        return 1;
    else
        return 0;
}

void decrement_end(void) {

	end = (end > 0) ?(end-1) : 0;
}

void ring_buffer_init(void) {

    uint8_t i;
    for(i=0;i<BUFF_SIZE;i++)
        buffer[i]=0;

}


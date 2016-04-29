
/*
 * Implementation of a first-in first-out (FIFO) buffer
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stdbool.h>
#include <stdbool.h>

#include "delay.h"

#define FIFO_SIZE       1024
#define FIFO_SIZE_BITS  FIFO_SIZE*8

/*
 * A memory area of FIFO_SIZE bytes
 * to asynchronously read from / write to
 */
struct fifo_s
{
    volatile uint32_t index_read;
    volatile uint32_t index_write;
    volatile uint32_t index_write_bit;
    volatile uint8_t  buffer[FIFO_SIZE];
    volatile uint32_t num_available;
};
typedef struct fifo_s fifo_t;

#define fifo_init(fifo) \
{ \
    fifo->index_read      = 0; \
    fifo->index_write     = 0; \
    fifo->index_write_bit = 0; \
    fifo->num_available   = 0; \
}

#define fifo_available(fifo)    ( fifo->num_available > 0 )
#define fifo_full(fifo)         ( fifo->num_available >= FIFO_SIZE )

bool fifo_read(fifo_t* fifo, char *dst);
bool fifo_write(fifo_t* fifo, char *c);

// a special function for WS2811 SPI control: write 5 bits to FIFO
bool fifo_write_5bits(fifo_t* fifo, uint8_t* value_5bits);

#endif

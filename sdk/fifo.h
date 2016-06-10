
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

#include "delay.h"

#define FIFO_SIZE 1024

/*
 * A memory area of FIFO_SIZE bytes
 * to asynchronously read from / write to
 */
struct fifo_s
{
    // the index from which the next byte should be read from the buffer
    volatile uint32_t index_read;

    // the index where the next byte should be written into the buffer
    volatile uint32_t index_write;

    // the actual buffer storing the bytes of the FIFO
    volatile uint8_t  buffer[FIFO_SIZE];

    // the current number of bytes, available for reading from this FIFO
    volatile uint32_t num_available;
};
typedef struct fifo_s fifo_t;

#define fifo_get_length(fifo)           ( fifo->num_available )
#define fifo_get_size(fifo)             FIFO_SIZE
#define fifo_is_empty(fifo)             ( fifo->num_available == 0 )
#define fifo_is_byte_available(fifo)    ( !fifo_is_empty(fifo) )
#define fifo_is_full(fifo)              ( fifo->num_available >= fifo_get_size(fifo) )

void fifo_init(fifo_t *fifo);
bool fifo_read(fifo_t *fifo, char *dst);
bool fifo_write(fifo_t *fifo, char *c);

#endif

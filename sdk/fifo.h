
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
    volatile uint32_t index_read;
    volatile uint32_t index_write;
    volatile uint8_t  buffer[FIFO_SIZE];
    volatile uint32_t num_available;
};
typedef struct fifo_s fifo_t;

#define fifo_init(fifo) \
{ \
    fifo->index_read     = 0; \
    fifo->index_write    = 0; \
    fifo->num_available  = 0; \
}

#define fifo_available(fifo)    ( fifo->num_available > 0 )
#define fifo_full(fifo)         ( fifo->num_available >= FIFO_SIZE )

bool fifo_read(fifo_t *fifo, char *dst);
bool fifo_write(fifo_t *fifo, char *c);

#endif

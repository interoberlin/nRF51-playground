
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

#define FIFO_SIZE 512

/*
 * A memory area of FIFO_SIZE bytes
 * to asynchronously read from / write to
 */
struct fifo_s
{
    uint32_t index_read;
    uint32_t index_write;
    uint8_t  buffer[FIFO_SIZE];
    uint32_t num_available;
};

#define fifo_init(fifo) \
{ \
    fifo->index_read     = 0; \
    fifo->index_write    = 0; \
    fifo->num_available  = 0; \
}

#define fifo_available(fifo)    ( fifo->num_available > 0 )
#define fifo_full(fifo)         ( fifo->num_available >= FIFO_SIZE )

bool fifo_read(struct fifo_s *fifo, char *dst);
bool fifo_write(struct fifo_s *fifo, char *c);

#endif

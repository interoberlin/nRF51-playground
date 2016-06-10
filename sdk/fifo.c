
/*
 * Implementation of a first-in first-out (FIFO) buffer
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#include "fifo.h"

/**
 * Initializes FIFO read/write indices and resets number of available bytes
 */
void fifo_init(fifo_t *fifo)
{
    fifo->index_read     = 0;
    fifo->index_write    = 0;
    fifo->num_available  = 0;
}

/*
 * Read one byte from FIFO
 */
bool fifo_read(fifo_t *fifo, char *dst)
{
    if (fifo_is_empty(fifo))
        return false;

    // read char from FIFO
    *dst = fifo->buffer[fifo->index_read];

    // increment read index
    fifo->index_read = (fifo->index_read + 1) % FIFO_SIZE;

    // decrement number of available bytes
    fifo->num_available--;

    // success
    return true;
}

/*
 * Write one byte to FIFO
 */
bool fifo_write(fifo_t *fifo, char *c)
{
    if (fifo_is_full(fifo))
        return false;

    // write one byte to FIFO
    fifo->buffer[fifo->index_write] = *c;

    // increment write index
    fifo->index_write = (fifo->index_write + 1) % FIFO_SIZE;

    // increment number of available bytes
    fifo->num_available++;

    // success
    return true;
}

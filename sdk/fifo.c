
/*
 * Implementation of a first-in first-out (FIFO) buffer
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#include "fifo.h"

/*
 * Read one byte from FIFO
 */
bool fifo_read(fifo_t *fifo, char *dst)
{
    if (!fifo_available(fifo))
        return false;

    // circular buffer
    fifo->index_read = (fifo->index_read + 1) % FIFO_SIZE;
    // decrement number of available bytes
    fifo->num_available--;
    // read char from FIFO
    *dst = fifo->buffer[fifo->index_read];

    // success
    return true;
}

/*
 * Write one byte to FIFO
 */
bool fifo_write(fifo_t *fifo, char *c)
{
    if (fifo_full(fifo))
        return false;

    // circular buffer
    fifo->index_write = (fifo->index_write + 1) % FIFO_SIZE;
    // increment number of available bytes
    fifo->num_available++;
    // write one byte to FIFO
    fifo->buffer[fifo->index_write] = *c;

    // success
    return true;
}

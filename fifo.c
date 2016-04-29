
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
bool fifo_read(struct fifo_s *fifo, char *dst)
{
    if (!fifo_available(fifo))
        return false;

    // read char from FIFO
    *dst = fifo->buffer[fifo->index_read];
    // decrement number of available bytes
    fifo->num_available--;

    // circular buffer
    fifo->index_read = (fifo->index_read + 1) % FIFO_SIZE;

    // success
    return true;
}

/*
 * Write one byte to FIFO
 */
bool fifo_write(struct fifo_s *fifo, char *c)
{
    if (fifo_full(fifo))
        return false;

    // write one byte to FIFO
    fifo->buffer[fifo->index_write] = *c;
    // increment number of available bytes
    fifo->num_available++;

    // circular buffer
    fifo->index_write = (fifo->index_write + 1) % FIFO_SIZE;

    // success
    return true;
}

/*
 * Write 5 bits to FIFO
 */
bool fifo_write_5bits(fifo_t* fifo, uint8_t* value_5bits)
{
    if (fifo_full(fifo))
        return false;

    // the FIFO byte to alter at first
    uint8_t index_current_byte             = fifo->index_write_bit % 8;

    // how much, do we need to shift the input to fit the bit frame
    uint8_t bits_shift_left                = fifo->index_write_bit - (index_current_byte*8);

    uint8_t value                          = (*value_5bits) & 0x1f;
    uint8_t apply_mask                     = value << bits_shift_left;

    // clear higher bits of current byte
    fifo->buffer[index_current_byte]      &= 0xff >> (8-bits_shift_left);

    // apply bitmask
    fifo->buffer[index_current_byte]      |= apply_mask;


    // the next FIFO byte, that (potentially) must be changed too
    if (bits_shift_left+5 >= 8)
    {
        // increment number of available bytes
        fifo->num_available++;

        uint8_t index_next_byte             = (index_current_byte+1) % FIFO_SIZE;

        uint8_t bits_shift_right            = (bits_shift_left+5) % 8;

        fifo->buffer[index_next_byte]       = value >> bits_shift_right;
    }

    // circular buffer
    fifo->index_write_bit = (fifo->index_write_bit + 5) % FIFO_SIZE_BITS;

    // success
    return true;
}

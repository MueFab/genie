#ifndef _MPEGG_CODECS_UTILS_H
#define _MPEGG_CODECS_UTILS_H

#include <stdio.h>

int mpegg_write_long_to_Buffer(
        unsigned char      *buffer,
        unsigned long       value,
        unsigned char       numBytes) {

    int ec = 0;

    if(buffer   == NULL ||
       numBytes <= 0    ){
        ec = 1; goto error;
    }

    unsigned int i, shiftSz;
    for (i = 0; i < numBytes; i++) {
        shiftSz = (numBytes - 1U - i) << 3U;
        *(buffer + i) = (value >> shiftSz) & 0xFFU;
    }

    error:
    return ec;
}


int mpegg_read_long_from_buffer(
        unsigned char const *buffer,
        unsigned long       *value,
        unsigned char        numBytes) {

    int ec = 0;

    if(buffer   == NULL ||
       numBytes <= 0    ){
        ec = 1; goto error;
    }

    unsigned int shiftSz, i;

    *value = 0;

    for (i = 0; i < numBytes; i++) {
        shiftSz = (numBytes - 1U - i) << 3U;
        *value += (*(buffer + i)) << shiftSz;
    }

    error:
    return ec;
}

#endif // _MPEGG_CODECS_UTILS_H


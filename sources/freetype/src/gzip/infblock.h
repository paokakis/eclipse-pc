/* infblock.h -- header to use infblock.c
 * Copyright (C) 1995-2002 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

#ifndef _INFBLOCK_H
#define _INFBLOCK_H

struct inflate_blocks_state;
typedef struct inflate_blocks_state FAR inflate_blocks_statef;

inflate_blocks_statef * inflate_blocks_new (
    z_streamp z,
    check_func c,               /* check function */
    uInt w);                   /* window size */

//local  int inflate_blocks OF((
//    inflate_blocks_statef *,
//    z_streamp ,
//    int));                      /* initial return code */

void inflate_blocks_reset (
    inflate_blocks_statef *,
    z_streamp ,
    uLongf *);                  /* check value on output */

int inflate_blocks_free (
    inflate_blocks_statef *,
    z_streamp);

#endif /* _INFBLOCK_H */

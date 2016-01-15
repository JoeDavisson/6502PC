/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn, Joe Davisson
 *
 * 65xx by Joe Davisson
 *
 */

#ifndef TABLE_H
#define TABLE_H

enum
{
  OP_NONE,             //            A 

  OP_IMMEDIATE,        // #$10       #

  OP_ADDRESS8,         // $10        zp
  OP_ADDRESS16,        // $1000      a

  OP_INDEXED8_X,       // $10,x      d,x
  OP_INDEXED8_Y,       // $10,y      d,y
  OP_INDEXED16_X,      // $1000,x    a,x
  OP_INDEXED16_Y,      // $1000,y    a,y

  OP_INDIRECT16,       // ($1000)    (a) 
  OP_X_INDIRECT8,      // ($10,x)    (d,x) 
  OP_INDIRECT8_Y,      // ($10),y    (d),y 

  OP_RELATIVE,         // $10        r
};

#endif


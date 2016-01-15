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

#include <stdint.h>

struct _table_opcodes
{
  uint8_t instr;
  uint8_t op;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

enum
{
  M65XX_ADC,
  M65XX_AND,
  M65XX_ASL,
  M65XX_BCC,
  M65XX_BCS,
  M65XX_BEQ,
  M65XX_BIT,
  M65XX_BMI,
  M65XX_BNE,
  M65XX_BPL,
  M65XX_BRK,
  M65XX_BVC,
  M65XX_BVS,
  M65XX_CLC,
  M65XX_CLD,
  M65XX_CLI,
  M65XX_CLV,
  M65XX_CMP,
  M65XX_CPX,
  M65XX_CPY,
  M65XX_DEC,
  M65XX_DEX,
  M65XX_DEY,
  M65XX_EOR,
  M65XX_INC,
  M65XX_INX,
  M65XX_INY,
  M65XX_JMP,
  M65XX_JSR,
  M65XX_LDA,
  M65XX_LDX,
  M65XX_LDY,
  M65XX_LSR,
  M65XX_NOP,
  M65XX_ORA,
  M65XX_PHA,
  M65XX_PHP,
  M65XX_PLA,
  M65XX_PLP,
  M65XX_ROL,
  M65XX_ROR,
  M65XX_RTI,
  M65XX_RTS,
  M65XX_SBC,
  M65XX_SEC,
  M65XX_SED,
  M65XX_SEI,
  M65XX_STA,
  M65XX_STX,
  M65XX_STY,
  M65XX_TAX,
  M65XX_TAY,
  M65XX_TSX,
  M65XX_TXA,
  M65XX_TXS,
  M65XX_TYA,
  M65XX_ERROR
};

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


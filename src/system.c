/*
Copyright (c) 2015 Joe Davisson.

This file is part of 6502PC.

6502PC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

6502PC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with 6502PC; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "func.h"
#include "system.h"
#include "table.h"

#define READ_RAM(a) (sys.mem[a])
#define WRITE_RAM(a, b) (sys.mem[a] = b)

#define REG_A sys.reg_a
#define REG_X sys.reg_x
#define REG_Y sys.reg_y
#define REG_SR sys.reg_sr
#define REG_PC sys.reg_pc
#define REG_SP sys.reg_sp

#define flag_c 0
#define flag_z 1
#define flag_i 2
#define flag_d 3
#define flag_b 4
#define flag_g 5
#define flag_v 6
#define flag_n 7

#define READ_BIT(dst, a) ((dst & (1 << a)) ? 1 : 0)
#define SET_BIT(dst, a) (dst |= (1 << a))
#define CLEAR_BIT(dst, a) (dst &= ~(1 << a))

#define READ_FLAG(a) ((REG_SR & (1 << a)) ? 1 : 0)
#define SET_FLAG(a) (REG_SR |= (1 << a))
#define CLEAR_FLAG(a) (REG_SR &= ~(1 << a))

#define FLAG(condition, flag) if(condition) SET_FLAG(flag); else CLEAR_FLAG(flag)

extern int table_mode[];
static int op_bytes[] = { 1, 2, 2, 3, 2, 2, 3, 3, 3, 2, 2, 2 };
struct _sys sys;

static int system_execute(const int opcode)
{
  if(opcode < 0 || opcode > 0xFF)
  {
    printf("Bad opcode.\n");
    return -1;
  }
  
  const int mode = table_mode[opcode];
  int address = REG_PC + 1;
  const int lo = READ_RAM(address);
  const int hi = READ_RAM((address + 1) & 0xFFFF);
  int indirect;

  switch(mode)
  {
    case OP_NONE:
      break;
    case OP_IMMEDIATE:
      break;
    case OP_ADDRESS8:
      address = lo & 0xFF;
      break;
    case OP_ADDRESS16:
      address = lo + 256 * hi;
      break;
    case OP_INDEXED8_X:
      address = (lo + REG_X) & 0xFF;
      break;
    case OP_INDEXED8_Y:
      address = (lo + REG_Y) & 0xFFFF;
      break;
    case OP_INDEXED16_X:
      address = ((lo + 256 * hi) + REG_X) & 0xFFFF;
      break;
    case OP_INDEXED16_Y:
      address = ((lo + 256 * hi) + REG_Y) & 0xFFFF;
      break;
    case OP_INDIRECT16:
      indirect = (lo + 256 * hi) & 0xFFFF;
      address = (READ_RAM(indirect) +
                  256 * READ_RAM((indirect + 1) & 0xFFFF)) & 0xFFFF;
      break;
    case OP_X_INDIRECT8:
      indirect = ((READ_RAM(lo) + REG_X) & 0xFF) +
                   256 * READ_RAM((lo + 1) & 0xFF);
      address = (indirect) & 0xFFFF;
      break;
    case OP_INDIRECT8_Y:
      indirect = READ_RAM(lo) + 256 * READ_RAM((lo + 1) & 0xFF);
      address = (indirect + REG_Y) & 0xFFFF;
      break;
    case OP_RELATIVE:
      address = (address + ((signed char)READ_RAM(address) + 1)) & 0xFFFF;
      break;
  }

  int m = READ_RAM(address);
  int temp;
  int pc_lo, pc_hi;
  const int temp_a = REG_A;

  switch(opcode)
  {
    // ADC
    case 0x61:
    case 0x65:
    case 0x69:
    case 0x6D:
    case 0x71:
    case 0x75:
    case 0x79:
    case 0x7D:
      if(READ_FLAG(flag_d))
      {
        int bcd_a = (REG_A & 15) + 10 * (REG_A >> 4);
        int bcd_m = (m & 15) + 10 * (m >> 4);
        int result = bcd_a + bcd_m + READ_FLAG(flag_c);

        FLAG(result > 99, flag_c);
        result %= 100;

        REG_A = (result % 10) + ((result / 10) << 4);
      }
      else
      {
        REG_A += m + READ_FLAG(flag_c);

        FLAG(REG_A > 255, flag_c);
      }

      REG_A &= 0xFF;
      FLAG((temp_a ^ REG_A) & (m ^ REG_A) & 0x80, flag_v);
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);

      break;
    // AND
    case 0x21:
    case 0x25:
    case 0x29:
    case 0x2D:
    case 0x31:
    case 0x35:
    case 0x39:
    case 0x3D:
      REG_A &= m;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
    // ASL
    case 0x06:
    case 0x0A:
    case 0x0E:
    case 0x16:
    case 0x1E:
      if(mode == 4)
      {
        FLAG(READ_BIT(REG_A, 7), flag_c);
        REG_A <<= 1;
        REG_A &= 0xFF;
        FLAG(REG_A > 127, flag_n);
        FLAG(REG_A == 0, flag_z);
      }
      else
      {
        FLAG(READ_BIT(m, 7), flag_c);
        m <<= 1;
        m &= 0xFF;
        FLAG(m > 127, flag_n);
        FLAG(m == 0, flag_z);
        WRITE_RAM(address, m);
      }
      break;
    // BCC
    case 0x90:
      if(READ_FLAG(flag_c) == 0)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BCS
    case 0xB0:
      if(READ_FLAG(flag_c) == 1)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BEQ
    case 0xF0:
      if(READ_FLAG(flag_z) == 1)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BMI
    case 0x30:
      if(READ_FLAG(flag_n) == 1)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BNE
    case 0xD0:
      if(READ_FLAG(flag_z) == 0)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BPL
    case 0x10:
      if(READ_FLAG(flag_n) == 0)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BVC
    case 0x50:
      if(READ_FLAG(flag_v) == 0)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BVS
    case 0x70:
      if(READ_FLAG(flag_v) == 1)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BRK
    case 0x00:
      // stop simulation
      printf("Break encountered.\n");
      return -1;
    // BIT
    case 0x24:
    case 0x2C:
      FLAG((REG_A & m) == 0, flag_z);
      FLAG(READ_BIT(m, 6), flag_v);
      FLAG(READ_BIT(m, 7), flag_n);
      break;
    // CLC
    case 0x18:
      CLEAR_FLAG(flag_c);
      break;
    // CLD
    case 0xD8:
      CLEAR_FLAG(flag_d);
      break;
    // CLI
    case 0x58:
      CLEAR_FLAG(flag_i);
      break;
    // CLV
    case 0xB8:
      CLEAR_FLAG(flag_v);
      break;
    // CMP
    case 0xC1:
    case 0xC5:
    case 0xC9:
    case 0xCD:
    case 0xD1:
    case 0xD5:
    case 0xD9:
    case 0xDD:
      temp = (REG_A - m);
      FLAG(temp >= 0, flag_c);
      temp &= 0xFF;
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // CPX
    case 0xE0:
    case 0xE4:
    case 0xEC:
      temp = (REG_X - m);
      FLAG(temp >= 0, flag_c);
      temp &= 0xFF;
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // CPY
    case 0xC0:
    case 0xC4:
    case 0xCC:
      temp = (REG_Y - m);
      FLAG(temp >= 0, flag_c);
      temp &= 0xFF;
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // DEC
    case 0xC6:
    case 0xCE:
    case 0xD6:
    case 0xDE:
      temp = (m - 1) & 0xFF;
      WRITE_RAM(address, temp);
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // DEX
    case 0xCA:
      REG_X = (REG_X - 1) & 0xFF;
      FLAG(REG_X > 127, flag_n);
      FLAG(REG_X == 0, flag_z);
      break;
    // DEY
    case 0x88:
      REG_Y = (REG_Y - 1) & 0xFF;
      FLAG(REG_Y > 127, flag_n);
      FLAG(REG_Y == 0, flag_z);
      break;
    // EOR
    case 0x41:
    case 0x45:
    case 0x49:
    case 0x4D:
    case 0x51:
    case 0x55:
    case 0x59:
    case 0x5D:
      REG_A ^= m;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
    // INC
    case 0xE6:
    case 0xEE:
    case 0xF6:
    case 0xFE:
      temp = (m + 1) & 0xFF;
      WRITE_RAM(address, temp);
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // INX
    case 0xE8:
      REG_X = (REG_X + 1) & 0xFF;
      FLAG(REG_X > 127, flag_n);
      FLAG(REG_X == 0, flag_z);
      break;
    // INY
    case 0xC8:
      REG_Y = (REG_Y + 1) & 0xFF;
      FLAG(REG_X > 127, flag_n);
      FLAG(REG_X == 0, flag_z);
      break;
    // JMP
    case 0x4C:
    case 0x6C:
      REG_PC = address;
      return 1;
    // JSR
    case 0x20:
      WRITE_RAM(0x100 + REG_SP, (REG_PC + 2) / 256);
      REG_SP--; 
      REG_SP &= 0xFF;
      WRITE_RAM(0x100 + REG_SP, (REG_PC + 2) & 0xFF);
      REG_SP--;
      REG_SP &= 0xFF;
      REG_PC = address;
      return 1;
    // LDA
    case 0xA1:
    case 0xA5:
    case 0xA9:
    case 0xAD:
    case 0xB1:
    case 0xB5:
    case 0xB9:
    case 0xBD:
      REG_A = m;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break; 
    // LDX
    case 0xA2:
    case 0xA6:
    case 0xAE:
    case 0xB6:
    case 0xBE:
      REG_X = m;
      FLAG(REG_X == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // LDY
    case 0xA0:
    case 0xA4:
    case 0xAC:
    case 0xB4:
    case 0xBC:
      REG_Y = m;
      FLAG(REG_Y == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // LSR
    case 0x46:
    case 0x4A:
    case 0x4E:
    case 0x56:
    case 0x5E:
      if(mode == 4)
      {
        FLAG(READ_BIT(REG_A, 0), flag_c);
        REG_A >>= 1;
        CLEAR_BIT(REG_A, 7);
        FLAG(REG_A > 127, flag_n);
        FLAG(REG_A == 0, flag_z);
      }
      else
      {
        FLAG(READ_BIT(m, 0), flag_c);
        m >>= 1;
        CLEAR_BIT(m, 7);
        FLAG(m > 127, flag_n);
        FLAG(m == 0, flag_z);
        WRITE_RAM(address, m);
      }
      break;
    // NOP
    case 0xEA:
      break;
    // ORA
    case 0x01:
    case 0x05:
    case 0x09:
    case 0x0D:
    case 0x11:
    case 0x15:
    case 0x19:
    case 0x1D:
      REG_A |= m;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
    // PHA
    case 0x48:
      WRITE_RAM(0x100 + REG_SP, REG_A);
      REG_SP--;
      REG_SP &= 0xFF;
      break;
    // PHP
    case 0x08:
      WRITE_RAM(0x100 + REG_SP, REG_SR);
      REG_SP--;
      REG_SP &= 0xFF;
      break;
    // PLA
    case 0x68:
      REG_SP++;
      REG_SP &= 0xFF;
      REG_A = READ_RAM(0x100 + REG_SP);
      break;
    // PLP
    case 0x28:
      REG_SP++;
      REG_SP &= 0xFF;
      REG_SR = READ_RAM(0x100 + REG_SP);
      break;
    // ROL
    case 0x26:
    case 0x2A:
    case 0x2E:
    case 0x36:
    case 0x3E:
      if(mode == 4)
      {
        FLAG(READ_BIT(REG_A, 7), flag_c);
        REG_A <<= 1;
        SET_BIT(REG_A, READ_FLAG(flag_c));
        REG_A &= 0xFF;
        FLAG(REG_A > 127, flag_n);
        FLAG(REG_A == 0, flag_z);
      }
      else
      {
        FLAG(READ_BIT(m, 7), flag_c);
        m <<= 1;
        SET_BIT(m, READ_FLAG(flag_c));
        m &= 0xFF;
        FLAG(m > 127, flag_n);
        FLAG(m == 0, flag_z);
        WRITE_RAM(address, m);
      }
      break;
    // ROR
    case 0x66:
    case 0x6A:
    case 0x6E:
    case 0x76:
    case 0x7E:
      if(mode == 4)
      {
        temp = READ_BIT(REG_A, 0);
        REG_A >>= 1;
        SET_BIT(REG_A, READ_FLAG(flag_c) << 7);
        FLAG(temp, flag_c);
        FLAG(REG_A > 127, flag_n);
        FLAG(REG_A == 0, flag_z);
      }
      else
      {
        temp = READ_BIT(m, 0);
        m >>= 1;
        SET_BIT(m, READ_FLAG(flag_c) << 7);
        FLAG(temp, flag_c);
        FLAG(m > 127, flag_n);
        FLAG(m == 0, flag_z);
        WRITE_RAM(address, m);
      }
      break;
    // RTI
    case 0x40:
      REG_SP++;
      REG_SP &= 0xFF;
      REG_SR = READ_RAM(0x100 + REG_SP);
      REG_SP++;
      REG_SP &= 0xFF;
      pc_lo = READ_RAM(0x100 + REG_SP);
      REG_SP++;
      REG_SP &= 0xFF;
      pc_hi = READ_RAM(0x100 + REG_SP);
      REG_PC = (pc_lo + 256 * pc_hi);
      REG_PC++;
      return 1;
    // RTS
    case 0x60:
      REG_SP++;
      REG_SP &= 0xFF;
      pc_lo = READ_RAM(0x100 + REG_SP);
      REG_SP++;
      REG_SP &= 0xFF;
      pc_hi = READ_RAM(0x100 + REG_SP);
      REG_PC = (pc_lo + 256 * pc_hi);
      REG_PC++;
      return 1;
    // SBC
    case 0xE1:
    case 0xE5:
    case 0xE9:
    case 0xED:
    case 0xF1:
    case 0xF5:
    case 0xF9:
    case 0xFD:
      if(READ_FLAG(flag_d))
      {
        int bcd_a = (REG_A & 15) + 10 * (REG_A >> 4);
        int bcd_m = (m & 15) + 10 * (m >> 4);
        int result = bcd_a - bcd_m - (1 - READ_FLAG(flag_c));

        // clear carry if < 0
        FLAG(result >= 0, flag_c);
        result %= 100;

        REG_A = (result % 10) + ((result / 10) << 4);
      }
      else
      {
        REG_A -= m - (1 - READ_FLAG(flag_c));

        FLAG(REG_A >= 0, flag_c);
      }

      REG_A &= 0xFF;
      FLAG((temp_a ^ REG_A) & (m ^ REG_A) & 0x80, flag_v);
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);

      break;
    // SEC
    case 0x38:
      SET_FLAG(flag_c);
      break;
    // SED
    case 0xF8:
      SET_FLAG(flag_d);
      break;
    // SEI
    case 0x78:
      SET_FLAG(flag_i);
      break;
    // STA
    case 0x81:
    case 0x85:
    case 0x8D:
    case 0x91:
    case 0x95:
    case 0x99:
    case 0x9D:
      // check for function
      func_check(&sys, address);

      WRITE_RAM(address, REG_A);
      break;
    // STX
    case 0x86:
    case 0x8E:
    case 0x96:
      WRITE_RAM(address, REG_X);
      break;
    // STY
    case 0x84:
    case 0x8C:
    case 0x94:
      WRITE_RAM(address, REG_Y);
      break;
    // TAX
    case 0xAA:
      REG_X = REG_A;
      FLAG(REG_X == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // TAY
    case 0xA8:
      REG_Y = REG_A;
      FLAG(REG_Y == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // TSX
    case 0xBA:
      REG_X = REG_SP;
      FLAG(REG_X == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // TXA
    case 0x8A:
      REG_A = REG_X;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
    // TXS
    case 0x9A:
      REG_SP = REG_X;
      break;
    // TYA
    case 0x98:
      REG_A = REG_Y;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
  }

  return 0;
}

int system_load(char *fn, int address)
{
  if(address < 0 || address > 0xFFFF)
    return -1;

  memset(sys.mem, 0, sizeof(sys.mem));

  FILE *in = fopen(fn, "rb");
  if(!in)
    return -1;

  for(int i = address; i <= 0xFFFF; i++)
  {
    int c = fgetc(in);
    if(c == EOF)
      break;

    sys.mem[i] = c;
  }

  printf("\n");

  fclose(in);

  return 0;
}

void system_run(const int address)
{
  REG_PC = address;

  while(1)
  {
    const int opcode = READ_RAM(REG_PC);
    const int ret = system_execute(opcode);

    if(ret == 0)
    {
      REG_PC += op_bytes[table_mode[opcode]];
      REG_PC &= 0xFFFF;
    }
    else if(ret == -1)
    {
      break;
    }
  }
}


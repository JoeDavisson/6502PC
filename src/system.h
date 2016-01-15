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

#ifndef SYSTEM_H
#define SYSTEM_H

struct _sys
{
  unsigned char mem[65536];
  int reg_a;
  int reg_x;
  int reg_y;
  int reg_sr;
  int reg_pc;
  int reg_sp;
};

int system_load(char *, int);
void system_run(const int);

#endif


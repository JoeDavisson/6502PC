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
#include <getopt.h>

#include "system.h"

enum
{
  OPTION_VERSION,
  OPTION_HELP
};

static int verbose_flag;

struct option long_options[] =
{
  { "version", no_argument,       &verbose_flag, OPTION_VERSION },
  { "help",    no_argument,       &verbose_flag, OPTION_HELP    },
  { 0, 0, 0, 0 }
};
  
int main(int argc, char *argv[])
{
  // parse command line
  int option_index = 0;

  while(1)
  {
    const int c = getopt_long(argc, argv, "", long_options, &option_index);
    if(c < 0)
      break;

    switch(c)
    {
      case 0:
      {
        switch(option_index)
        {
          case OPTION_VERSION:
            break;
   
          case OPTION_HELP:
            break;
        }
      }
      default:
      {
// print help and exit
        printf("Error.\n");
        return 0;
      }
    }
  }

  if(optind < argc)
  {
    if(system_load(argv[optind], 0x1000) < 0)
    {
      printf("Could not load program: %s\n", argv[optind]);
      return 0;
    }
  }
  else
  {
    printf("No filename.\n");
  }

  system_run(0x1000);

  return 0;
}


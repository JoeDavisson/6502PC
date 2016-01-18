# 6502PC Makefile

NAME="6502PC "
VERSION=$(shell git describe --always --dirty)

SRC_DIR=src

ifndef MAKE_HOST
  $(error MAKE_HOST undefined)
endif

ifneq (,$(findstring linux,$(MAKE_HOST)))
  HOST=
  CC=gcc
  CFLAGS=-O3 -DPACKAGE_STRING=\"$(NAME)$(VERSION)\" $(INCLUDE)
  EXE=6502pc
endif

ifneq (,$(findstring mingw,$(MAKE_HOST)))
# mingw
ifneq (,$(findstring i686,$(MAKE_HOST)))
  HOST=i686-w64-mingw32
  CC=$(HOST)-gcc
  CFLAGS=-O3 -static-libgcc -static-libstdc++ -DPACKAGE_STRING=\"$(NAME)$(VERSION)\" $(INCLUDE)
  LIBS+=-lgdi32 -lcomctl32 -lws2_32 -static -lpthread
  EXE=6502pc.exe
endif
ifneq (,$(findstring x86_64,$(MAKE_HOST)))
  HOST=x86_64-w64-mingw32
  CC=$(HOST)-gcc
  CFLAGS=-O3 -static-libgcc -static-libstdc++ -DPACKAGE_STRING=\"$(NAME)$(VERSION)\" $(INCLUDE)
  LIBS+=-lgdi32 -lcomctl32 -lws2_32 -static -lpthread
  EXE=6502pc.exe
endif
endif

ifndef HOST
  $(error Unknown host)
endif

OBJ= \
  $(SRC_DIR)/func.o \
  $(SRC_DIR)/system.o \
  $(SRC_DIR)/table.o

default: $(OBJ)
	$(CC) -o ./$(EXE) $(SRC_DIR)/main.c $(OBJ) $(CFLAGS) $(LIBS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cxx $(SRC_DIR)/%.H
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(SRC_DIR)/*.o 
	@rm -f ./$(EXE)
	@echo "Clean!"


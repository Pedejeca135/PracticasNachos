# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
comma:= ,
empty:=
space:= $(empty) $(empty)

# and change the flags to ld and the build procedure for as
GCCDIR = /usr/local/mips-linux/bin/
LDFLAGS = -T script -N --cref -S -EL
ASFLAGS = -mips1 -EL
CPPFLAGS = $(INCDIR)

CC = $(GCCDIR)gcc
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = /usr/local/lib/gcc-lib/mips-linux/2.7.2.2/cpp
INCDIR = -I../userprog -I../threads
CFLAGS =-ggdb -mcpu=r3000 -mno-mips-tfile $(INCDIR)
#CFLAGS =-ggdb -mcpu=r3000 -mno-abicalls -mno-mips-tfile $(INCDIR)

all: halt shell matmult sort

.c.o:
	$(CC) $(CFLAGS) -S $< -o - | $(AS) $(ASFLAGS) - -o $@

start.o: start.s ../userprog/syscall.h
	$(CPP) -DTEST $(CPPFLAGS) $< -o str.s
	$(AS) $(ASFLAGS) str.s -o $@
	rm str.s

halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt
	
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

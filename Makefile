OBJDIR	= obj
SRCDIR	= src
INCDIR	= include
PROGRAM = mmc_test 
_OBJS	= mmc_test.o mmc.o smf0.o 
OBJS	= $(patsubst %,$(OBJDIR)/%,$(_OBJS))
CC	= gcc
WINDRES	= windres

CFLAGS	= -g -Wall
#LIBS	= `

.SUFFIXIES:	.c .o

all:	$(PROGRAM)

$(PROGRAM)	:$(OBJS)
	$(CC) -o $(PROGRAM) $^ $(LIBS) 

$(OBJDIR)/%.o	:$(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

icon.o	:icon.ico
	echo "1 ICON \"icon.ico\"" | $(WINDRES) -o icon.o

.PHONY:	clean
clean:
	$(RM) $(PROGRAM) $(OBJDIR)/*

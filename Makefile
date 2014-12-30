OBJDIR	= obj
SRCDIR	= src
INCDIR	= include
PROGRAM = midifile_test
_OBJS	= midifile_test.o 
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

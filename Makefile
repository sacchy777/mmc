OBJDIR	= obj
SRCDIR	= src
INCDIR	= include
PROGRAM = mmc_test
_OBJS	= mmc_test.o mmc.o smf0.o lex.o dlog.o
PROGRAM_DLL = mmc.dll
_OBJS_DLL	= mmcdll.o mmc.o smf0.o lex.o dlog.o
OBJS	= $(patsubst %,$(OBJDIR)/%,$(_OBJS))
OBJS_DLL	= $(patsubst %,$(OBJDIR)/%,$(_OBJS_DLL))
CC	= gcc
WINDRES	= windres

CFLAGS	= -g -Wall -I/usr/local/include
LIBS	= -lCunit -L/usr/local/lib

.SUFFIXIES:	.c .o

all:	$(PROGRAM) $(PROGRAM_DLL)
#all:	$(PROGRAM_DLL)
#all:	$(PROGRAM)

$(PROGRAM)	:$(OBJS)
	$(CC) -o $(PROGRAM) $^ $(LIBS) 

$(PROGRAM_DLL)	:$(OBJS_DLL)
	$(CC) $(CFLAGS) -shared -o $(PROGRAM_DLL) $^ -Wl,--out-implib,libmmcdll.a

$(OBJDIR)/%.o	:$(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

icon.o	:icon.ico
	echo "1 ICON \"icon.ico\"" | $(WINDRES) -o icon.o

.PHONY:	clean
clean:
	$(RM) $(PROGRAM) $(PROGRAM_DLL) $(OBJDIR)/*

# Makefile to test how to include root library into the project

TOP	:= $(shell pwd)/
OBJ	:= $(TOP)obj/
BIN	:= $(TOP)bin/
SRC	:= $(TOP)src/
INCLUDE	:= $(TOP)include/

SRCEXT := cpp

CC	 = g++
CPPFLAGS = -O -Wall -fPIC -I$(INCLUDE)

ROOTCFLAGS	= $(shell root-config --cflags)
ROOTLIBS	= $(shell root-config --libs)
ROOTGLIBS	= $(shell root-config --glibs)
CPPFLAGS       += -I$(ROOTCFLAGS)
CPPLIBS		= $(ROOTLIBS) $(ROOTGLIBS)

all: main
main	: $(patsubst $(SRC)%.$(SRCEXT),$(OBJ)%.o,$(wildcard $(SRC)*.$(SRCEXT)))
	$(CC) $^ $(CPPLIBS) -o $(BIN)$(notdir $@)
	@echo

$(OBJ)%.o :	$(SRC)%.$(SRCEXT)
	$(CC) $(CPPFLAGS) -c $(SRC)$(notdir $<) -o $(OBJ)$(notdir $@)
	@echo

.PHONY:clean
clean:
	rm -f $(OBJ)*.o
	rm -f $(BIN)*

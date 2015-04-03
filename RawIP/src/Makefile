# In The Name Of God
# ========================================
# [] File Name : Makefile
#
# [] Creation Date : 31-03-2015
#
# [] Created By : Parham Alvani (parham.alvani@gmail.com)
# =======================================

P=RawIP
OBJ=main.o common.o ip_func.o
LIB=
MLIB=

ifdef LIB
CFLAGS=-Wall -O0 -g -std=gnu11 \
       $(shell pkg-config --cflags $(LIB))
LDLIBS=$(shell pkg-config --libs $(LIB)) $(MLIB)
else
CFLAGS=-Wall -O0 -g -std=gnu11
LDLIBS=$(MLIB)
endif
ASFLAGS=-Wall -O0 -g
CFLAGS+=-DDEBUG

$(P): $(OBJ)
	$(CC) -o $(P) $(OBJ) $(LDLIBS)
main.o: common.h ip_func.h
common.o: common.h
ip_func.o: ip_func.h

.PHONY: clean
clean:
	rm $(OBJ) $(P)

INC=.
SRC=.
OBJ=.
OUT=.

LBDD=/usr/local/lib/libbdd.a

CC=clang
# g++ -I/usr/local/include myfile.cc -o myfile -L/usr/local/lib -lbdd
CFLAGS=-std=c99 -I$(INC) -Wno-initializer-overrides -O3
LDFLAGS=-O3

DEPS=$(INC)/parse_cl.h $(INC)/types.h
OBJS=$(OBJ)/parse_cl.o $(OBJ)/main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUT)/bitbuddy: $(OBJS)
	$(CC) -o $(OUT)/$@ $^ $(LBDD) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o $(OUT)/bitbuddy

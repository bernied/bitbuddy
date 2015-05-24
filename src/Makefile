INC=.
SRC=.
OBJ=.
OUT=.

CC=clang
# g++ -I/usr/local/include myfile.cc -o myfile -L/usr/local/lib -lbdd
CFLAGS=-I$(INC) -Wno-initializer-overrides -O3
LDFLAGS=/usr/local/lib/libbdd.a -O3

DEPS=$(INC)/parse_cl.h $(INC)/types.h
OBJS=$(OBJ)/parse_cl.o $(OBJ)/main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUT)/bitbuddy: $(OBJS)
	$(CC) -o $(OUT)/$@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o $(OUT)/bitbuddy

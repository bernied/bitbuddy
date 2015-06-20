INC=.
SRC=.
OBJ=.
OUT=.
BDD=BUDDY
#BDD=CUDD

LBDD=/usr/local/lib/libbdd.a
#LBDD=/usr/local/lib/libcudd.a /usr/local/lib/libepd.a /usr/local/lib/libutil.a /usr/local/lib/libmtr.a /usr/local/lib/libst.a

CC=clang
CFLAGS=-std=c99 -I/usr/local/include -I$(INC) -D$(BDD) -Wno-initializer-overrides -O3
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

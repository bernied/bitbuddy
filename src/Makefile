INC=.
SRC=.
OBJ=.
OUT=.

LBDD_BUDDY=/usr/local/lib/libbdd.a
LBDD_CUDD=/usr/local/lib/libcudd.a /usr/local/lib/libepd.a /usr/local/lib/libutil.a /usr/local/lib/libmtr.a /usr/local/lib/libst.a

CC=clang
CFLAGS=-std=c99 -I/usr/local/include -I$(INC) -Wno-initializer-overrides -O3
#CFLAGS=-std=c99 -I/usr/local/include -I$(INC) -D$(BDD) -Wno-initializer-overrides -g
LDFLAGS=-O3

DEPS=$(INC)/parse_cl.h $(INC)/types.h
OBJS_BUDDY=$(OBJ)/parse_cl.o $(OBJ)/main_buddy.o $(OBJ)/bdd_buddy.o
OBJS_CUDD=$(OBJ)/parse_cl.o $(OBJ)/main_cudd.o $(OBJ)/bdd_cudd.o

all: $(OUT)/bitbuddy_b $(OUT)/bitbuddy_c

bdd_buddy.o: bdd_buddy.c bdd_buddy.h $(DEPS)
	$(CC) -c -c -DBUDDY -o $@ $< $(CFLAGS)

bdd_cudd.o: bdd_cudd.c bdd_cudd.h $(DEPS)
	$(CC) -c -c -DCUDD -o $@ $< $(CFLAGS)

main_buddy.o: main.c bdd_buddy.h $(DEPS)
	$(CC) -c -DBUDDY -o $@ $< $(CFLAGS)

main_cudd.o: main.c bdd_cudd.h $(DEPS)
	$(CC) -c -DCUDD -o $@ $< $(CFLAGS)

$(OUT)/bitbuddy_b: $(OBJS_BUDDY)
	$(CC) -o $(OUT)/$@ $^ $(LBDD_BUDDY) -DBUDDY $(LDFLAGS)

$(OUT)/bitbuddy_c: $(OBJS_CUDD)
	$(CC) -o $(OUT)/$@ $^ $(LBDD_CUDD) -DCUDD $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o $(OUT)/bitbuddy_*

blif:
	rm -f *.blif

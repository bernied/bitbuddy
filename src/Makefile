INC=.
SRC=.
OBJ=.
OUT=.

LBDD_ABCD=/usr/local/lib/libabcd.a
LBDD_BUDDY=/usr/local/lib/libbdd.a
LBDD_CAL=/usr/local/lib/libcal.a
LBDD_CUDD=/usr/local/lib/libcudd.a /usr/local/lib/libepd.a /usr/local/lib/libutil.a /usr/local/lib/libmtr.a /usr/local/lib/libst.a
LBDD_SDD=/usr/local/lib/libsdd.a
LBDD_SYLVAN=/usr/local/lib/libsylvan.a -lpthread -lm

CC=clang
#CFLAGS=-std=gnu11 -I/usr/local/include -I$(INC) -Wno-initializer-overrides -O3
#LDFLAGS=-O3
CFLAGS=-std=gnu11 -I/usr/local/include -I$(INC) -Wno-initializer-overrides -g
LDFLAGS=-g

DEPS=$(INC)/parse_cl.h $(INC)/types.h
OBJS_ABCD=$(OBJ)/parse_cl.o $(OBJ)/main_abcd.o $(OBJ)/bdd_abcd.o
OBJS_BUDDY=$(OBJ)/parse_cl.o $(OBJ)/main_buddy.o $(OBJ)/bdd_buddy.o
OBJS_CAL=$(OBJ)/parse_cl.o $(OBJ)/main_cal.o $(OBJ)/bdd_cal.o
OBJS_CUDD=$(OBJ)/parse_cl.o $(OBJ)/main_cudd.o $(OBJ)/bdd_cudd.o
OBJS_SYLVAN=$(OBJ)/parse_cl.o $(OBJ)/main_sylvan.o $(OBJ)/bdd_sylvan.o
OBJS_SDD=$(OBJ)/parse_cl.o $(OBJ)/main_sdd.o $(OBJ)/bdd_sdd.o

all: $(OUT)/bitbuddy_a $(OUT)/bitbuddy_b $(OUT)/bitbuddy_c $(OUT)/bitbuddy_d $(OUT)/bitbuddy_l $(OUT)/bitbuddy_s

bdd_abcd.o: bdd_abcd.c bdd_abcd.h $(DEPS)
	$(CC) -c -c -DABCD -o $@ $< $(CFLAGS)

bdd_buddy.o: bdd_buddy.c bdd_buddy.h $(DEPS)
	$(CC) -c -c -DBUDDY -o $@ $< $(CFLAGS)

bdd_cal.o: bdd_cal.c bdd_cal.h $(DEPS)
	$(CC) -c -c -DCAL -o $@ $< $(CFLAGS)

bdd_cudd.o: bdd_cudd.c bdd_cudd.h $(DEPS)
	$(CC) -c -c -DCUDD -o $@ $< $(CFLAGS)

bdd_sdd.o: bdd_sdd.c bdd_sdd.h $(DEPS)
	$(CC) -c -c -DSDD -o $@ $< $(CFLAGS)

bdd_sylvan.o: bdd_sylvan.c bdd_sylvan.h $(DEPS)
	$(CC) -c -c -DSYLVAN -o $@ $< $(CFLAGS)

main_abcd.o: main.c bdd_abcd.h $(DEPS)
	$(CC) -c -DABCD -o $@ $< $(CFLAGS)

main_buddy.o: main.c bdd_buddy.h $(DEPS)
	$(CC) -c -DBUDDY -o $@ $< $(CFLAGS)

main_cal.o: main.c bdd_cal.h $(DEPS)
	$(CC) -c -DCAL -o $@ $< $(CFLAGS)

main_cudd.o: main.c bdd_cudd.h $(DEPS)
	$(CC) -c -DCUDD -o $@ $< $(CFLAGS)

main_sdd.o: main.c bdd_sdd.h $(DEPS)
	$(CC) -c -DSDD -o $@ $< $(CFLAGS)

main_sylvan.o: main.c bdd_sylvan.h $(DEPS)
	$(CC) -c -DSYLVAN -o $@ $< $(CFLAGS)

$(OUT)/bitbuddy_a: $(OBJS_ABCD)
	$(CC) -o $(OUT)/$@ $^ $(LBDD_ABCD) -DABCD $(LDFLAGS)

$(OUT)/bitbuddy_b: $(OBJS_BUDDY)
	$(CC) -o $(OUT)/$@ $^ $(LBDD_BUDDY) -DBUDDY $(LDFLAGS)

$(OUT)/bitbuddy_c: $(OBJS_CUDD)
	$(CC) -o $(OUT)/$@ $^ $(LBDD_CUDD) -DCUDD $(LDFLAGS)

$(OUT)/bitbuddy_d: $(OBJS_SDD)
	$(CC) -o $(OUT)/$@ $^ $(LBDD_SDD) -DSDD $(LDFLAGS)

$(OUT)/bitbuddy_l: $(OBJS_CAL)
	$(CC) -o $(OUT)/$@ $^ $(LBDD_CAL) -DCAL $(LDFLAGS)

$(OUT)/bitbuddy_s: $(OBJS_SYLVAN)
	$(CC) -o $(OUT)/$@ $^ $(LBDD_SYLVAN) -DSYLVAN $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o $(OUT)/bitbuddy_*

blif:
	rm -f *.blif

#ifndef __BDD_BUDDY__
#define __BDD_BUDDY__

#include "bdd.h"

// operations
#define BB_op_type int
#define BB_AND bddop_and
#define BB_OR bddop_or
#define BB_XOR bddop_xor
#define BB_FALSE bddfalse
#define BB_TRUE bddtrue

// main bdd type
#define BB_bdd bdd

// bdd operations
extern BB_bdd BB_addref(BB_bdd bdd);
extern BB_bdd BB_delref(BB_bdd bdd);
extern void BB_setvarnum(int vars);
extern BB_bdd BB_ithvar(int var);
extern int BB_satcount(BB_bdd bdd);
extern void BB_save(BB_bdd bdd, char* name);
extern void BB_done();

#endif

#ifndef __BDD_CUDD__
#define __BDD_CUDD__

#include "util.h"
#include "cudd.h"

// operations
#define BB_op_type int
#define BB_AND 0
#define BB_OR 1
#define BB_XOR 2
#define BB_FALSE Cudd_ReadZero(manager)
#define BB_TRUE Cudd_ReadOne(manager)

// main bdd type
typedef DdNode* BB_bdd;

// bdd operations
extern BB_bdd BB_addref(BB_bdd bdd);
extern BB_bdd BB_delref(BB_bdd bdd);
extern void BB_setvarnum(int vars);
extern BB_bdd BB_ithvar(int var);
extern int BB_satcount(BB_bdd bdd);
extern void BB_save(BB_bdd bdd, char* name);
extern void BB_done();

#endif
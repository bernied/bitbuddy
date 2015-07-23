#ifndef __BDD_CAL__
#define __BDD_CAL__

#include <cal.h>
#include "parse_cl.h"

// operations
#define BB_op_type Cal_BddOpEnum
#define BB_AND CAL_AND
#define BB_OR CAL_OR
#define BB_XOR CAL_XOR

// main bdd type
typedef Cal_Bdd BB_bdd;

// bdd operations
extern BB_bdd BB_false();
extern BB_bdd BB_true();
extern BB_bdd BB_not(BB_bdd bdd);
extern BB_bdd BB_apply(BB_bdd lhs, BB_bdd rhs, enum BB_op_type op);
extern BB_bdd BB_addref(BB_bdd bdd);
extern BB_bdd BB_delref(BB_bdd bdd);
extern void BB_setvarnum(int vars);
extern BB_bdd BB_ithvar(int var);
extern int BB_satcount(BB_bdd bdd);
extern void BB_print_dot(int n, BB_bdd bdd);
extern void BB_save(BB_bdd bdd, char* name);
extern void BB_init(struct arg_t* args, int inputs, int outputs);
extern void BB_done();

#endif
#ifndef __BDD_ABCD__
#define __BDD_ABCD__

#include "abcd.h"
//#include "abcd_node.h"
#include "parse_cl.h"

// operations
#define BB_op_type int
#define BB_AND 0
#define BB_OR 1
#define BB_XOR 2

// main bdd type
typedef aBCD BB_bdd;

// bdd operations
extern BB_bdd BB_false();
extern BB_bdd BB_true();
extern BB_bdd BB_not(BB_bdd bdd);
extern BB_bdd BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op);
extern BB_bdd BB_addref(BB_bdd bdd);
extern BB_bdd BB_delref(BB_bdd bdd);
extern void BB_setvarnum(int vars);
extern BB_bdd BB_ithvar(int var);
extern int BB_satcount(BB_bdd bdd);
extern void BB_print_dot(int n, BB_bdd bdd);
extern void BB_save(BB_bdd bdd, char* name);
extern void BB_init(struct arg_t* args);
extern void BB_done();

#endif

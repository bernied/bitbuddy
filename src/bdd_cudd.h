#ifndef __BDD_CUDD__
#define __BDD_CUDD__

#include "util.h"
#include "cudd.h"
#include "parse_cl.h"

// operations
#define BB_op_type int
#define BB_AND 0
#define BB_OR 1
#define BB_XOR 2

// main bdd type
typedef DdNode* BB_bdd;

// bdd operations
#include "bdd_ops.h"

#endif

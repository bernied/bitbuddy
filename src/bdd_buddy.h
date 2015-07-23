#ifndef __BDD_BUDDY__
#define __BDD_BUDDY__

#include "bdd.h"
#include "parse_cl.h"

// operations
#define BB_op_type int
#define BB_AND bddop_and
#define BB_OR bddop_or
#define BB_XOR bddop_xor

// main bdd type
typedef bdd BB_bdd;

// bdd operations
#include "bdd_ops.h"

#endif

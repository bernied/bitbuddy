#ifndef __BDD_ABCD__
#define __BDD_ABCD__

#include "abcd.h"
#include "parse_cl.h"

// operations
#define BB_op_type int
#define BB_AND 0
#define BB_OR 1
#define BB_XOR 2

// main bdd type
typedef aBCD BB_bdd;

// bdd operations
#include "bdd_ops.h"

#endif

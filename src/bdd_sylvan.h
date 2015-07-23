#ifndef __BDD_SYLVAN__
#define __BDD_SYLVAN__

#include <sylvan.h>
#include <llmsset.h>
#include "parse_cl.h"

#define CUBE

// operations
#define BB_op_type int
#define BB_AND 0
#define BB_OR 1
#define BB_XOR 2

// main bdd type
typedef BDD BB_bdd;

// bdd operations
#include "bdd_ops.h"

#endif

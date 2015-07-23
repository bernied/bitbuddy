#ifndef __BDD_SDD__
#define __BDD_SDD__

#include <sddapi.h>
#include "parse_cl.h"

// operations
#define BB_op_type int
#define BB_AND CONJOIN
#define BB_OR DISJOIN
#define BB_XOR 2

// main bdd type
typedef SddNode* BB_bdd;

// bdd operations
#include "bdd_ops.h"

#endif

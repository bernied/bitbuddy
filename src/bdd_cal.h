#ifndef __BDD_CAL__
#define __BDD_CAL__

#include <cal.h>
#include "parse_cl.h"

// operations
typedef enum Cal_BddOpEnum BB_op_type;
#define BB_AND CAL_AND
#define BB_OR CAL_OR
#define BB_XOR CAL_XOR

// main bdd type
typedef Cal_Bdd BB_bdd;

// bdd operations
#include "bdd_ops.h"

#endif

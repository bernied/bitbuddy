#ifndef __TYPES__
#define __TYPES__

#include <sys/types.h>
#include <ctype.h>

#if defined(BUDDY)
  #include "bdd.h"

  #define BB_op_type int
  #define BB_AND bddop_and
  #define BB_OR bddop_or
  #define BB_XOR bddop_xor
  #define BB_FALSE bddfalse
  #define BB_TRUE bddtrue

  #define BB_bdd bdd
  #define BB_addref(b) bdd_addref((b))
  #define BB_delref(b) bdd_delref((b))
  #define BB_setvarnum(b) bdd_setvarnum((b))
  #define BB_ithvar(b) bdd_ithvar((b))
  #define BB_done() bdd_done()

#elif defined(CUDD)
  #include "util.h"
  #include "cudd.h"

  #define BB_op_type int
  #define BB_AND 0
  #define BB_OR 1
  #define BB_XOR 2
  #define BB_FALSE Cudd_ReadZero(manager)
  #define BB_TRUE Cudd_ReadOne(manager)

  #define BB_bdd DdNode*
  #define BB_addref(b) (Cudd_Ref((b)),b)
  #define BB_delref(b) (Cudd_Deref((b)),b)
  #define BB_setvarnum(i)
  #define BB_ithvar(b) Cudd_bddIthVar(manager, (b))
  #define BB_done() Cudd_Quit(manager)

#endif

#include "uthash.h"

#ifndef bool
typedef enum bool_t
{
  false = 0, true
} bool;
#endif

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;
// typedef uint8_t  uint8;
// typedef uint32_t  uint32;
// typedef uint8_t uint64;

typedef enum _op
{
  IO, IN, OUT, AND, OR, XOR, NOT, FREE
} Operation;

static char* OpNames[] = { "IO", "IN", "OUT", "AND", "OR", "XOR", "NOT", "FREE" };

struct Node
{
  int node;
  int lhs;
  int rhs;
};

struct Io
{
  uint32 inputs;
  uint32 outputs;
};

struct In
{
  uint32 node;
  uint32 index;
};

struct Out
{
  uint32 node;
  uint32 input;
  uint32 index;
};

struct Free
{
  uint32 node;
};

typedef struct line_t
{
  struct line_t* next;
  uint32 line_no;
  Operation op;
  union
  {
    struct Node n;
    struct Io io;
    struct In in;
    struct Out out;
    struct Free f;
  } data;
} Line;

typedef struct bdd_map_t
{
  int node; // key
  BB_bdd func; // value
  UT_hash_handle hh; // makes this structure hashable
} Bdd_map;

typedef struct state_t
{
  Bdd_map* map;
  uint32 num_inputs;
  BB_bdd* inputs;
  uint32 num_outputs;
  int* outputs;
  BB_bdd sat;
  Line* line;
} State;

#endif

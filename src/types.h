#ifndef __TYPES__
#define __TYPES__

#include <sys/types.h>
#include <ctype.h>
#include <bdd.h>

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

struct In_Out
{
  uint32 node;
  uint32 index;
};

struct Free
{
  uint32 node;
};

typedef struct line_t
{
  Operation op;
  union
  {
    struct Node n;
    struct Io io;
    struct In_Out i_o;
    struct Free f;
  } data;
} Line;

typedef struct bdd_map_t
{
  int node; // key
  bdd func; // value
  UT_hash_handle hh; // makes this structure hashable
} Bdd_map;

typedef struct state_t
{
  Bdd_map* map;
  uint32 num_inputs;
  bdd* inputs;
  uint32 num_outputs;
  bdd* outputs;
  bdd sat;
  int line;
} State;

#endif

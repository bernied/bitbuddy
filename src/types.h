#ifndef __TYPES__
#define __TYPES__

#include <sys/types.h>
#include <ctype.h>

#include "uthash.h"

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;
// typedef uint8_t  uint8;
// typedef uint32_t  uint32;
// typedef uint8_t uint64;

typedef enum _op
{
  IO, IN, OUT, AND, OR, CNF, XOR, NOT, FREE, TRUE, FALSE
} Operation;

static char* OpNames[] = { "IO", "IN", "OUT", "AND", "OR", "CNF", "XOR", "NOT", "FREE", "TRUE", "FALSE" };

struct Node
{
  int node;
  int lhs;
  int rhs;
};

struct Cnf
{
  int node;
  int n1;
  int n2;
  int n3;
  int n4;
  int n5;
  int n6;
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
    struct Cnf cnf;
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

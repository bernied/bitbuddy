#ifndef __TYPES__
#define __TYPES__

#include <sys/types.h>
#include <ctype.h>

#include "uthash.h"

extern void die(const char* msg, ...);

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t  uint32;
typedef uint64_t uint64;

typedef enum _op
{
  NOP, IO, IN, OUT, DOT, AND, OR, CON, DIS, XOR, NOT, FREE, TRUE, FALSE
} Operation;

static char* OpNames[] = { "NOP", "IO", "IN", "OUT", "DOT", "AND", "OR", "CON", "DIS", "XOR", "NOT", "FREE", "TRUE", "FALSE" };

struct Node
{
  int node;
  int lhs;
  int rhs;
};

struct NormalForm
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

struct Dot
{
  uint32 node;
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
    struct NormalForm nf;
    struct Io io;
    struct In in;
    struct Out out;
    struct Dot dot;
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

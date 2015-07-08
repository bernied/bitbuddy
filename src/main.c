#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stdarg.h>

#ifdef ABCD
#include "bdd_abcd.h"
#endif

#ifdef BUDDY
#include "bdd_buddy.h"
#endif

#ifdef CUDD
#include "bdd_cudd.h"
#endif

#ifdef SYLVAN
#include "bdd_sylvan.h"
#endif

#include "types.h"
#include "parse_cl.h"

#define VERSION "0.3"

struct arg_t args;
static Bdd_map* bdd_free_list = NULL;
static char* mask = NULL;
static uint32 mask_len = 0;
static uint32 ith_var = 0; // LAMb: hack!

/*
  TODO:
  -Read the file into memory
  -Make bdd's swapable
    -Add ABCD
    -Add others?
  -Add SAT support
    -Add repeating over input trying to find a good solution
*/

/*
v / version           flag        "version of bitbuddy"
b / bits              string      "bits to set for inputs"
f / file-bits         string      "file containing bits to set for inputs"
k / keep-nodes        flag        "do not free nodes"
g / garbage-collect   flag        "free before garbage collection"
p / print-gc          flag        "print garbage collection events"
n / nodes             int         "number of nodes to pre-allocate"
r / rounds            int         "use up to r rounds to find sat"
s / sat               int         "attempt to find input for given output with max bits"

  bool v;
  char * b;
  char * f;
  bool k;
  bool g;
  bool p;
  int n;
  int r;
  int s;
  bool h;
*/
void
init_default_args(struct arg_t* args)
{
  args->b = NULL;
  args->f = NULL;
  args->k = false;
  args->g = false;
  args->p = false;
  args->n = 100000;
  args->r = -1;
  args->s = -1;

  args->v = false;
}

static void
die(const char* msg, ...)
{
  va_list val;
  fputs("[bitbuddy] ", stderr);

  va_start(val, msg);
  vfprintf(stderr, msg, val);
  va_end(val);

  fputc('\n', stderr);
  fflush(stderr);

  exit(1);
}

void
bool_mask(char* bits)
{
  if (mask)
    die("mask has been defined more then once");

  mask_len = strlen(bits);
  mask = malloc(mask_len);
  if (!mask)
    die("unable to allocate memory for bit mask");

  for (int i = 0; i < mask_len; i++)
  {
    if (bits[i] == '0') {
      mask[i] = 0;
    }
    else if (bits[i] == '1') {
      mask[i] = 1;
    }
    else {
      mask[i] = -1;
    }
  }
}

char*
handle_arguments(int argc, char** argv, struct arg_t* args)
{
  init_default_args(args);
  Cmdline(args, argc, argv);
  if (argc == 1 || args->optind+1 != argc) {
    usage(-1, argv[0]);
  }

  if (args->v) {
    printf("bitbuddy version %s\n", VERSION);
  }

  if (args->k && args->g)
    die("'do not free' and 'garbage collect' options can not both be enabled");

  if (args->n < 10000)
  {
    fprintf(stderr, "number of nodes must be >= 10,000 (%d); setting to 10,000\n", args->n);
    args->n = 10000;
  }

  if (args->b != NULL && args->f != NULL) {
    die("-b and -f can not be used at the same time");
  }
  else if (args->b != NULL) {
    bool_mask(args->b);
  }
  else if (args->f != NULL)
  {
    // LAMb: read file into str
    // call bool_mask(str)
  }

  if (args->s < 0 && args->r > 0) {
    fprintf(stderr, "warning: rounds was set w/o --sat being set, ignoring\n");
  }
  if (args->s >= 0 && args->b == NULL) { // LAMb: make args->b get filled w/ random data if not set
    die("expecting --bits to be set with --sat");
  }
  if (args->r == 0) {
    die("expecting --rounds to be set to non-zero number (%d)", args->r);
  }

  return argv[args->optind];
}

char*
parse_line(char* str, Line* line)
{
  char* c = str;
  uint32 i=1;

  // Parse out the operator
  switch (*c++)
  {
    case 'I':
      if (*c == 'O')
      {
        line->op = IO;
        i = 2;
      }
      else if (*c == 'N')
      {
        line->op = IN;
        i = 2;
      }
      else {
        return "Expected 'IO' or 'IN'";
      }
    break;

    case 'O':
      if (*c == 'R')
      {
        line->op = OR;
        i = 3;
      }
      else if (*c == 'U')
      {
        line->op = OUT;
        i = 3;
      }
      else {
        return "Expected 'OR' or 'OUT'";
      }
    break;

    case 'C':
      line->op = CNF;
      i = 6;
    break;

    case 'A':
      line->op = AND;
      i = 3;
    break;

    case 'X':
      line->op = XOR;
      i = 3;
    break;

    case 'N':
      line->op = NOT;
      i = 2;
    break;

    case 'F':
      if (*c == 'R')
      {
        line->op = FREE;
        i = 1;
      }
      else if (*c == 'A')
      {
        line->op = FALSE;
        i = 1;
      }
      else {
        return "Expected 'FREE' or 'FALSE'";
      }
    break;

    case 'T':
      line->op = TRUE;
      i = 1;
    break;

    default:
      return "Unknown operation";
    break;
  }

  while(*c++ != '\0')
  {
    if (*c == '\t') {
      break;
    }
  }

  int x1, x2, x3, x4, x5, x6, x7;
  switch (i)
  {
    case 1:
      sscanf(c, "%d", &x1);
      break;

    case 2:
      sscanf(c, "%d\t%d", &x1, &x2);
      break;

    case 3:
      sscanf(c, "%d\t%d\t%d", &x1, &x2, &x3);
      break;

    case 4:
      sscanf(c, "%d\t%d\t%d\t%d", &x1, &x2, &x3, &x4);
      break;

    case 5:
      sscanf(c, "%d\t%d\t%d\t%d\t%d", &x1, &x2, &x3, &x4, &x5);
      break;

    case 6:
      sscanf(c, "%d\t%d\t%d\t%d\t%d\t%d", &x1, &x2, &x3, &x4, &x5, &x6);
      break;

    case 7:
      sscanf(c, "%d\t%d\t%d\t%d\t%d\t%d\t%d", &x1, &x2, &x3, &x4, &x5, &x6, &x7);
      break;

    default:
      return "Unexpected number of parameters";
  }

  switch(line->op)
  {
    case IO:
      line->data.io.inputs = x1;
      line->data.io.outputs = x2;
    break;

    case IN:
      line->data.in.node = x1;
      line->data.in.index = x2;
    break;

    case OUT:
      line->data.out.node = x1;
      line->data.out.input = x2;
      line->data.out.index = x3;
    break;

    case AND:
    case OR:
    case XOR:
      line->data.n.node = x1;
      line->data.n.lhs = x2;
      line->data.n.rhs = x3;
    break;

    case CNF:
      line->data.cnf.node = x1;
      line->data.cnf.n1 = x2;
      line->data.cnf.n2 = x3;
      line->data.cnf.n3 = x4;
      line->data.cnf.n4 = x5;
      line->data.cnf.n5 = x6;
      line->data.cnf.n6 = x7;
    break;

    case TRUE:
    case FALSE:
      line->data.n.node = x1;
    break;

    case NOT:
      line->data.n.node = x1;
      line->data.n.lhs = x2;
      line->data.n.rhs = x3;
    break;

    case FREE:
      line->data.f.node = x1;
    break;

    default:
      return "Unknown operation";
  }

  return NULL;
}

Bdd_map*
create_bdd_map(int node, BB_bdd func)
{
  Bdd_map* map = (Bdd_map*) malloc(sizeof(Bdd_map));
  if (map == NULL)
    die("Unable to allocate bdd map");

  map->node = node;
  map->func = func;

  return map;
}

void
put_bdd(State* state, int n, BB_bdd func)
{
  Bdd_map* map = create_bdd_map(n, func);
  HASH_ADD_INT(state->map, node, map);
}

Bdd_map*
get_bdd(State* state, int n)
{
  Bdd_map *s;
  HASH_FIND_INT(state->map, &n, s);

  if (s == NULL && n < 0)
  {
    int inv_n = -n;
    HASH_FIND_INT(state->map, &inv_n, s);
    if (s != NULL)
    {
      BB_bdd inverse = BB_addref(BB_not(s->func));
      put_bdd(state, n, inverse);
      HASH_FIND_INT(state->map, &n, s);
    }
  }

  return s;
}

Bdd_map*
del_bdd(State* state, int n)
{
  Bdd_map* map = get_bdd(state, n);
  if (map == NULL) {
    return NULL;
  }
  HASH_DEL(state->map, map);

  return map;
}

void
clear_bdds(State* state)
{
  Bdd_map *i, *tmp;

  HASH_ITER(hh, state->map, i, tmp)
  {
    HASH_DEL(state->map, i);
    BB_delref(i->func);
  }
}

void
free_bdd(Bdd_map* map)
{
  if (map != NULL)
  {
    if (args.g)
    {
      map->hh.next = bdd_free_list;
      bdd_free_list = map;
    }
    else
    {
      if (!args.k) {
        BB_delref(map->func);
      }
      free(map);
    }
  }
}

void
free_node(State* state, int n)
{
  Bdd_map *map;

  map = del_bdd(state, n);
  free_bdd(map);

  map = del_bdd(state, -n); // remove inverse
  free_bdd(map);
}

char*
process_line(Line* line, State* state)
{
  BB_bdd var, var2;
  Bdd_map *map, *lhs, *rhs;
  int op =-1;
  int index;

  switch(line->op)
  {
    case IO:
      if (state->num_inputs != 0 && state->num_outputs != 0) {
        return "attempted to reset inputs or outputs";
      }

      state->num_inputs = line->data.io.inputs;
      state->num_outputs = line->data.io.outputs;
      state->inputs = (BB_bdd*) calloc(state->num_inputs, sizeof(BB_bdd));

      if (state->inputs == NULL) {
        return "unable to allocate memory for inputs";
      }
      if (mask && mask_len != state->num_inputs) {
        return "mask length does not equal number of inputs";
      }

      uint32 num_vars;
      if (mask)
      {
        num_vars = 0;
        for (int i=0; i < mask_len; i++)
        {
          if (mask[i] == -1) {
            num_vars++;
          }
        }
      }
      else {
        num_vars = state->num_inputs;
      }
      assert(num_vars <= state->num_inputs);

      if (num_vars > 0) {
        BB_setvarnum(num_vars);
      }

      state->outputs = calloc(state->num_outputs, sizeof(int));
      if (!state->outputs) {
        return "unable to allocate memory for outputs";
      }
    break;

    case IN:
      index = line->data.in.index;
      if (index < 0 || index > state->num_inputs)
        die("invalid index %d is not in range of inputs %d", index, state->num_inputs);

      int m = mask ? mask[index] : -1;
      switch(m)
      {
        case 0:
          var = BB_false();
          break;

        case 1:
          var = BB_true();
          break;

        default:
          var = BB_ithvar(ith_var++); // ith_var is incremented for each new var; assumes INPUTs are in order
      }

      state->inputs[index] = var;
      put_bdd(state, line->data.in.node, var); // LAMb: should we call ref method?
    break;

    case OUT:
      map = get_bdd(state, line->data.out.input);
      if (!map) {
        return "unable to find output in hash table";
      }
      var = BB_addref(map->func);
      put_bdd(state, line->data.out.node, var);
      state->outputs[line->data.out.index] = line->data.out.node;
    break;

    case CNF:
      lhs = get_bdd(state, line->data.cnf.n1);
      if (!lhs) {
        return "unable to find n1 in hash table";
      }
      rhs = get_bdd(state, line->data.cnf.n2);
      if (!rhs) {
        return "unable to find b2 in hash table";
      }

      var = BB_addref(BB_apply(lhs->func, rhs->func, BB_OR));
      if (line->data.cnf.n3 != 0)
      {
        rhs = get_bdd(state, line->data.cnf.n3);
        if (!rhs) {
          return "unable to find n3 in hash table";
        }

        var2 = BB_addref(BB_apply(var, rhs->func, BB_OR));
        BB_delref(var);
        var = var2;

        if (line->data.cnf.n4 != 0)
        {
          rhs = get_bdd(state, line->data.cnf.n4);
          if (!rhs) {
            return "unable to find n4 in hash table";
          }

          var2 = BB_addref(BB_apply(var, rhs->func, BB_OR));
          BB_delref(var);
          var = var2;

          if (line->data.cnf.n5 != 0)
          {
            rhs = get_bdd(state, line->data.cnf.n5);
            if (!rhs) {
              return "unable to find n5 in hash table";
            }

            var2 = BB_addref(BB_apply(var, rhs->func, BB_OR));
            BB_delref(var);
            var = var2;

            if (line->data.cnf.n6 != 0)
            {
              rhs = get_bdd(state, line->data.cnf.n6);
              if (!rhs) {
                return "unable to find n6 in hash table";
              }

              var2 = BB_addref(BB_apply(var, rhs->func, BB_OR));
              BB_delref(var);
              var = var2;
            }
          }
        }
      }

      put_bdd(state, line->data.cnf.node, var);
    break;

    case TRUE:
      var = BB_true();
      put_bdd(state, line->data.n.node, var);
    break;

    case FALSE:
      var = BB_false();
      put_bdd(state, line->data.n.node, var);
    break;

    case AND:
      op = BB_AND;
    case OR:
      op = (op == -1) ? BB_OR : op;
    case XOR:
      op = (op == -1) ? BB_XOR : op;

      lhs = get_bdd(state, line->data.n.lhs);
      if (!lhs) {
        return "unable to find lhs in hash table";
      }
      rhs = get_bdd(state, line->data.n.rhs);
      if (!rhs) {
        return "unable to find rhs in hash table";
      }

      var = BB_addref(BB_apply(lhs->func, rhs->func, op));
      put_bdd(state, line->data.n.node, var);
    break;

    case NOT:
      lhs = get_bdd(state, line->data.n.lhs);
      if (!lhs) {
        return "unable to find inverted input in hash table";
      }

      var = BB_addref(BB_not(lhs->func));
      put_bdd(state, line->data.n.node, var);
    break;

    case FREE:
      free_node(state, line->data.n.node);
    break;

    default:
      return "unknown operation";
  }
  state->line++;

  return NULL;
}

State*
init_state(State* state)
{
  if (state == NULL) return  NULL;

  state->map = NULL;
  state->num_inputs = 0;
  state->inputs = NULL;
  state->num_outputs = 0;
  state->outputs = NULL;
  state->line = NULL;

  put_bdd(state, 0, BB_false());
  put_bdd(state, 1, BB_true());

  return state;
}

State*
read_file(FILE* file)
{
  char line_buffer[2048];
  uint32 line_number = 0;
  Line *line, *prev=NULL;
  State* state;
  state = (State*) malloc(sizeof(State));
  init_state(state);

  while (fgets(line_buffer, sizeof(line_buffer), file))
  {
    ++line_number;

    line = calloc(1, sizeof(Line)); // zero entire struct
    if (line == NULL)
      die("unable to allocate memory for line element");

    line->line_no = line_number;
    if (prev != NULL) {
      prev->next = line;
    }
    else {
      state->line = line;
    }
    prev = line;

    char* err = parse_line(line_buffer, line);
    if (err != NULL) {
      fprintf(stderr, "%4d: %s:%s", line_number, err, line_buffer);
    }
  }

  return state;
}

void
line_to_str(Line* line, char* str)
{
  //  IO, IN, OUT, AND, OR, XOR, NOT, FREE
  char* name = OpNames[line->op];
  sprintf(str, "%u: %s", line->line_no, name);
}

void
process_state(State* state, bool ignoreIO)
{
  char *err, str[2048];
  Line* line = state->line;

  while (line != NULL)
  {
//    if (line->line_no % 1000 == 0)  //LAMb
//     if (line->line_no > 34882)  //LAMb
    if (0)
    {
      line_to_str(line, str);
      printf("%s\n", str);
      fflush(stdout);
    }
    if (ignoreIO && line->op == IO) {
      continue;
    }
    err = process_line(line, state);
    if (err != NULL)
      die("Failed to process line: %4d: %s", line->line_no, err);

    line = line->next;
  }
}

void
reset_state(State* state)
{
  ith_var = 0; // LAMb: hack
  clear_bdds(state);
}

int
adjust_sat_mask(char* smask, int len, int bits)
{
  assert(bits >= 0);
  assert(bits <= len);

  int i=0, b=0;

  if (smask == NULL)
    die("expecting mask for sat"); // LAMb: remove when create random mask to start w/

  for (i=0; b < bits && i < len; i++)
  {
    if (smask[i] != 0 && smask[i] != 1) {
      b++;
    }
  }

  for (;i < len; i++)
  {
    if (smask[i] != 0 && smask[i] != 1)
    {
      fprintf(stderr, "too many mask bits: setting bit %d to random number\n", i);
      smask[i] = rand() % 2;
    }
  }

  return b;
}

void
rand_sat_mask(char* smask, int len, int bits)
{
  int c = bits;

  while (c != 0)
  {
    int i = rand() % len;
    if (smask[i] == 0 || smask[i] == 1)
    {
      smask[i] = -1;
      c--;
    }
  }
}

static State* bdd_sat_compare_state; // LAMb: totally evil!
int
bdd_sat_compare(const void* lhs, const void* rhs)
{
    int lhs_node = *((int*)lhs);
    int rhs_node = *((int*)rhs);

    Bdd_map* lhs_map = get_bdd(bdd_sat_compare_state, lhs_node);
    if (!lhs_map)
      die("unable to find lhs map for %d", lhs_node);

    Bdd_map* rhs_map = get_bdd(bdd_sat_compare_state, rhs_node);
    if (!rhs_map)
      die("unable to find rhs map for %d", rhs_node);

    int lhs_sat = (int) BB_satcount(lhs_map->func);
    int rhs_sat = (int) BB_satcount(rhs_map->func);

    return rhs_sat - lhs_sat;
}

void sat_print_handler(char* varset, int size)
{
  for (int i=0; i < size; ++i)
  {
    printf("%c", varset[i] < 0 ? 'x' : varset[i] ? '1' : '0');
  }
  printf("\n");
}

void
next_state(State* state)
{
  // copy state->outputs to outputs
  Bdd_map* map;
  int* outputs = (int*) malloc(state->num_outputs * sizeof(int));
  if (!outputs)
    die("unable to allocate memory to sort bdds");
  memcpy(outputs, state->outputs, state->num_outputs * sizeof(int));

  // sort the list of outputs by sat size
  bdd_sat_compare_state = state; // LAMb: evil...must remove!
  qsort(outputs, state->num_outputs, sizeof(int), bdd_sat_compare);

  // print outputs
  // for (int i=0; i < state->num_outputs; i++)
  // {
  //   map = get_bdd(state, outputs[i]);
  //   if (!map)
  //     die("unable to find bdd for output %d", outputs[i]);
  //   int s = (int) bdd_satcount(map->func); // LAMb: needs to be wrapped!
  //   printf("%i\t%d\n", i, s);
  // }

  // and together until not satisfiable

  map = get_bdd(state, outputs[0]);
  BB_bdd bdd, prev;
  prev = BB_addref(map->func);
  int sc = BB_satcount(prev);
  printf("sats\t%d\t%d\n", 0, sc);
  for (int i=1; i < state->num_outputs; i++)
  {
    map = get_bdd(state, outputs[i]);
    bdd = BB_addref(BB_apply(prev, map->func, BB_AND));
    sc = BB_satcount(bdd);
    printf("sats\t%d\t%d\n", i, sc);

    if (sc == 0)
    {
      BB_delref(bdd);
      state->sat = prev;
      sc = BB_satcount(prev);
      printf("sats\t%d\n", sc);
      BB_save(prev, "sats.blif");
#ifdef BUDDY
      bdd_allsat(prev, sat_print_handler);
#endif

      goto free_outputs;
    }
    else
    {
      BB_delref(prev);
      prev = bdd;
    }
  }

  // LAMB something goofy happened
free_outputs:
  free(outputs);
}

// LAMb: finish me
void
process_sat(State* state, int rounds, int probe)
{
  char *err, str[2048], c;
  Line* line = state->line;

  int bits_set = adjust_sat_mask(mask, mask_len, args.s);
  if (bits_set < probe) {
    rand_sat_mask(mask, mask_len, probe - bits_set);
  }
  process_state(state, false);
  next_state(state);  //LAMb
  rounds--;

  while(rounds != 0)
  {
    next_state(state);
    process_state(state, true);

    if (rounds > 0) {
      rounds--;
    }
  }
}

void
free_list()
{
  printf("FREE NODES: ");

  Bdd_map* node = bdd_free_list;
  Bdd_map* next = NULL;

  int i=0;
  while (node != NULL)
  {
    BB_delref(node->func);
    next = (Bdd_map*) node->hh.next;
    free(node);
    node = next;
    i++;
  }
  bdd_free_list = NULL;
  printf("%d\n", i);
}

void
init()
{
  BB_init(&args);
}

void
save_bdd(State* state, char* name, int node)
{
  Bdd_map* map = get_bdd(state, node);
  BB_bdd bdd = map->func;
  BB_save(bdd, name);
}

int
main(int argc, char** argv)
{
  struct stat sb;
  char* file_name;

  // Handle arguments
  file_name = handle_arguments(argc, argv, &args);

  if (stat(file_name, &sb) == -1)
    die("File %s does not exist", file_name);

  off_t size = sb.st_size;

  // Open file for processing
  FILE* file = fopen(file_name, "r");
  if (file == NULL)
    die("An error occured while opening file \"%s\"", file_name);

  init();

  State* state = read_file(file);

  if (args.s >= 0) {
    process_sat(state, args.r, args.s);
  }
  else {
    process_state(state, false);
  }

  char buff[256];
  for (int i=0; i < state->num_outputs; i++)
  {
//    itoa(i, buff, 10);
    sprintf(buff, "%03d.blif", i);
    save_bdd(state, buff, state->outputs[i]);
  }

  BB_done();
  fclose(file);
  exit(0);
}

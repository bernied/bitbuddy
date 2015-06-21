#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stdarg.h>

#include "types.h"
#include "parse_cl.h"

#define VERSION "0.2"

struct arg_t args;
static Bdd_map* bdd_free_list = NULL;
static char* mask = NULL;
static unsigned mask_len = 0;
static unsigned int ith_var = 0; // LAMb: hack!

#ifdef CUDD
  DdManager* manager = NULL;
#endif

/*
v / version           flag        "version of bitbuddy"
b / bits              string      "bits to set for inputs"
f / file-bits         string      "file containing bits to set for inputs"
k / keep-nodes        flag        "do not free nodes"
g / garbage-collect   flag        "free before garbage collection"
n / nodes             int         "number of nodes to pre-allocate"
s / sat               string      "attempt to find input for given output"

  bool v;
  char * b;
  char * f;
  bool k;
  bool g;
  int n;
  char * s;
  bool h;
*/
void
init_default_args(struct arg_t* args)
{
  args->b = NULL;
  args->f = NULL;
  args->k = false;
  args->g = false;
  args->n = 100000;
  args->s = NULL;

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
    fprintf(stderr, "number of nodes must be >= 10,000; setting to 10,000\n");
    args->n = 10000;
  }

  if (args->b != NULL && args->f != NULL) {
    die("-b and -f can not be used at the same time");
  }
  else if (args->b != NULL)
  {
    bool_mask(args->b);
  }
  else if (args->f != NULL)
  {
    // LAMb: read file into str
    // call bool_mask(str)
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
      else if(*c == 'N')
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
      else if(*c == 'U')
      {
        line->op = OUT;
        i = 3;
      }
      else {
        return "Expected 'OR' or 'OUT'";
      }
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
      line->op = FREE;
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

  int x, y, z;
  switch (i)
  {
    case 1:
      sscanf(c, "%d", &x);
      break;

    case 2:
      sscanf(c, "%d\t%d", &x, &y);
      break;

    case 3:
      sscanf(c, "%d\t%d\t%d", &x, &y, &z);
      break;

    default:
      return "Unexpected number of parameters";
  }

  switch(line->op)
  {
    case IO:
      line->data.io.inputs = x;
      line->data.io.outputs = y;
    break;

    case IN:
      line->data.in.node = x;
      line->data.in.index = y;
    break;

    case OUT:
      line->data.out.node = x;
      line->data.out.input = y;
      line->data.out.index = z;
    break;

    case AND:
    case OR:
    case XOR:
      line->data.n.node = x;
      line->data.n.lhs = y;
      line->data.n.rhs = z;
    break;

    case NOT:
      line->data.n.node = x;
      line->data.n.lhs = y;
      line->data.n.rhs = z;
    break;

    case FREE:
      line->data.f.node = x;
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

BB_bdd
BB_not(BB_bdd bdd)
{
#ifdef BUDDY
  return bdd_not(bdd);
#elif defined(CUDD)
  return Cudd_Not(bdd);
#endif
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

BB_bdd
BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op)
{
#ifdef BUDDY
  return bdd_apply(lhs, rhs, op);
#elif defined(CUDD)
  BB_bdd bdd;
  switch(op)
  {
    case BB_AND:
      bdd = Cudd_bddAnd(manager, lhs, rhs);
    break;

    case BB_OR:
      bdd = Cudd_bddOr(manager, lhs, rhs);
    break;

    case BB_XOR:
      bdd = Cudd_bddXor(manager, lhs, rhs);
    break;

    default:
      die("Unknown operation");
  }
  Cudd_Ref(bdd);
  return bdd;
#endif
}

char*
process_line(Line* line, State* state)
{
  BB_bdd var;
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

      unsigned int num_vars;
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

      state->outputs = (BB_bdd*) calloc(state->num_outputs, sizeof(BB_bdd));
      if (state->outputs == NULL) {
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
          var = BB_FALSE;
          break;

        case 1:
          var = BB_TRUE;
          break;

        default:
          var = BB_ithvar(ith_var++); // ith_var is incremented for each new var; assumes INPUTs are in order
      }

      state->inputs[index] = var;
      put_bdd(state, line->data.in.node, var);
    break;

    case OUT:
      map = get_bdd(state, line->data.out.input);
      if (map == NULL) {
        return "unable to find output in hash table";
      }
      var = map->func;
      BB_addref(var);
      state->outputs[line->data.out.index] = var;
      // LAMb: handle outputs and SAT situations
    break;

    case AND:
      op = BB_AND;
    case OR:
      op = (op == -1) ? BB_OR : op;
    case XOR:
      op = (op == -1) ? BB_XOR : op;

      lhs = get_bdd(state, line->data.n.lhs);
      if (lhs == NULL) {
        return "unable to find lhs in hash table";
      }
      rhs = get_bdd(state, line->data.n.rhs);
      if (rhs == NULL) {
        return "unable to find rhs in hash table";
      }

      var = BB_addref(BB_apply(lhs->func, rhs->func, op));
      put_bdd(state, line->data.n.node, var);
    break;

    case NOT:
      lhs = get_bdd(state, line->data.n.lhs);
      if (lhs == NULL) {
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
  state->line = 0;

  put_bdd(state, 0, BB_FALSE);
  put_bdd(state, 1, BB_TRUE);

  return state;
}

State*
process_file(FILE* file)
{
  char line_buffer[2048];
  uint32 line_number = 0;
  Line line;
  State* state;
  state = (State*) malloc(sizeof(State));
  init_state(state);

  while (fgets(line_buffer, sizeof(line_buffer), file))
  {
      ++line_number;
//      printf("%4d: %s", line_number, line_buffer);
      char* err = parse_line(line_buffer, &line);
      if (err != NULL) {
        fprintf(stderr, "%4d: %s:%s", line_number, err, line_buffer);
      }

      if (state->line % 1000 == 0)  //LAMb
      {
        printf("%d: %s", state->line+1, line_buffer);
        fflush(stdout);
      }
      err = process_line(&line, state); // requires table inputs and outputs
      if (err != NULL)
        die("Failed to process line: %4d: %s:%s", line_number, err, line_buffer);
  }

  return state;
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

#ifdef BUDDY
void
bitbuddy_gbc_handler(int pre, bddGbcStat *stat)
{
  if (pre && bdd_free_list != NULL) {
    free_list();
  }
  bdd_default_gbchandler(pre, stat);
}
#endif

void
init()
{
#ifdef BUDDY
  bdd_init(args.n, 10000);
  bdd_autoreorder(BDD_REORDER_WIN2ITE);
//  bdd_autoreorder(BDD_REORDER_NONE);
  if (args.g) {
    bdd_gbc_hook(&bitbuddy_gbc_handler);
  }
#elif defined(CUDD)
    manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
#endif
}

void
save_bdd(char* name, BB_bdd bdd)
{
#ifdef BUDDY
    bdd_fnsave(name, bdd);
#elif defined(CUDD)
//extern int Cudd_DumpBlif (DdManager *dd, int n, DdNode **f, char const * const *inames, char const * const *onames, char *mname, FILE *fp, int mv);
    FILE* file = fopen(name, "w");
    Cudd_DumpBlif(manager, 1, &bdd, NULL, NULL, NULL, file, 0);
    fclose(file);
#endif
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

  State* state = process_file(file);

  char buff[256];
  for (int i=0; i < state->num_outputs; i++)
  {
//    itoa(i, buff, 10);
    sprintf(buff, "%03d.blif", i);
    save_bdd(buff, state->outputs[i]);
  }

  BB_done();
  fclose(file);
  exit(0);
}

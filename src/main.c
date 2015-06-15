#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "types.h"
#include "parse_cl.h"

#define VERSION "0.1"

struct arg_t args;
Bdd_map* bdd_free_list = NULL;

/*
v / version           flag        "version of bitbuddy"
a                     string      "first set of bits"
b                     string      "second set of bits"
c / chunk-size        int         "size of h, a and b (defaults to input size/3)"
f / free              flag        "don't free nodes"
g / garbage-collect   flag        "free before garbage collection"
h / hash              string      "hash value input"
i / initial-hash      flag        "use standard initial sha256 hash"
l / length            flag        "use length of parameter a"
n / nodes             int         "number of nodes to pre-allocate"
s / sat               string      "attempt to find input for given output"
*/
void
init_default_args(struct arg_t* args)
{
  args->a = NULL;
  args->b = NULL;
  args->c = 0;
  args->f = false;
  args->g = false;
  args->h = NULL;
  args->i = false;
  args->l = false;
  args->n = 100000;
  args->s = NULL;

  args->v = false;
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

  if (args->f && args->g)
  {
    fprintf(stderr, "'do not free' and 'garbage collect' options can not both be enabled\n");
    exit(EXIT_FAILURE);
  }

  if (args->n < 10000)
  {
    fprintf(stderr, "number of nodes must be >= 10,000; setting to 10,000\n");
    args->n = 10000;
  }

  if (args->i)
  {
    if (args->h != NULL)
    {
      fprintf(stderr, "-i and -h can not be used at the same time\n");
      exit(EXIT_FAILURE);
    }
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
create_bdd_map(int node, bdd func)
{
  Bdd_map* map = (Bdd_map*) malloc(sizeof(Bdd_map));
  if (map == NULL) {
    exit(EXIT_FAILURE);
  }
  map->node = node;
  map->func = func;

  return map;
}

void
put_bdd(State* state, int n, bdd func)
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
      bdd inverse = bdd_addref(bdd_not(s->func));
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
      if (!args.f) {
        bdd_delref(map->func);
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
  bdd var;
  Bdd_map *map, *lhs, *rhs;
  int op =-1;

  switch(line->op)
  {
    case IO:
      if (state->num_inputs != 0 && state->num_outputs != 0) {
        return "Attempted to reset inputs or outputs";
      }
      state->num_inputs = line->data.io.inputs;
      state->num_outputs = line->data.io.outputs;
      state->inputs = (bdd*) calloc(state->num_inputs, sizeof(bdd));
      if (state->inputs == NULL) {
        return "Unable to allocate memory for inputs";
      }
      bdd_setvarnum(state->num_inputs);

      state->outputs = (bdd*) calloc(state->num_outputs, sizeof(bdd));
      if (state->outputs == NULL) {
        return "Unable to allocate memory for outputs";
      }
    break;

    case IN:
      var = bdd_ithvar(line->data.in.index);
      state->inputs[line->data.in.index] = var;
      put_bdd(state, line->data.in.node, var);
    break;

    case OUT:
      map = get_bdd(state, line->data.out.input);
      if (map == NULL) {
        return "Unable to find output in hash table";
      }
      var = map->func;
      bdd_addref(var);
      state->outputs[line->data.out.index] = var;
      // LAMb: handle outputs and SAT situations
    break;

    case AND:
      op = bddop_and;
    case OR:
      op = (op == -1) ? bddop_or : op;
    case XOR:
      op = (op == -1) ? bddop_xor : op;

      lhs = get_bdd(state, line->data.n.lhs);
      if (lhs == NULL) {
        return "Unable to find lhs in hash table";
      }
      rhs = get_bdd(state, line->data.n.rhs);
      if (rhs == NULL) {
        return "Unable to find rhs in hash table";
      }

      var = bdd_addref(bdd_apply(lhs->func, rhs->func, op));
      put_bdd(state, line->data.n.node, var);
    break;

    case NOT:
      lhs = get_bdd(state, line->data.n.lhs);
      if (lhs == NULL) {
        return "Unable to find inverted input in hash table";
      }

      var = bdd_addref(bdd_not(lhs->func));
      put_bdd(state, line->data.n.node, var);
    break;

    case FREE:
      free_node(state, line->data.n.node);
    break;

    default:
      return "Unknown operation";
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

  put_bdd(state, 0, bddfalse);
  put_bdd(state, 1, bddtrue);

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

      printf("%d: %s", state->line+1, line_buffer);
      fflush(stdout);
      err = process_line(&line, state); // requires table inputs and outputs
      if (err != NULL)
      {
        fprintf(stderr, "%4d: %s:%s", line_number, err, line_buffer);
        exit(EXIT_FAILURE);
      }
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
    bdd_delref(node->func);
    next = (Bdd_map*) node->hh.next;
    free(node);
    node = next;
    i++;
  }
  bdd_free_list = NULL;
  printf("%d\n", i);
}

void
bitbuddy_gbc_handler(int pre, bddGbcStat *stat)
{
  if (pre && bdd_free_list != NULL) {
    free_list();
  }
  bdd_default_gbchandler(pre, stat);
}

void
init()
{
  bdd_init(args.n, 10000);
  bdd_autoreorder(BDD_REORDER_WIN2ITE);
//  bdd_autoreorder(BDD_REORDER_NONE);
  if (args.g) {
    bdd_gbc_hook(&bitbuddy_gbc_handler);
  }
}

int
main(int argc, char** argv)
{
  struct stat sb;
  char* file_name;

  // Handle arguments
  file_name = handle_arguments(argc, argv, &args);

  if (stat(file_name, &sb) == -1)
  {
    fprintf(stderr, "File %s does not exist.\n", file_name);
    exit(EXIT_FAILURE);
  }
  off_t size = sb.st_size;

  // Open file for processing
  FILE* file = fopen(file_name, "r");
  if (file == NULL)
  {
    fprintf(stderr, "An error occured while opening file \"%s\".", file_name);
    exit(EXIT_FAILURE);
  }

  init();

  State* state = process_file(file);

  char buff[256];
  for (int i=0; i < state->num_outputs; i++)
  {
//    itoa(i, buff, 10);
    sprintf(buff, "%d", i);
    bdd_fnsave(buff, state->outputs[i]);
  }

  bdd_done();
  fclose(file);
  exit(0);
}

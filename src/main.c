#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <bdd.h>

#include "types.h"
#include "parse_cl.h"

#define VERSION "0.1"

struct arg_t args;

/*
v / version           flag        "  version of bitbuddy"
i / initial-hash      flag        "  use standard initial sha256 hash"
h / hash              string      "  hash value input"
a                     string      "  first set of bits"
b                     string      "  second set of bits"
l / length            flag        "  use length of parameter a"
s / sat               string      "  attempt to find input for given output"
c / chunk-size        int         "  size of h, a and b (defaults to input size/3)"
*/
void
init_default_args(struct arg_t* args)
{
  args->v = false;
  args->i = false;
  args->h = NULL;
  args->a = NULL;
  args->b = NULL;
  args->l = false;
  args->s = NULL;
  args->c = 0;
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
        i = 2;
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
    case OUT:
      line->data.i_o.node = x;
      line->data.i_o.index = y;
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

  // LAMb: fix by using hash table!
  // for(s=state->map; s != NULL; s=s->hh.next) {
  //   if (s->node == n) {
  //     return s;
  //   }
  // }
  // return NULL;

// printf("node = %d\n", n);
  HASH_FIND_INT(state->map, &n, s);
// printf("value = %d\n", s->func);
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
      var = bdd_ithvar(line->data.i_o.index);
      state->inputs[line->data.i_o.index] = var;
      put_bdd(state, line->data.i_o.node, var);
    break;

    case OUT:
      map = get_bdd(state, line->data.i_o.node);
      if (map == NULL) {
        return "Unable to find output in hash table";
      }
      var = map->func;
      bdd_addref(var);
      state->outputs[line->data.i_o.index] = var;
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
      map = del_bdd(state, line->data.n.node);
      if (map != NULL)
      {
//        bdd_delref(map->func);
        free(map);
      }
      else {
        fprintf(stderr, "missing node %d to free\n", line->data.n.node);
      }
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

      err = process_line(&line, state); // requires table inputs and outputs
      printf("%d: %s", state->line, line_buffer);
      if (err != NULL)
      {
        fprintf(stderr, "%4d: %s:%s", line_number, err, line_buffer);
        exit(EXIT_FAILURE);
      }
  }

  return state;
}

void
init()
{
  bdd_init(200000000,10000);
  bdd_autoreorder(BDD_REORDER_WIN2ITE);
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

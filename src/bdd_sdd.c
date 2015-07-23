#include "bdd_sdd.h"

static SddManager* manager =NULL;

BB_bdd
BB_false()
{
  return sdd_manager_false(manager);
}

BB_bdd
BB_true()
{
  return sdd_manager_true(manager);
}

BB_bdd
BB_not(BB_bdd bdd)
{
  return sdd_negate(bdd, manager);
}

static BB_bdd
sdd_xor(BB_bdd lhs, BB_bdd rhs)
{
  BB_bdd or = sdd_ref(sdd_apply(lhs, rhs, BB_OR, manager), manager);
  BB_bdd and = sdd_ref(sdd_apply(lhs, rhs, BB_AND, manager), manager);
  BB_bdd nand = sdd_ref(sdd_negate(and, manager), manager);

  BB_bdd xor = sdd_ref(sdd_apply(or, nand, BB_AND, manager), manager);

  sdd_deref(or, manager);
  sdd_deref(and, manager);
  sdd_deref(nand, manager);

  return xor;
}

BB_bdd
BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op)
{
  return (op == BB_XOR) ? sdd_xor(lhs, rhs) : sdd_ref(sdd_apply(lhs, rhs, op, manager), manager);
}

BB_bdd
BB_addref(BB_bdd bdd)
{
  return sdd_ref(bdd, manager);
}

BB_bdd
BB_delref(BB_bdd bdd)
{
  return sdd_deref(bdd, manager);
}

void
BB_setvarnum(int vars)
{
}

BB_bdd
BB_ithvar(int var)
{
  return sdd_manager_literal(var, manager);
}

int
BB_satcount(BB_bdd bdd)
{
  return (int) sdd_model_count(bdd, manager); // LAMb: should be long!
}

void
BB_print_dot(int n, BB_bdd bdd)
{

  char file_name[256];
  sprintf(file_name, "%d.dot", n);
  sdd_save_as_dot(file_name, bdd);
}

void
BB_save(BB_bdd bdd, char* name)
{
  FILE* file = fopen(name, "w");
  sdd_save(name, bdd);
}

void
BB_init(struct arg_t* args, int inputs, int outputs)
{
  manager = sdd_manager_create(inputs, 1);
}

void
BB_done()
{
  sdd_manager_free(manager);
}

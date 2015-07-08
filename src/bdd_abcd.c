#include "bdd_abcd.h"

static aBCD_Manager manager;

BB_bdd
BB_false()
{
  return 0;
}

BB_bdd
BB_true()
{
  return 1;
}

BB_bdd
BB_not(BB_bdd bdd)
{
  return aBCD_not(manager, bdd);
}

BB_bdd
BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op)
{
  BB_bdd bdd;
  switch(op)
  {
    case BB_AND:
      bdd = aBCD_and(manager, lhs, rhs);
    break;

    case BB_OR:
      bdd = aBCD_or(manager, lhs, rhs);
    break;

    case BB_XOR:
      bdd = aBCD_not(manager, aBCD_implies(manager, lhs, rhs));
    break;
  }
  // LAMb: ref?
  return bdd;
}

BB_bdd
BB_addref(BB_bdd bdd)
{
  return bdd;
}

BB_bdd
BB_delref(BB_bdd bdd)
{
  return bdd;
}

void
BB_setvarnum(int vars)
{

}

BB_bdd
BB_ithvar(int var)
{
  return aBCD_var(manager, var, NULL);
}

int
BB_satcount(BB_bdd bdd)
{
  return -1; // LAMb;
}

void
BB_save(BB_bdd bdd, char* name)
{
  aBCD_dump(manager, bdd, NULL, NULL); // LAMb: wrong
}

void
BB_init(struct arg_t* args)
{
  manager = aBCD_new_manager(NULL, NULL, NULL, args->n, 10.0);
}

void
BB_done()
{
  aBCD_free_manager(manager);
  manager = NULL;
}
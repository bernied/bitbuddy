#include "bdd_cudd.h"

static DdManager* manager =NULL;

BB_bdd
BB_false()
{
  return Cudd_ReadZero(manager);
}

BB_bdd
BB_true()
{
  return Cudd_ReadOne(manager);
}

BB_bdd
BB_not(BB_bdd bdd)
{
  return Cudd_Not(bdd);
}

BB_bdd
BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op)
{
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
  }
  Cudd_Ref(bdd);
  return bdd;
}

BB_bdd
BB_addref(BB_bdd bdd)
{
  Cudd_Ref(bdd);
  return bdd;
}

BB_bdd
BB_delref(BB_bdd bdd)
{
  Cudd_Deref(bdd);
  return bdd;
}

void
BB_setvarnum(int vars)
{
  //LAMb
}

BB_bdd
BB_ithvar(int var)
{
  return Cudd_bddIthVar(manager, var);
}

int
BB_satcount(BB_bdd bdd)
{
  return 0; //LAMb
}

void
BB_print_dot(int n, BB_bdd bdd)
{

}

void
BB_save(BB_bdd bdd, char* name)
{
  FILE* file = fopen(name, "w");
  Cudd_DumpBlif(manager, 1, &bdd, NULL, NULL, NULL, file, 0);
  fclose(file);
}

void
BB_init(struct arg_t* args)
{
  manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
}

void
BB_done()
{
  Cudd_Quit(manager);
}

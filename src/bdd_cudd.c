#include "bdd_cudd.h"

extern DdManager* manager;

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
BB_save(BB_bdd bdd, char* name)
{
  //LAMb
}

void
BB_done()
{
  Cudd_Quit(manager);
}

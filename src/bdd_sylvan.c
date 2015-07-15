#include "bdd_sylvan.h"
#include <string.h>

BB_bdd
BB_false()
{
  return sylvan_false;
}

BB_bdd
BB_true()
{
  return sylvan_true;
}

BB_bdd
BB_not(BB_bdd bdd)
{
  LACE_ME;
  return sylvan_not(bdd);
}

BB_bdd
BB_cube(int cube[], size_t size)
{
  LACE_ME;
  uint8_t* m = (uint8_t*) alloca(size);
  BDDVAR* c = (BDDVAR*) alloca(size * sizeof(BDDVAR));
  memset (c, 0, size * sizeof(BDDVAR));

  for (int i=0; i < size; i++)
  {
    if (cube[i] < 0)
    {
      m[i] = 1; // invert because cube creates a conjunction instead of disjunction
      c[i] = -cube[i];
    }
    else
    {
      m[i] = 0;
      c[i] = cube[i];
    }
  }

  BDDSET vars = sylvan_set_fromarray(c, size);
  BDD bdd = sylvan_cube(vars, m);
  sylvan_not(bdd); // turn into disjunction

  return bdd;
}

BB_bdd
BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op)
{
  LACE_ME;
  BB_bdd bdd;
  switch(op)
  {
    case BB_AND:
      bdd = sylvan_and(lhs, rhs);
    break;

    case BB_OR:
      bdd = sylvan_or(lhs, rhs);
    break;

    case BB_XOR:
      bdd = sylvan_xor(lhs, rhs);
    break;
  }
  return bdd;
}

BB_bdd
BB_addref(BB_bdd bdd)
{
  LACE_ME;
  sylvan_ref(bdd);
  return bdd;
}

BB_bdd
BB_delref(BB_bdd bdd)
{
  LACE_ME;
  sylvan_deref(bdd);
  return bdd;
}

void
BB_setvarnum(int vars)
{

}

BB_bdd
BB_ithvar(int var)
{
  LACE_ME;
  return sylvan_ithvar(var);
}

int
BB_satcount(BB_bdd bdd)
{
  LACE_ME;
  return sylvan_satcount(bdd, 100); //LAMb
}

void
BB_save(BB_bdd bdd, char* name)
{

}

void
BB_init(struct arg_t* args)
{
  lace_init(0, 1000000);
  lace_startup(0, NULL, NULL);
  sylvan_init_package(1LL<<24, 1LL<<33, 1LL<<24, 1LL<<33); // LAMb: get values from args!
  sylvan_init_bdd(1);
}

void
BB_done()
{
  sylvan_quit();
}

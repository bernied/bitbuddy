#include "bdd_buddy.h"

BB_bdd
BB_addref(BB_bdd bdd)
{
  bdd_addref(bdd);
  return bdd;
}

BB_bdd
BB_delref(BB_bdd bdd)
{
  bdd_delref(bdd);
  return bdd;
}

void
BB_setvarnum(int vars)
{
  bdd_setvarnum(vars);
}

BB_bdd
BB_ithvar(int var)
{
  return bdd_ithvar(var);
}

int
BB_satcount(BB_bdd bdd)
{
  return (int) dd_satcount(bdd);
}

void
BB_save(BB_bdd bdd, char* name)
{
  bdd_fnsave(name, bdd);
}

void
BB_done()
{
  bdd_done();
}

#include "bdd_cal.h"

static Cal_BddManager manager =NULL;
static Cal_Bdd* vars;

BB_bdd
BB_false()
{
  return Cal_BddZero(manager);
}

BB_bdd
BB_true()
{
  return Cal_BddOne(manager);
}

BB_bdd
BB_not(BB_bdd bdd)
{
  return Cal_BddNot(manager, bdd);
}

BB_bdd
BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op)
{
  BB_bdd bdd;
  switch(op)
  {
    case BB_AND:
      bdd = Cal_BddAnd(manager, lhs, rhs);
    break;

    case BB_OR:
      bdd = Cal_BddOr(manager, lhs, rhs);
    break;

    case BB_XOR:
      bdd = Cal_BddXor(manager, lhs, rhs);
    break;
  }
  Cal_BddUnFree(manager, bdd);
  return bdd;
}

BB_bdd
BB_addref(BB_bdd bdd)
{
  Cal_BddUnFree(manager, bdd);
  return bdd;
}

BB_bdd
BB_delref(BB_bdd bdd)
{
  Cal_BddFree(manager, bdd);
  return bdd;
}

void
BB_setvarnum(int vars)
{
  //LAMb
}

BB_bdd
BB_ithvar(int i)
{
  return vars[i];
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
  Cal_BddDumpBdd(manager, bdd, NULL, file);
  fclose(file);
}

void
BB_init(struct arg_t* args, int inputs, int outputs)
{
  manager = Cal_BddManagerInit();
  vars = (Cal_Bdd*) malloc((inputs+1) * sizeof(Cal_Bdd));

  for(int i=0; i <= inputs; ++i) {
    vars[i] = Cal_BddManagerCreateNewVarLast(manager);
  }
}

void
BB_done()
{
  Cal_BddManagerQuit(manager);
  manager = NULL;
}

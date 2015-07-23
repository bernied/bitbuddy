#include "zdd_cudd.h"

static DdManager* manager =NULL;

BB_bdd
BB_false()
{
  return NULL; // LAMb: what is false?
}

BB_bdd
BB_true()
{
  return Cudd_ReadZddOne(manager, 0); // LAMb: need max cover!
}

BB_bdd
BB_not(BB_bdd bdd)
{
  return Cudd_zddComplement(manager, bdd); // transforms to bdd, inverts and converts back to zdd
}

static BB_bdd
zdd_xor(BB_bdd lhs, BB_bdd rhs)
{
  BB_bdd ldiff = Cudd_zddDiff(manager, lhs, rhs);
  Cudd_Ref(ldiff);
  BB_bdd rdiff = Cudd_zddDiff(manager, rhs, lhs);
  Cudd_Ref(rdiff);
  BB_bdd bdd = Cudd_zddUnion(manager, ldiff, rdiff);
  Cudd_Ref(bdd);
  Cudd_Deref(ldiff);
  Cudd_Deref(rdiff);
  return bdd;
}

BB_bdd
BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op)
{
  BB_bdd bdd;
  switch(op)
  {
    case BB_AND:
      bdd = Cudd_zddIntersect(manager, lhs, rhs);
      Cudd_Ref(bdd);
    break;

    case BB_OR:
      bdd = Cudd_zddUnion(manager, lhs, rhs);
      Cudd_Ref(bdd);
    break;

    case BB_XOR:
      bdd = zdd_xor(lhs, rhs);
    break;
  }
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

}

BB_bdd
BB_ithvar(int var)
{
  return Cudd_zddIthVar(manager, var);
}

int
BB_satcount(BB_bdd bdd)
{
  return 0; //LAMb
}

void
BB_print_dot(int n, BB_bdd bdd)
{

  char file_name[256];
  sprintf(file_name, "%d.dot", n);
  FILE* file = fopen(file_name, "w");
  Cudd_zddDumpDot(manager, 1, &bdd, NULL, NULL, file);
  fflush(file);
  fclose(file);
}

void
BB_save(BB_bdd bdd, char* name)
{
  FILE* file = fopen(name, "w");
  Cudd_DumpBlif(manager, 1, &bdd, NULL, NULL, NULL, file, 0);
  fclose(file);
}

void
BB_init(struct arg_t* args, int inputs, int outputs)
{
  manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
}

void
BB_done()
{
  Cudd_Quit(manager);
}

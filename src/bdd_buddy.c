#include "bdd_buddy.h"

BB_bdd
BB_false()
{
  return bddfalse;
}

BB_bdd
BB_true()
{
  return bddtrue;
}

BB_bdd
BB_not(BB_bdd bdd)
{
  return bdd_not(bdd);
}

BB_bdd
BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op)
{
  return bdd_apply(lhs, rhs, op);
}

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
  return (int) bdd_satcount(bdd);
}

void
BB_print_dot(int n, BB_bdd bdd)
{

}

void
BB_save(BB_bdd bdd, char* name)
{
  bdd_fnsave(name, bdd);
}

static int default_handler =0;

void
bitbuddy_gbc_handler_default(int pre, bddGbcStat *stat)
{
  if (default_handler) {
    bdd_default_gbchandler(pre, stat);
  }
}

extern void free_list();

void
bitbuddy_gbc_handler_free(int pre, bddGbcStat *stat)
{
//  if (pre && bdd_free_list != NULL) {
  if (pre) {
    free_list();
  }
  bitbuddy_gbc_handler_default(pre, stat);
}

void
BB_init(struct arg_t* args)
{
  bdd_init(args->n, 10000);
  bdd_autoreorder(BDD_REORDER_WIN2ITE);
//  bdd_autoreorder(BDD_REORDER_NONE);
  if (args->g) {
    bdd_gbc_hook(&bitbuddy_gbc_handler_free);
  }
  else {
    bdd_gbc_hook(&bitbuddy_gbc_handler_default);
  }
  default_handler = args->p;
}

void
BB_done()
{
  bdd_done();
}

// Do not include this file.
// Its included by each header that needs these operations.

extern BB_bdd BB_false();
extern BB_bdd BB_true();
extern BB_bdd BB_not(BB_bdd bdd);
extern BB_bdd BB_apply(BB_bdd lhs, BB_bdd rhs, BB_op_type op);
extern BB_bdd BB_addref(BB_bdd bdd);
extern BB_bdd BB_delref(BB_bdd bdd);
extern void BB_setvarnum(int vars);
extern BB_bdd BB_ithvar(int var);
extern int BB_satcount(BB_bdd bdd);
extern void BB_print_dot(int n, BB_bdd bdd);
extern void BB_save(BB_bdd bdd, char* name);
extern void BB_init(struct arg_t* args, int inputs, int outputs);
extern void BB_done();

#ifdef CUBE
extern BB_bdd BB_cube(int cube[], size_t size);
#endif

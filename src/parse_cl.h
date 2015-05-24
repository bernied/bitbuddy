/******************************************************************************
**
** parse_cl.h
**
** Sun May 24 17:56:03 2015
** Linux 3.2.0-23-generic-pae (#36-Ubuntu SMP Tue Apr 10 22:19:09 UTC 2012) i686
** vagrant@precise32 (vagrant)
**
** Header file for command line parser
**
** Automatically created by genparse v0.8.7
**
** See http://genparse.sourceforge.net for details and updates
**
******************************************************************************/

#include <stdio.h>
#include "types.h"

/* customized structure for command line parameters */
struct arg_t
{
  bool v;
  char * a;
  char * b;
  int c;
  bool f;
  bool g;
  char * h;
  bool i;
  bool l;
  int n;
  char * s;
  bool help;
  int optind;
};

/* function prototypes */
void Cmdline (struct arg_t *my_args, int argc, char *argv[]);
void usage (int status, char *program_name);

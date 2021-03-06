#ifndef __PARSE_CL__
#define __PARSE_CL__

/******************************************************************************
**
** parse_cl.h
**
** Sun Jun 21 14:18:10 2015
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

#ifndef bool
typedef enum bool_t
{
  false = 0, true
} bool;
#endif

/* customized structure for command line parameters */
struct arg_t
{
  bool v;
  char * b;
  bool c;
  char * f;
  bool k;
  bool g;
  bool p;
  int n;
  int r;
  int s;
  bool h;
  int optind;
};

/* function prototypes */
void Cmdline (struct arg_t *my_args, int argc, char *argv[]);
void usage (int status, char *program_name);

#endif

/******************************************************************************
**
** parse_cl.c
**
** Sat Jun 20 02:00:02 2015
** Linux 3.2.0-23-generic-pae (#36-Ubuntu SMP Tue Apr 10 22:19:09 UTC 2012) i686
** vagrant@precise32 (vagrant)
**
** C file for command line parser
**
** Automatically created by genparse v0.8.7
**
** See http://genparse.sourceforge.net for details and updates
**
******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "parse_cl.h"

static struct option const long_options[] =
{
  {"version", no_argument, NULL, 'v'},
  {"bits", required_argument, NULL, 'b'},
  {"file-bits", required_argument, NULL, 'f'},
  {"keep-nodes", no_argument, NULL, 'k'},
  {"garbage-collect", no_argument, NULL, 'g'},
  {"nodes", required_argument, NULL, 'n'},
  {"sat", required_argument, NULL, 's'},
  {"help", no_argument, NULL, 'h'},
  {NULL, 0, NULL, 0}
};

/*----------------------------------------------------------------------------
**
** Cmdline ()
**
** Parse the argv array of command line parameters
**
**--------------------------------------------------------------------------*/

void Cmdline (struct arg_t *my_args, int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  int errflg = 0;

  my_args->v = false;
  my_args->b = NULL;
  my_args->f = NULL;
  my_args->k = false;
  my_args->g = false;
  my_args->s = NULL;
  my_args->h = false;

  optind = 0;
  while ((c = getopt_long (argc, argv, "vb:f:kgn:s:h", long_options, &optind)) != - 1)
    {
      switch (c)
        {
        case 'v':
          my_args->v = true;
          break;

        case 'b':
          my_args->b = optarg;
          break;

        case 'f':
          my_args->f = optarg;
          break;

        case 'k':
          my_args->k = true;
          break;

        case 'g':
          my_args->g = true;
          break;

        case 'n':
          my_args->n = atoi (optarg);
          break;

        case 's':
          my_args->s = optarg;
          break;

        case 'h':
          my_args->h = true;
          usage (EXIT_SUCCESS, argv[0]);
          break;

        default:
          usage (EXIT_FAILURE, argv[0]);

        }
    } /* while */

  if (errflg)
    usage (EXIT_FAILURE, argv[0]);

  if (optind >= argc)
    my_args->optind = 0;
  else
    my_args->optind = optind;
}

/*----------------------------------------------------------------------------
**
** usage ()
**
** Print out usage information, then exit
**
**--------------------------------------------------------------------------*/

void usage (int status, char *program_name)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, "Try `%s --help' for more information.\n",
            program_name);
  else
    {
      printf ("\
Usage: %s [OPTION]... [FILE]\n\
\n\
  -v, --version         version of bitbuddy\n\
  -b, --bits            bits to set for inputs\n\
  -f, --file-bits       file containing bits to set for inputs\n\
  -k, --keep-nodes      do not free nodes\n\
  -g, --garbage-collect free before garbage collection\n\
  -n, --nodes           number of nodes to pre-allocate\n\
  -s, --sat             attempt to find input for given output\n\
  -h, --help            Display this help and exit.\n\
\n", program_name);
    }
  exit (status);
}

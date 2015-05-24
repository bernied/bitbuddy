/******************************************************************************
**
** parse_cl.c
**
** Sun May 24 17:56:03 2015
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
  {"chunk-size", required_argument, NULL, 'c'},
  {"free", no_argument, NULL, 'f'},
  {"garbage-collect", no_argument, NULL, 'g'},
  {"hash", required_argument, NULL, 'h'},
  {"initial-hash", no_argument, NULL, 'i'},
  {"length", no_argument, NULL, 'l'},
  {"nodes", required_argument, NULL, 'n'},
  {"sat", required_argument, NULL, 's'},
  {"help", no_argument, NULL, 0},
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
  my_args->a = NULL;
  my_args->b = NULL;
  my_args->f = false;
  my_args->g = false;
  my_args->h = NULL;
  my_args->i = false;
  my_args->l = false;
  my_args->s = NULL;
  my_args->help = false;

  optind = 0;
  while ((c = getopt_long (argc, argv, "va:b:c:fgh:iln:s:", long_options, &optind)) != - 1)
    {
      switch (c)
        {
        case 'v':
          my_args->v = true;
          break;

        case 'a':
          my_args->a = optarg;
          break;

        case 'b':
          my_args->b = optarg;
          break;

        case 'c':
          my_args->c = atoi (optarg);
          break;

        case 'f':
          my_args->f = true;
          break;

        case 'g':
          my_args->g = true;
          break;

        case 'h':
          my_args->h = optarg;
          break;

        case 'i':
          my_args->i = true;
          break;

        case 'l':
          my_args->l = true;
          break;

        case 'n':
          my_args->n = atoi (optarg);
          break;

        case 's':
          my_args->s = optarg;
          break;

        case 0:
          my_args->help = true;
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
  -a                    first set of bits\n\
  -b                    second set of bits\n\
  -c, --chunk-size      size of h, a and b (defaults to input size/3)\n\
  -f, --free            don't free nodes\n\
  -g, --garbage-collect free before garbage collection\n\
  -h, --hash            hash value input\n\
  -i, --initial-hash    use standard initial sha256 hash\n\
  -l, --length          use length of parameter a\n\
  -n, --nodes           number of nodes to pre-allocate\n\
  -s, --sat             attempt to find input for given output\n\
      --help            display this help and exit\n\
\n", program_name);
    }
  exit (status);
}

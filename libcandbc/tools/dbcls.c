/*  dbcls -- list contents of a DBC file 
    Copyright (C) 2007-2011 Andreas Heitmann

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/*
 * usage:
 *
 * dbcls -d dbcfile -N  > networks.txt
 * dbcls -d dbcfile -n  > nodes.txt
 * dbcls -d dbcfile -e  > environment.txt
 * dbcls -d dbcfile -m  > messages.txt
 * dbcls -d dbcfile -s  > signals.txt
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>
#include <getopt.h>

#include "model/dbcModel.h"
#include "model/dbcReader.h"
#include "showDbc.h"

static int verbose_flag;
static int networks_flag;
static int messages_flag;
static int signals_flag;
static int nodes_flag;
static int envvars_flag;
static int valtables_flag;

static void help(void)
{
  fprintf(stderr,
          "Usage: dbcls [OPTION] -d dbcfile\n"
          "List information about dbcfile.\n"
          "\n"
          "Options:\n"
          "  -N, --networks             list networks\n"
          "  -n, --nodes                list nodes\n"
          "  -e, --envvars              list environment variables\n"
          "  -m, --messages             list messages\n"
          "  -s, --signals              list signals\n"
          "      --verbose              verbose output\n"
          "      --brief                brief output (default)\n"
          "      --help     display this help and exit\n");
}

int
main(int argc, char **argv)
{
  dbc_t *dbc;
  char *filename = NULL; /* default: stdin */
  int c;

#if 0
  extern int yydebug;
  yydebug = 1;
#endif

  verbose_flag = 0;

  while (1) {
    static struct option long_options[] = {
      /* These options set a flag. */
      {"verbose", no_argument,       &verbose_flag, 1},
      {"brief",   no_argument,       &verbose_flag, 0},
      /* These options don't set a flag.
         We distinguish them by their indices. */
      {"dbc",     required_argument, 0, 'd'},
      {"networks",no_argument,       &networks_flag,  'N'},
      {"nodes"   ,no_argument,       &nodes_flag,     'n'},
      {"envvars" ,no_argument,       &envvars_flag,   'e'},
      {"messages",no_argument,       &messages_flag,  'm'},
      {"signals" ,no_argument,       &signals_flag,   's'},
      {"valtables" ,no_argument,     &valtables_flag, 't'},
      {"help",    no_argument,       NULL,            'h'},
      {0, 0, 0, 0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "d:Nnemst",
                     long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
    case 0:
      /* If this option set a flag, do nothing else now. */
      if (long_options[option_index].flag != 0)
        break;
      printf ("option %s", long_options[option_index].name);
      if (optarg)
        printf (" with arg %s", optarg);
      printf ("\n");
      break;
    case 'd': filename = optarg;  break;
    case 'N': networks_flag = 1;  break;
    case 'n': nodes_flag = 1;     break;
    case 'e': envvars_flag = 1;   break;
    case 'm': messages_flag = 1;  break;
    case 's': signals_flag = 1;   break;
    case 't': valtables_flag = 1; break;
    case 'h': help(); exit(0);    break;
    case '?':
      /* getopt_long already printed an error message. */
      fprintf(stderr, "Typ `dbcls --help' for more information\n");
      abort();
      break;
    default:
      abort();
      break;
    }
  }

  if(NULL != (dbc = dbc_read_file(filename))) {
    if(networks_flag)  show_dbc_network(dbc);
    if(messages_flag)  show_dbc_message_list(dbc->message_list);
    if(signals_flag)   show_dbc_signals(dbc);
    if(nodes_flag)     show_dbc_nodes(dbc);
    if(envvars_flag)   show_dbc_envvars(dbc->envvar_list);
    if(valtables_flag) show_dbc_valtable_list(dbc->valtable_list);
    dbc_free(dbc);
  }

  return 0;
}

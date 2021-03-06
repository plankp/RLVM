/**
 * MIT License
 *
 * Copyright (c) 2016 Paul Teng
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "rasm.h"
#include "rlvm.h"
#include "bcode.h"
#include "getopt.h"

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif /* !__cplusplus */

int
main (int argc, char **argv)
{
  bool compile = false;
  bool run = false;
  bool dasm = false;
  size_t num_inf = 0;
  char **inf = NULL;
  char *outf = NULL;

  int c;
  while ((c = getopt (argc, argv, "crdo:h")) != -1)
    switch (c)
      {
      case 'c':
	compile = true;
	break;
      case 'r':
	run = true;
	break;
      case 'd':
	dasm = true;
	break;
      case 'o':
	outf = optarg;
	break;
      case 'h':
      print_help_msg:
	printf ("Usage: rlvm [options] file...\n"
		"Options:\n"
		"  -c    Compile assembly file (not used with -d)\n"
		"  -r    Executes a bytecode (or assembly file if -c is used)\n"
		"  -d    Disassembles a bytecode (not used with -c)\n"
		"  -o    Output file (only used with -c or -d)\n"
		"  -h    Displays help\n"
		"\n"
		"-c will not print to the console if -o is not specified.\n"
		"However, -d will print to the console if -o is not present."
		"\n"
		"For bug reporting, go to\n"
		"<https://github.com/plankp/rlvm>.");
	return 1;
      default:
	goto print_help_msg;
      }
  inf = argv + optind;
  num_inf = argc - optind;

  if (num_inf == 0)
    {
      fprintf (stderr, "error: no input files\n");
      return 2;
    }
  if (compile && dasm)
    {
      fprintf (stderr, "error: -c cannot be used with -d\n");
      return 2;
    }

  bcode_t code;
  if (compile)
    {
      /*
       * Compile to buffer first. If outf
       * is NULL, that means we do not save
       * the compiled binary.
       */
      FILE *files[num_inf];
      size_t i;
      for (i = 0; i < num_inf; ++i)
	{
	  FILE *f = fopen (inf[i], "r");
	  if (f == NULL)
	    {
	      fprintf (stderr, "error: failed to read file %s\n", inf[i]);
	      return 2;
	    }
	  files[i] = f;
	}
      code = assemble (files, num_inf);
      for (i = 0; i < num_inf; ++i)
	fclose (files[i]);

      if (outf != NULL)
	{
	  FILE *f = fopen (outf, "wb");
	  if (f == NULL)
	    {
	      fprintf (stderr, "error: failed to open file %s\n", outf);
	      return 2;
	    }
	  if (!write_bytecode (f, &code))
	    {
	      fprintf (stderr, "error: failed to write file %s\n", outf);
	      return 2;
	    }
	  fflush (f);
	  fclose (f);
	}
    }

  if (dasm)
    {
      bcode_t files[num_inf];
      size_t i;
      for (i = 0; i < num_inf; ++i)
	{
	  FILE *f = fopen (inf[i], "r");
	  if (f == NULL)
	    {
	      fprintf (stderr, "error: failed to read file %s\n", inf[i]);
	      return 2;
	    }
	  if (read_bytecode (f, &files[i]) == NULL)
	    {
	      fprintf (stderr,
		       "error: failed interpreting bytecode from %s\n", *inf);
	      return 3;
	    }
	  fclose (f);
	  if (i == 0)
	    code = files[i];
	}

      FILE *f;
      if (outf == NULL)
	f = stdout;
      else
	{
	  f = fopen (outf, "w");
	  if (f == NULL)
	    {
	      fprintf (stderr, "error: failed to open file %s\n", outf);
	      return 2;
	    }
	}

      if ((i = disassemble (files, num_inf, f)) != 0)
	return i;
      fflush (f);
      if (f != stdout) /* Don't close stdout. Maybe -r needs to print to it */
	fclose (f);
      for (i = 1; i < num_inf; ++i) /* Leave i=0 for -r */
        clean_bcode (&files[num_inf]);
    }

  if (run)
    {
      if (!(compile || dasm))
	{
	  /* Read and initalize $code */
	  FILE *f = fopen (*inf, "rb");
	  if (read_bytecode (f, &code) == NULL)
	    {
	      fprintf (stderr,
		       "error: failed interpreting bytecode from %s\n", *inf);
	      return 3;
	    }
	  fclose (f);
	}

      rlvm_t vm;
      const status_t retval = exec_bcode_t (&vm, &code);
      clean_rlvm (&vm);
      clean_bcode (&code);
      return retval.state;
    }
  return 0;
}

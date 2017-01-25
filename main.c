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

#include "rlvm.h"
#include "bcode.h"

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
#include <stdbool.h>

extern "C"
{
#endif /* !__cplusplus */

  /* This is from bison */
  extern bcode_t assemble (FILE *f);

#ifdef __cplusplus
}
#endif /* !__cplusplus */

int
main (int argc, char **argv)
{
  bool compile = false;
  bool run = false;
  char *inf = NULL;
  char *outf = NULL;

  opterr = 0;

  int c;
  while ((c = getopt (argc, argv, "cro:h")) != -1)
    switch (c)
      {
      case 'c':
	compile = true;
	break;
      case 'r':
	run = true;
	break;
      case 'o':
	outf = optarg;
	break;
      case 'h':
	printf ("Usage: rlvm [options] file\n"
		"Options:\n"
		"  -c    Compile assembly file\n"
		"  -r    Executes a bytecode (or assembly file if -c is used)\n"
		"  -o    Output file (only used with -c)\n"
		"  -h    Displays help\n"
		"\n"
		"For bug reporting, go to\n"
		"<https://github.com/plankp/rlvm>.");
        return 1;
      default:
	abort ();
      }
  inf = argv[optind];

  bcode_t code;
  if (compile)
    {
      /*
       * Compile to buffer first. If outf
       * is NULL, that means we do not save
       * the compiled binary.
       */
      FILE *f = fopen (inf, "r");
      if (f == NULL)
	{
	  fprintf (stderr, "Failed to read file %s\n", inf);
	  return 2;
	}
      code = assemble (f);
      fclose (f);

      if (outf != NULL)
	{
	  f = fopen (outf, "wb");
	  if (f == NULL)
	    {
	      fprintf (stderr, "Failed to open file %s\n", outf);
	      return 2;
	    }
	  if (!write_bytecode (f, &code))
	    {
	      fprintf (stderr, "Failed to write file %s\n", outf);
	      return 2;
	    }
	  fclose (f);
	}
    }

  if (run)
    {
      if (!compile)
	{
	  /* Read and initalize $code */
	  FILE *f = fopen (inf, "rb");
	  if (read_bytecode (f, &code) == NULL)
	    {
	      fprintf (stderr, "Failed interpreting bytecode from %s\n", inf);
	      return 3;
	    }
	  fclose (f);
	}

      rlvm_t vm;
      status_t retval = exec_bcode_t (&vm, &code);
      if (retval.state != CLEAN)
	printf ("VM EXITED WITH NON-ZERO VALUE! (%d: %d)\n", retval.state,
		retval.uid);

      print_rlvm_state (&vm);
      clean_rlvm (&vm);
      clean_bcode (&code);
    }
  return 0;
}

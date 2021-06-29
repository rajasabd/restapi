/*
 * crun - OCI runtime written in C
 *
 * Copyright (C) 2017, 2018, 2019 Giuseppe Scrivano <giuseppe@scrivano.org>
 * crun is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * crun is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with crun.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include "libcrun/container.h"
#include "libcrun/utils.h"
#include "crun_temp.h"

static char doc[] = "OCI runtime";

struct spec_options_s
{
  bool rootless;
};

enum
{
  OPTION_ROOTLESS = 1000
};

static struct spec_options_s spec_options;

int crunCmdSpec (struct crun_global_arguments *global_args, int argc, char **argv, libcrun_error_t *err)
{
  int first_arg;
  libcrun_context_t crun_context = {
    0,
  };
  int ret;
  cleanup_file FILE *f = NULL;

  ret = init_libcrun_context (&crun_context, argv[first_arg], global_args, err);
  if (UNLIKELY (ret < 0))
    return ret;

  ret = access ("config.json", 1);
  if (ret == 0)
    return libcrun_make_error (err, 0, "config.json already exists", err);

  f = fopen ("config.json", "w+");
  if (f == NULL)
    return libcrun_make_error (err, 0, "cannot open config.json", err);

  ret = libcrun_container_spec (! spec_options.rootless, f, err);

  return ret >= 0 ? 0 : ret;
}

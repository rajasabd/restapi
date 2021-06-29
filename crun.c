#include <stdio.h>

#include <stddef.h>
#include "libcrun/container.h"
#include "libcrun/utils.h"
#include <crun_temp.h>

char * argp_mandatory_argument (char *arg, struct argp_state *state)
{
  if (arg)
    return arg;
  return state->argv[state->next++];
}

int init_libcrun_context (libcrun_context_t *con, const char *id, struct crun_global_arguments *glob, libcrun_error_t *err)
{
  int ret;

  con->id = id;
  con->state_root = glob->root;
  con->systemd_cgroup = glob->option_systemd_cgroup;
  con->force_no_cgroup = glob->option_force_no_cgroup;
  con->notify_socket = getenv ("NOTIFY_SOCKET");
  con->fifo_exec_wait_fd = -1;

  ret = libcrun_init_logging (&con->output_handler, &con->output_handler_arg, id, glob->log, err);
  if (UNLIKELY (ret < 0))
    return ret;

  if (glob->log_format)
    {
      ret = libcrun_set_log_format (glob->log_format, err);
      if (UNLIKELY (ret < 0))
        return ret;
    }

  if (con->bundle == NULL)
    con->bundle = ".";

  if (con->config_file == NULL)
    con->config_file = "./config.json";

  return 0;
}

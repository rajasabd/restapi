/**
 * 
 * Ulfius Framework example program
 * 
 * This example program describes the main features 
 * that are available in a callback function
 * 
 * Copyright 2015-2017 Nicolas Mora <mail@babelouest.org>
 * 
 * License MIT
 *
 */

#include <string.h>
#include <errno.h>

#include <ulfius.h>
#include <u_example.h>
#include <jansson.h>
#include <spec_1.h>

#define PORT 8537
#define PREFIX "/podstate"

/**
 * callback functions declaration
 */
int callback_get_pod_status (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_post_pod_state (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_default (const struct _u_request * request, struct _u_response * response, void * user_data);

static struct crun_global_arguments arguments;


/**
 * decode a u_map into a string
 */
char * print_map(const struct _u_map * map) {
  char * line, * to_return = NULL;
  const char **keys, * value;
  int len, i;
  if (map != NULL) {
    keys = u_map_enum_keys(map);
    for (i=0; keys[i] != NULL; i++) {
      value = u_map_get(map, keys[i]);
      len = snprintf(NULL, 0, "key is %s, value is %s", keys[i], value);
      line = o_malloc((len+1)*sizeof(char));
      snprintf(line, (len+1), "key is %s, value is %s", keys[i], value);
      if (to_return != NULL) {
        len = o_strlen(to_return) + o_strlen(line) + 1;
        to_return = o_realloc(to_return, (len+1)*sizeof(char));
        if (o_strlen(to_return) > 0) {
          strcat(to_return, "\n");
        }
      } else {
        to_return = o_malloc((o_strlen(line) + 1)*sizeof(char));
        to_return[0] = 0;
      }
      strcat(to_return, line);
      o_free(line);
    }
    return to_return;
  } else {
    return NULL;
  }
}

char * read_file(const char * filename) {
  char * buffer = NULL;
  long length;
  FILE * f = fopen (filename, "rb");
  if (f != NULL) {
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    buffer = o_malloc (length + 1);
    if (buffer != NULL) {
      fread (buffer, 1, length, f);
      buffer[length] = '\0';
    }
    fclose (f);
  }
  return buffer;
}

int main (int argc, char **argv) {
  int ret;
  
  // Set the framework port number
  struct _u_instance instance;
  
  y_init_logs("server", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting rest server");
  
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error ulfius_init_instance, abort");
    return(1);
  }
  
  u_map_put(instance.default_headers, "Access-Control-Allow-Origin", "*");
  
  // Maximum body size sent by the client is 1 Kb
  instance.max_post_body_size = 1024;
  
  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "GET", PREFIX, NULL, 0, &callback_get_pod_status, NULL);
  ulfius_add_endpoint_by_val(&instance, "POST", PREFIX, NULL, 0, &callback_post_pod_state, NULL);
  
  // default_endpoint declaration
  ulfius_set_default_endpoint(&instance, &callback_default, NULL);
  
  // Start the framework
  if (argc == 4 && o_strcmp("-secure", argv[1]) == 0) {
    // If command-line options are -secure <key_file> <cert_file>, then open an https connection
    char * key_pem = read_file(argv[2]), * cert_pem = read_file(argv[3]);
    ret = ulfius_start_secure_framework(&instance, key_pem, cert_pem);
    o_free(key_pem);
    o_free(cert_pem);
  } else {
    // Open an http connection
    ret = ulfius_start_framework(&instance);
  }
  
  if (ret == U_OK) {
    y_log_message(Y_LOG_LEVEL_DEBUG, "Start %sframework on port %d", ((argc == 4 && o_strcmp("-secure", argv[1]) == 0)?"secure ":""), instance.port);
    
    // Wait for the user to press <enter> on the console to quit the application
    getchar();
  } else {
    y_log_message(Y_LOG_LEVEL_DEBUG, "Error starting framework");
  }
  y_log_message(Y_LOG_LEVEL_DEBUG, "End framework");
  
  y_close_logs();
  
  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);
  
  return 0;
}

/**
 * Callback function that to get the pod status in the response
 */
int callback_get_pod_status (const struct _u_request * request, struct _u_response * response, void * user_data) 
{
  ulfius_set_string_body_response(response, 200, "pod status");
  
  return U_CALLBACK_CONTINUE;
}

 char *argv1[4] = { "create","--console-socket" , "/home/aahanad/test2.sock","busybox1"};
  
/**
 * Callback function that create, start, run and the pod  and send the pod status to the client in the response
 */
int callback_post_pod_state (const struct _u_request * request, struct _u_response * response, void * user_data) {
  libcrun_error_t err =NULL;
  int ret, first_argument = 0;

  json_t * json_pod_state = ulfius_get_json_body_request(request, NULL);
  json_t * json_body = NULL;
 
  json_int_t * nb_sheep = user_data;
  //
  if (json_pod_state != NULL) 
  {
    if(json_boolean_value(json_object_get(json_pod_state,"spec")))
    {
        // call spec
        y_log_message(Y_LOG_LEVEL_DEBUG, "spec command is received");

    }
    if(json_boolean_value(json_object_get(json_pod_state,"create")))
    {
        // call create
        y_log_message(Y_LOG_LEVEL_DEBUG, "create command is received");

        crunCmdCreate (&arguments, 4, argv1, &err);
    }
  } 
  else 
  {
    * nb_sheep = 0;
  }
  
  //command response to client
  json_body = json_object();
  json_object_set_new(json_body, "pod status", json_integer(10));
  ulfius_set_json_body_response(response, 200, json_body);
  json_decref(json_pod_state);
  json_decref(json_body);

  return U_CALLBACK_CONTINUE;
}


/**
 * Default callback function called if no endpoint has a match
 */
int callback_default (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 404, "Page not found, do what you want");
  return U_CALLBACK_CONTINUE;
}

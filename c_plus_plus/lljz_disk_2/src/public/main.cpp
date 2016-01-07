#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <tbsys.h>
#include <tbnet.h>

#include "shared.h"
#include "server_entry.h"

lljz::disk::IServerEntry* g_iserver_entry = NULL;

void sign_handler(int sig) {
  switch (sig) {
  case SIGTERM:
  case SIGINT:
    if(g_iserver_entry != NULL) {
      g_iserver_entry->stop();
    }
    break;
  case 40:
    TBSYS_LOGGER.checkFile();
    break;
  case 41:
  case 42:
    if(sig == 41) {
      TBSYS_LOGGER._level++;
    }
    else {
      TBSYS_LOGGER._level--;
    }
    TBSYS_LOG(INFO,"TBSYS_LOGGER._level: %d", TBSYS_LOGGER._level);
    break;
  }
}

void print_usage(char *prog_name)
{
  fprintf(stderr, "%s\n"
          "    -h, --help         this help\n"
          "    -V, --version      version\n\n", prog_name);
}

void parse_cmd_line(int argc, char *const argv[])
{
  int opt;
  const char* opt_string = "hV:";
  struct option longopts[] = {
    {"help", 0, NULL, 'h'},
    {"version", 0, NULL, 'V'},
    {0, 0, 0, 0}
  };

  while((opt = getopt_long(argc, argv, opt_string, longopts, NULL)) != -1) {
    switch (opt) {
    case 'V':
      fprintf(stderr, "BUILD_TIME: %s %s\n", __DATE__, __TIME__);
      exit(1);
    case 'h':
      print_usage(argv[0]);
      exit(1);
    }
  }
}

int main(int argc, char *argv[])
{
  // parse cmd
  parse_cmd_line(argc, argv);
  char config_file[64];
  char* prog_name=strrchr(argv[0],'/');
  if (NULL!=prog_name) {
    prog_name++;
  } else {
    prog_name=argv[0];
  }
  sprintf(config_file,"./%s.ini",prog_name);
  if(TBSYS_CONFIG.load(config_file)) {
    fprintf(stderr, "load file %s error\n", config_file);
    return EXIT_FAILURE;
  }

  char value[64];
  sprintf(value,"%s.pid",prog_name);
  const char* sz_pid_file = TBSYS_CONFIG.getString(
    "server", "pid_file", value);
  sprintf(value,"%s.log",prog_name);
  const char* sz_log_file = TBSYS_CONFIG.getString(
    "server", "log_file", value);
  if(1) {
    char* p, dir_path[256];
    sprintf(dir_path, "%s", sz_pid_file);
    p = strrchr(dir_path, '/');
    if(p != NULL)
      *p = '\0';
    if(p != NULL && !tbsys::CFileUtil::mkdirs(dir_path)) {
      fprintf(stderr, "create dir %s error\n", dir_path);
      return EXIT_FAILURE;
    }
    sprintf(dir_path, "%s", sz_log_file);
    p = strrchr(dir_path, '/');
    if(p != NULL)
      *p = '\0';
    if(p != NULL && !tbsys::CFileUtil::mkdirs(dir_path)) {
      fprintf(stderr, "create dir %s error\n", dir_path);
      return EXIT_FAILURE;
    }
  }

  const char* sz_log_level = TBSYS_CONFIG.getString(
    "server", "log_level", "info");
  TBSYS_LOGGER.setLogLevel(sz_log_level);

  int daemon = TBSYS_CONFIG.getInt("server", "daemon", 0);
  if (0 != daemon) {
    int pid;
    if((pid = tbsys::CProcess::existPid(sz_pid_file))) {
      fprintf(stderr, "program has been exist: pid=%d\n", pid);
      return EXIT_FAILURE;
    }

    if(tbsys::CProcess::startDaemon(sz_pid_file, sz_log_file) != 0) {
      fprintf(stderr, "start daemon fail");
      return EXIT_FAILURE;
    }
  }

  signal(SIGPIPE, SIG_IGN);
  signal(SIGHUP, SIG_IGN);
  signal(SIGINT, sign_handler);
  signal(SIGTERM, sign_handler);
  signal(40, sign_handler);
  signal(41, sign_handler);
  signal(42, sign_handler);

  g_iserver_entry = new lljz::disk::ServerEntry();
  g_iserver_entry->start();

  // ignore signal when destroy, cause sig_handler may use g_iserver_entry between delete and set it to NULL.
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  delete g_iserver_entry;
  g_iserver_entry = NULL;

  TBSYS_LOG(ERROR,"%s\n","exit program.");

  return EXIT_SUCCESS;
}

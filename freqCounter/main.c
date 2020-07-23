/**
* @file main.c
* @author Thainá Braz, Pedro Moreira
* @number 2161902, 2161829
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <regex.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <math.h>

#include "freq.h"


#include "args.h"
#include "debug.h"
#include "memory.h"


#define ERR_ARGS 1
#define ERR_SYS_CALL 3

void iterate_folder(int argc, char *argv[], char *path);
void write_file(char *filename);

int main(int argc, char * argv[]) {

  //Inicializing variables	
  const char delimitator[2] = ",";
  char * dirname = NULL;
  clock_t begin;
  struct gengetopt_args_info args;

  if (cmdline_parser(argc, argv, & args) != 0) {
    exit(1);
  }

  if (args.time_given) {
    //begins the clock counter --to count the execution time--
    begin = clock();

  }

  //validation of the -m parameter
  if (args.mode_given) {
    if (!(args.mode_arg == 1 || args.mode_arg == 2 || args.mode_arg == 4)) {
      ERROR(2, "Invalid value '%d' for -m/mode.\n", args.mode_arg);

    }
  }
  //setting dir and file as required (if dir is given file is optional or if file is given dir is automatically optional)
  if (!(args.dir_given || args.file_given)) {
    ERROR(3, "file 'FILE' and/or directory ‘DIR’ are required \n");

  }
  //setting compact and discrete option conflicts
  if (args.compact_given && args.discrete_given) {
    ERROR(4, " option -c/--compact conflicts with option --discrete \n");
  }
  //setting compact and search option conflicts
  if (args.compact_given && args.search_given) {
    ERROR(5, " option -c/--compact conflicts with option -s/--search \n");
  }
  //setting discrete and search option conflicts
  if (args.discrete_given && args.search_given) {
    ERROR(6, " option --discrete conflicts with option -s/--search \n");
  }

  if (args.output_given) {
    //When output is given executes the function bellow
    write_file(args.output_arg);

  }

  //initializing aux vector to saves dir names passed through parameters (max 100 directories)
  char * v_aux[100];
  //dir counter
  int aux = 0;

  if (args.dir_given) {

    if (args.dir_arg[strlen(args.dir_arg) - 1] == '/') {
      args.dir_arg[strlen(args.dir_arg) - 1] = 0;
    }
    //function strtok used to set parameters ex:<dir1,dir2,dir3> into the vector 'v_aux'
    dirname = strtok(args.dir_arg, delimitator);
    while (dirname != NULL) {

      v_aux[aux] = dirname;
      aux++;

      dirname = strtok(NULL, delimitator);
    }
    for (int i = 0; i < aux; i++) {
      //It opens every dir from the vector 'v_aux'
      iterate_folder(argc, argv, v_aux[i]);
    }

  }
  if (args.file_given) {
    if (args.file_arg[strlen(args.file_arg) - 1] == '/') {
      args.file_arg[strlen(args.file_arg) - 1] = 0;
    }
    //function to opens and count files byte-to-byte
    bytesCounter(argc, argv, args.file_arg);
  }
  //Simultaneously treats file and dir when passed through parameters
  if ((args.dir_given && args.file_given)) {

    if (args.dir_arg[strlen(args.dir_arg) - 1] == '/') {
      args.dir_arg[strlen(args.dir_arg) - 1] = 0;
    }

    dirname = strtok(args.dir_arg, delimitator);
    while (dirname != NULL) {

      v_aux[aux] = dirname;
      aux++;

      dirname = strtok(NULL, delimitator);
    }
    for (int i = 0; i < aux; i++) {

      iterate_folder(argc, argv, v_aux[i]);
    }
    if (args.file_arg[strlen(args.file_arg) - 1] == '/') {
      args.file_arg[strlen(args.file_arg) - 1] = 0;
    }
    bytesCounter(argc, argv, args.file_arg);

  }
  //If time is given it counts the execution time and print the results in seconds
  if (args.time_given) {
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time:%f seconds\n", time_spent);
  }
  //free parser
  cmdline_parser_free( & args);

  return 0;
}

void iterate_folder(int argc, char * argv[], char * path) {
  DIR * dir = opendir(path);
  if (dir == NULL) {
    ERROR(ERR_SYS_CALL, "Cannot access directory ‘%s’", path);
  }

  regex_t regex;
  if (regcomp( & regex, "^simulation-[[:digit:]]\\{8\\}-[[:digit:]]\\{6\\}-0", 0) != 0) {
    ERROR(ERR_SYS_CALL, "regcomp() failed");
  }

  struct dirent * entry;
  //char * file_name;
  while ((entry = readdir(dir)) != NULL) {

    //process_file(entry->d_name);

    if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
      //do nothing
    } else {
      printf("DIR: '%s'\n", path);
      bytesCounter(argc, argv, entry->d_name);
    }

  }
  closedir(dir);

}
void write_file(char * filename) {
  int fd = open(filename, O_WRONLY | O_CREAT);

  if (fd == -1) {
    ERROR(2, "\nInsucesso na abertura %s\n", filename);
  } else {
    freopen(filename, "a+", stdout);

  }

  close(fd);

}
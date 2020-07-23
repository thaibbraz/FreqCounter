#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freq.h"
#include "debug.h"
#include "memory.h"
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>

#include <unistd.h>

#include "args.h"


void bytesCounter(int argc, char * argv[], char * file) {
  //Initializing parameters
  int mode;
  char * filename = NULL;
  //discrete flag
  int flag = 0;
  const char delimitator[2] = ",";
  struct gengetopt_args_info args;
  //vector used in discrete mode <--discrete>
  int vetorDiscrete[100];

  if (cmdline_parser(argc, argv, & args) != 0) {
    exit(1);
  }

  //-m parameter input saved into variable 'mode'
  mode = args.mode_arg;

  //if discrete is given as param then it puts all values passed into the vector 'vetorDiscrete'
  if (args.discrete_given) {
    flag = 1; //flag to know that program it's in the discrete mode
    for (int i = 0; i < (int) args.discrete_given; ++i) {
      vetorDiscrete[i] = args.discrete_arg[i];
      printf(" %d ", vetorDiscrete[i]);

    }

  }

  //Settling down filename(s) passed by parameters
  filename = strtok(file, delimitator);

  //walk through vector of parameter files
  while (filename != NULL) {

    size_t i = 0;
    int count = 0;

    //Opening file 
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
      ERROR(3, "%s: CANNOT PROCESS FILE \n", filename);

    }

    //Getting the size of the file
    off_t ls = lseek(fd, 0, SEEK_END);
    if (ls == -1) {
      ERROR(4, "Invalid seek offset\n");
    }

    size_t fileSize = (size_t) ls;

    //Placing pointer on top of the file
    ls = lseek(fd, 0, SEEK_SET);
    if (ls == -1) {
      ERROR(5, "Invalid seek offset\n");
    }

    u_int32_t buf;
    u_int32_t vetor[fileSize + 1];

    switch (mode) {
    case 2:
      //MODE=2 u_int16_t
      while ((unsigned) i < fileSize) {

        ssize_t rd = read(fd, & 
        
  
        
        
        , sizeof(u_int16_t));
        if (rd == -1) {
          ERROR(6, "Can't read from file %s\n", filename);
        } else {

          //Saving bytes into the vector 'vetor'
        vetor[i]=(u_int16_t)vetor[i];
        vetor[i]=(u_int16_t)buf;

          count++;

        }
        i++;
      }
      break;
    case 4:
      //MODE=4 u_int32_t
      while ((unsigned) i < fileSize) {

        ssize_t rd = read(fd, & buf, sizeof(u_int32_t));
        if (rd == -1) {
          ERROR(6, "Can't read from file %s\n", filename);
        } else {

          //Saving bytes on the vector
          vetor[i] = buf;
          count++;

        }
        i++;
      }
      break;
    default:
      //MODE=1 u_int8_t	
      //Go through file (byte to byte) until the last byte
      while ((unsigned) i < fileSize) {

        ssize_t rd = read(fd, & buf, sizeof(buf));
        if (rd == -1) {
          ERROR(6, "Can't read from file %s\n", filename);
        } else {

          //Saving bytes into the vector
         vetor[i]=(u_int8_t)vetor[i];
         vetor[i]=(u_int8_t)buf;

        }
        i++;
      }
      break;
    }

    //sorting vector
    int temp = 0;
    for (int i = 0; i < count - 1; ++i) {
      for (int j = i + 1; j < count; ++j) {
        if (vetor[j] < vetor[i]) {
          temp = vetor[i];
          vetor[i] = vetor[j];
          vetor[j] = temp;
        }
      }
    }

    if (args.compact_given) {
      //Output for compact mode					
      printf("%s:%ldbytes:", filename, fileSize);

    } else {
      //Default output
      printf("freqCounter:'%s':%lu bytes\n", filename, fileSize);
    }
    int aux = fileSize;
  
    if (flag == 1) {
      aux = args.discrete_given;
    }

    for (int i = 0; i < (int) aux; i++) {

      // looks to the left in the array if the number was used before
      int found = 0;

      for (int j = 0; j < i; j++) {

        if (flag == 0) {
          if (vetor[i] == vetor[j]) {
            found++;

          }

        }

      }
      // goes on if it's the first occurance
      if (found == 0) {
        //we know of one occurance
        int freq = 1;

        //looks to the right in the array for other occurances
        int j = 0;
        if (flag == 1) {
          freq = 0;
          for (j = 0; j < (int) fileSize; j++) {
            if (vetorDiscrete[i]==  (int)vetor[j]) {
              freq++;

            }

          }
        } else {

          for (j = i + 1; j < (int) fileSize; j++) {
            if (vetor[i] == vetor[j]) {

              freq++;

            }

          }
        }

        if (args.compact_given) {

          printf("%d", freq);
          printf(",");

        } else {

          switch (mode) {
          case 2:

            if (flag == 1) {
              printf("bi-byte: %0*d:%d", 5, vetorDiscrete[i], freq);

            } else {
              printf("bi-byte: %0*d:%d", 5, vetor[i], freq);
            }
            printf("\n");
            break;
          case 4:
            if (flag == 1) {
              printf("quad-byte: %0*d:%d", 10, vetorDiscrete[i], freq);
            } else {
              printf("quad-byte: %0*d:%d", 10, vetor[i], freq);
            }

            printf("\n");
            break;

          default:
            if (flag == 1) {
              printf("byte: %0*d:%d", 3, vetorDiscrete[i], freq);

            } else {
              printf("byte: %0*d:%d", 3, vetor[i], freq);
            }
            printf("\n");
            break;
          }

        }

      }

    }

    printf("----------\n");

    //fechar ficheiro
    close(fd);
    filename = strtok(NULL, delimitator);
  }
  cmdline_parser_free( & args);

}
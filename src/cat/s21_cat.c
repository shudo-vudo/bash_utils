#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

long file_size(FILE *in);
void print_number(int *counter);
int inv_char_print(const char *simvol, int *flag_etv);

int main(int argc, char *argv[]) {
  int number_noblank = 0, number = 0, show_tabs = 0, show_endlines = 0,
      squeeze = 0, show_tabs_out_v = 0, show_endlines_out_v = 0,
      invisible_characters = 0;

  FILE *fin;
  const char *short_options = "+benstvET";
  const struct option long_options[] = {
      {"number-nonblank", 0, NULL, 'b'},
      {"number", 0, NULL, 'n'},
      {"squeeze-blank", 0, NULL, 's'},
      {NULL, 0, NULL, 0},
  };
  int flag = 0;
  while ((flag = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (flag) {
      case 'b': {
        number_noblank = 1;
        number = 0;
        break;
      }
      case 'e': {
        show_endlines = 1;
        break;
      }
      case 'n': {
        if (number_noblank == 1) {
          number = 0;
        } else {
          number = 1;
        }
        break;
      }
      case 's': {
        squeeze = 1;
        break;
      }
      case 't': {
        show_tabs = 1;
        break;
      }
      case 'E': {
        show_endlines_out_v = 1;
        break;
      }
      case 'T': {
        show_tabs_out_v = 1;
        break;
      }
      case 'v': {
        invisible_characters = 1;
        break;
      }
      case '?': {
        fprintf(stderr, "usage: cat [-benstv] [file ...]\n");
        return 0;
      }
    }
  }
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      fin = fopen(argv[i], "r");
      if (fin == NULL) {
        fprintf(stderr, "cat: %s: No such file or directory\n", argv[i]);
      } else {
        long size = file_size(fin);
        char *infa_masiv = (char *)malloc(size);
        char simvol_masiva;
        int flag_combo_etv = 0;
        int line_count = 1;
        int j = 0;
        while (fscanf(fin, "%c", &simvol_masiva) != EOF) {
          int allow_print = 1;
          if (squeeze == 1 && j > 1 && infa_masiv[j - 2] == '\n' &&
              infa_masiv[j - 1] == '\n' && simvol_masiva == '\n') {
            allow_print = 0;
          }
          if (number == 1 && j == 0) {
            print_number(&line_count);
          } else if ((number == 1 && j != 0 && infa_masiv[j - 1] == '\n') &&
                     allow_print == 1) {
            print_number(&line_count);
          }
          if (simvol_masiva != '\n' && j == 0 && number_noblank == 1) {
            print_number(&line_count);
          } else if (j != 0 && infa_masiv[j - 1] == '\n' &&
                     simvol_masiva != '\n' && number_noblank == 1) {
            print_number(&line_count);
          }
          if (invisible_characters == 1 && allow_print == 1) {
            allow_print = inv_char_print(&simvol_masiva, &flag_combo_etv);
          }
          if (show_tabs == 1 && allow_print == 1) {
            allow_print = inv_char_print(&simvol_masiva, &flag_combo_etv);
            if (simvol_masiva == '\t') {
              printf("^I");
              allow_print = 0;
            }
          }
          if (show_tabs_out_v == 1 && simvol_masiva == '\t') {
            printf("^I");
            allow_print = 0;
          }
          if (show_endlines == 1 && allow_print == 1) {
            if (simvol_masiva == '\n') {
              printf("$");
            }
            allow_print = inv_char_print(&simvol_masiva, &flag_combo_etv);
          }
          if ((show_endlines_out_v == 1) && (simvol_masiva == 10)) {
            printf("$");
          }
          if (allow_print == 1) {
            printf("%c", simvol_masiva);
          }
          infa_masiv[j] = simvol_masiva;
          j++;
          flag_combo_etv = 0;
        }
        free(infa_masiv);
        fclose(fin);
      }
    }
  }
  return 0;
}

long file_size(FILE *in) {
  fseek(in, 0, SEEK_END);
  long size = ftell(in);
  fseek(in, 0, SEEK_SET);
  return size;
}

void print_number(int *counter) {
  printf("%6d\t", *counter);
  *counter = *counter + 1;
}

int inv_char_print(const char *simvol, int *flag_etv) {
  int coun = 1;
  if ((*simvol >= 0) && (*simvol <= 31) && (*simvol != 10) && (*simvol != 9) &&
      *flag_etv == 0) {
    printf("^%c", *simvol + 64);
    coun = 0;
    *flag_etv = 1;
  } else if (*simvol == 127 && *flag_etv == 0) {
    printf("^?");
    coun = 0;
    *flag_etv = 1;
  }
  return coun;
}

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e;
  char *templates;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int o_rip;
} flag_opt;

void parcer(int argc, char **argv, flag_opt *opt);
void e_template(flag_opt *opt);
void f_templates(flag_opt *opt);
void output(FILE *fp, flag_opt *opt, int argc, char **argv, int j);
void o_flag(char *str, regex_t reg, regmatch_t *pmatch);

int main(int argc, char **argv) {
  flag_opt opt = {0};
  if (argc > 2) {
    opt.templates = malloc(2 * sizeof(char));
    opt.templates[0] = '\0';
    parcer(argc, argv, &opt);
    for (int i = optind; i < argc; i++) {
      FILE *in;
      if ((in = fopen(argv[i], "r")) == NULL) {
        if (!opt.s) {
          fprintf(stderr, "grep: %s: No such file or directory\n", argv[i]);
        }
      } else {
        output(in, &opt, argc, argv, i);
        fclose(in);
      }
    }
    free(opt.templates);
  }
  return 0;
}

void parcer(int argc, char **argv, flag_opt *opt) {
  int flag;
  while ((flag = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (flag) {
      case 'e':
        e_template(opt);
        break;
      case 'i':
        opt->i = 1;
        break;
      case 'v':
        opt->v = 1;
        break;
      case 'c':
        opt->c = 1;
        break;
      case 'l':
        opt->l = 1;
        break;
      case 'n':
        opt->n = 1;
        break;
      case 'h':
        opt->h = 1;
        break;
      case 's':
        opt->s = 1;
        break;
      case 'f':
        f_templates(opt);
        break;
      case 'o':
        opt->o = 1;
        break;
      default:
        if (opt->templates) {
          free(opt->templates);
        }
        exit(1);
        break;
    }
  }
  if (!(opt->e)) {
    if (strcmp(argv[optind], "") == 0) {
      opt->o_rip = 1;
      opt->templates = strcat(opt->templates, ".");
      optind++;
    } else {
      opt->templates = realloc(opt->templates, strlen(argv[optind]) + 1);
      opt->templates = strcat(opt->templates, argv[optind++]);
    }
  } else if (opt->templates[strlen(opt->templates) - 1] == '|') {
    opt->templates[strlen(opt->templates) - 1] = '\0';
  }
}

void e_template(flag_opt *opt) {
  opt->e = 1;
  if (strcmp(optarg, "") == 0) {
    opt->o_rip = 1;
    opt->templates = realloc(opt->templates, strlen(opt->templates) + 3);
    opt->templates = strcat(opt->templates, ".");
  } else {
    opt->templates =
        realloc(opt->templates, strlen(opt->templates) + strlen(optarg) + 2);
    opt->templates = strcat(opt->templates, optarg);
  }
  opt->templates = strcat(opt->templates, "|");
}

void f_templates(flag_opt *opt) {
  opt->e = 1;
  if (opt->f) {
    opt->templates = realloc(opt->templates, strlen(opt->templates) + 2);
    opt->templates = strcat(opt->templates, "|");
  }
  opt->f = 1;
  FILE *fin = NULL;
  if ((fin = fopen(optarg, "r")) != NULL) {
    char ch, prev;
    for (prev = '|'; (ch = getc(fin)) != EOF; prev = ch) {
      if (ch == '\n') {
        ch = '|';
        if (prev == ch) {
          opt->o_rip = 1;
          opt->templates = realloc(opt->templates, strlen(opt->templates) + 2);
          opt->templates = strcat(opt->templates, ".");
        }
      }
      opt->templates = realloc(opt->templates, strlen(opt->templates) + 2);
      opt->templates[strlen(opt->templates)] = ch;
      opt->templates[strlen(opt->templates) + 1] = '\0';
    }
    fclose(fin);
    if (opt->templates[strlen(opt->templates) - 1] == '|') {
      opt->templates[strlen(opt->templates) - 1] = '\0';
    }
  } else {
    fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
    if (opt->templates) {
      free(opt->templates);
    }
    exit(1);
  }
}

void output(FILE *fin, flag_opt *opt, int argc, char **argv, int j) {
  size_t len = 0;
  regex_t reg;
  regmatch_t pmatch[1];
  char *str = NULL;
  int rc;
  int flag = REG_EXTENDED;
  if (opt->i) {
    flag |= REG_ICASE;
  }
  rc = regcomp(&reg, opt->templates, flag);
  if (rc == 0) {
    int l_flag = 0, number_of_line = 0, lines_count = 0;
    while (getline(&str, &len, fin) != -1) {
      number_of_line++;
      rc = regexec(&reg, str, 1, pmatch, 0);
      if ((rc == 0) && !opt->v) {
        if (opt->l || opt->c) {
          l_flag = 1;
          if (opt->c) {
            lines_count++;
            continue;
          }
          continue;
        }
        if (!opt->h && argc - optind > 1) {
          printf("%s:", argv[j]);
        }
        if (opt->n) {
          printf("%d:", number_of_line);
        }
        if (opt->o && !opt->o_rip) {
          o_flag(str, reg, pmatch);
          continue;
        }
        printf("%s", str);
        if (str[strlen(str) - 1] != '\n') {
          printf("\n");
        }
      } else if (opt->v && rc == 0) {
        if (opt->l || opt->c) {
          l_flag = 1;
          if (opt->c) {
            lines_count++;
            continue;
          }
          continue;
        }
        if (!opt->h && argc - optind > 1) {
          printf("%s:", argv[j]);
        }
        if (opt->n) {
          printf("%d:", number_of_line);
        }
        printf("%s", str);
        if (str[strlen(str) - 1] != '\n') {
          printf("\n");
        }
      }
    }

    if (opt->c) {
      if (!opt->h && argc - optind > 1) {
        printf("%s:", argv[j]);
      }
      if (opt->l && lines_count > 1) {
        lines_count = 1;
      }
      printf("%d\n", lines_count);
    }
    if (opt->l && l_flag) {
      printf("%s\n", argv[j]);
    }
  }
  free(str);
  regfree(&reg);
}

void o_flag(char *str, regex_t reg, regmatch_t *pmatch) {
  while (1) {
    if (regexec(&reg, str, 1, pmatch, 0) != 0) {
      break;
    }
    for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; ++i) {
      printf("%c", str[i]);
    }
    printf("\n");
    str += pmatch[0].rm_eo;
  }
}

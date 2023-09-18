#include <getopt.h>
#include <stdio.h>
#include <string.h>
#define N 512

int common(char *name, int chn, int chb, int vf, int s, int e, int tab);
void vflag(char *arr, int t);
int flagCheck(char *arr, int chn, int chb, int vf, int s, int tab, int *elc,
              int *n);
int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex);

extern char *optarg;
extern int optind, opterr, optopt;
const struct option long_options[] = {
    {"number-nonblank", no_argument, NULL, 'b'},
    {"number", no_argument, NULL, 'n'},
    {"squeeze-blank", no_argument, NULL, 's'}};

int main(int argc, char **argv) {
  if (argc == 1) {  // no arguments
    printf("n/a");
  } else {
    int chn = 0;
    int chb = 0;
    int vf = 0;
    int tab = 0;
    int e = 0;
    int s = 0;
    char rez;
    int index = 0;
    int i = 1;
    while (argv[i][0] == '-') {
      index++;
      i++;
    }
    int temp = 1;
    while ((rez = getopt_long(argc, argv, "bnestvTE", long_options, 0)) != -1 &&
           temp <= index) {
      if (rez == 'n') chn = 1;
      if (rez == 'b') chb = 1;
      if (rez == 'v') {
        vf = 1;
      }
      if (rez == 't') {
        vf = 1;
        tab = 1;
      }
      if (rez == 'e') {
        vf = 1;
        e = 1;
      }
      if (rez == 's') s = 1;
      if (rez == 'T') {
        tab = 1;
      }
      if (rez == 'E') e = 1;
      if (rez == '?') return -1;
      temp++;
    }
    for (i = index + 1; i < argc; i++) {
      int ret;
      char *name;
      name = argv[i];
      ret = common(name, chn, chb, vf, s, e, tab);
      if (ret == 0) {  // ne naiden file
        printf("n/a\n");
      } else if (ret == -1) {  // ne tot flag
        break;
      }
    }
  }
}
int common(char *name, int chn, int chb, int vf, int s, int e,
           int tab) {  // Main action
  int ret = 1;
  FILE *file;
  int n = 0;
  int elc = 0;
  optind = 1;
  file = fopen(name, "r");
  if (file == NULL) {
    ret = 0;
  } else {
    char arr[N];
    int i = 0;
    while ((arr[i] = fgetc(file)) != EOF) {
      if (tab == 1 && arr[i] == '\t') {
        arr[i++] = '^';
        arr[i] = 'I';
      }
      if (arr[i] == '\0' && (vf == 1)) {
        arr[i++] = '^';
        arr[i] = '@';
      }
      if (arr[i] == '\n') {
        arr[i] = '\0';
        if (flagCheck(arr, chn, chb, vf, s, tab, &elc, &n) != 2) {
          if (e == 1) {
            printf("$\n");
          } else {
            printf("\n");
          }
        }
        i = 0;
      } else {
        i++;
      }
    }
    arr[i] = '\0';
    if (arr[0] == '\0') {
      fclose(file);
      return ret;
    }
    flagCheck(arr, chn, chb, vf, s, tab, &elc, &n);
    fclose(file);
  }
  return ret;
}

void vflag(char *arr, int t) {  // Reading hidden symbols
  int len = strlen(arr);
  for (int i = 0; i < len; i++) {
    if ((arr[i] > 0 && arr[i] < 9) || (arr[i] > 10 && arr[i] < 32)) {
      printf("^%c", arr[i] + 64);
    } else if (arr[i] == 127) {
      printf("^?");
    } else if (arr[i] + 128 + 64 < 96) {
      printf("M-^%c", arr[i] + 128 + 64);
    } else if (t == 1 && arr[i] == '\t') {
      printf("^I");
    } else {
      printf("%c", arr[i]);
    }
  }
}

int flagCheck(char *arr, int chn, int chb, int vf, int s, int tab, int *elc,
              int *n) {  // Checking of the flag
  int ret = 1;
  if (s == 1 && arr[0] == '\0') {
    *elc = *elc + 1;
    if (*elc > 1) {
      ret = 2;
      return ret;
    }
  } else {
    *elc = 0;
  }
  if (chn == 1 && chb == 0) {
    *n = *n + 1;
    printf("%6d\t", *n);
  }
  if (chb == 1) {
    if (arr[0] != '\0') {
      *n = *n + 1;
      printf("%6d\t", *n);
    }
  }
  if (vf == 0) {
    printf("%s", arr);
  } else {
    vflag(arr, tab);
  }
  return ret;
}
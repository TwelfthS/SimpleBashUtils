#include <ctype.h>
#include <pcre.h>
#include <stdio.h>
#include <string.h>
#define N 1024

int common(char *template, char *name, char *flag, int mult, char **etemplate,
           int e);
// common func for all flags: pattern, name of file, flag, checker for multiple
// files, e flag templates array, number of patterns in e flag
int regex(char *pattern, char *arr);
// pattern checking
int fflag(char *arr, char *template);
// go through patterns file in f flag
int flagCheck(char *flag, char *arr, char *name, char *fname, char *template,
              int *line, int *c, int *matchCheck, int match, int fin);
// flag check: many necessary counters

int first = 0;  // checks if this is the first printed line

int main(int argc, char **argv) {
  char *name;
  char *template;
  char *etemplate[N];
  int e = 0;
  if (argc == 1) {  // no arguments
    printf("n/a");
  } else if (!strcmp(argv[1], "-e")) {    // e flag
    int t = 2;                            // counter for arguments
    template = "nothing";                 // to avoid error message
    while (!strcmp(argv[t - 1], "-e")) {  // add all templates
      etemplate[e] = argv[t];
      t = t + 2;
      e++;
    }
    for (int i = t - 1; i < argc; i++) {  // go through names
      name = argv[i];
      if (argc == t) {
        if (common(template, name, argv[1], 0, etemplate, e) == 0)
          printf("n/a\n");
      } else {
        if (common(template, name, argv[1], 1, etemplate, e) == 0 &&
            i == argc - 1)
          printf("n/a\n");  // n/a printed only one time for last file
      }
    }
  } else if (argv[1][0] == '-') {  // other flags
    template = argv[2];
    for (int i = 3; i < argc; i++) {
      name = argv[i];
      if (argc == 4) {
        if (common(template, name, argv[1], 0, etemplate, e) == 0)
          printf("n/a\n");
      } else {
        if (common(template, name, argv[1], 1, etemplate, e) == 0 &&
            i == argc - 1)
          printf("n/a\n");
      }
    }
  } else {  // no flags
    template = argv[1];
    for (int i = 2; i < argc; i++) {
      name = argv[i];
      if (argc == 3) {
        common(template, name, "none", 0, etemplate, e);
      } else {
        common(template, name, "none", 1, etemplate, e);
      }
    }
  }
}

int common(char *template, char *name, char *flag, int mult, char **etemplate,
           int e) {
  int ret = 1;
  FILE *file;
  int matchCheck = 0;
  int c = 0;
  int line = 0;
  int sizetemp = strlen(template);
  char fname[20];
  strcpy(fname, "");
  if (mult == 1) {  // only print name if there are many files
    strcat(fname, name);
    strcat(fname, ":");
  }
  file = fopen(name, "r");
  if (file == NULL) {
    if (strcmp(flag, "-s")) {
      printf("n/a\n");
    }
  } else {
    int i = 0;
    char arr[N];
    char iarr[N];  // all lowercase version of arr for i flag
    int match = 0;
    if (!strcmp(flag, "-i")) {
      for (int tem = 0; tem < sizetemp;
           tem++) {  // makes template also lowercase
        template[tem] = tolower(template[tem]);
      }
    }
    while ((arr[i] = fgetc(file)) != EOF) {
      if (arr[i] == '\n') {
        arr[i] = '\0';
        if (!strcmp(flag, "-e")) {
          for (int n = 0; n < e; n++) {
            if (regex(etemplate[n], arr) > 0) match = 1;
          }
        } else if (!strcmp(flag, "-i")) {
          for (int temp = 0; temp < i; temp++) {
            iarr[temp] = arr[temp];
          }
          iarr[i] = '\0';
          if (regex(template, iarr) > 0) {
            match = 1;
          }
        } else if (regex(template, arr) > 0) {
          match = 1;
        }
        if (i == 0) match = 0;
        flagCheck(flag, arr, name, fname, template, &line, &c, &matchCheck,
                  match, 0);
        match = 0;
        i = 0;
      } else {
        i++;
      }
    }
    arr[i] = '\0';
    if (arr[0] == '\0' && strcmp(flag, "-c") && strcmp(flag, "-l")) {
      fclose(file);
      return ret;
    }
    if (!strcmp(flag, "-e")) {
      for (int n = 0; n < e; n++) {
        if (regex(etemplate[n], arr) > 0) match = 1;
      }
    } else if (!strcmp(flag, "-i")) {
      for (int temp = 0; temp < i; temp++) {
        iarr[temp] = arr[temp];
      }
      iarr[i] = '\0';
      if (regex(template, iarr) > 0) {
        match = 1;
      }
    } else if (regex(template, arr) > 0) {
      match = 1;
    }
    if (flagCheck(flag, arr, name, fname, template, &line, &c, &matchCheck,
                  match, 1) == 0) {
      ret = 0;
    }
    fclose(file);
  }
  return ret;  // 1 by default, 0 if unknown flag
}

int flagCheck(char *flag, char *arr, char *name, char *fname, char *template,
              int *line, int *c, int *matchCheck, int match, int fin) {
  int ret = 1;
  if (!strcmp(flag, "-v")) {
    if (match == 0) {
      printf("%s%s\n", fname, arr);
    }
  } else if (!strcmp(flag, "-c")) {
    if (match == 1) *c = *c + 1;
    if (fin == 1) {
      printf("%s%d\n", fname, *c);
    }
  } else if (!strcmp(flag, "-l")) {
    if (match == 1) *matchCheck = 1;
    if (fin == 1 && *matchCheck == 1) {
      printf("%s\n", name);
    }
  } else if (!strcmp(flag, "-n")) {
    *line = *line + 1;
    if (match == 1) {
      printf("%s%d:%s\n", fname, *line, arr);
    }
  } else if (!strcmp(flag, "none") || !strcmp(flag, "-i") ||
             !strcmp(flag, "-s") || !strcmp(flag, "-e")) {
    if (match == 1) printf("%s%s\n", fname, arr);
  } else if (!strcmp(flag, "-h")) {
    if (match == 1) printf("%s\n", arr);
  } else if (!strcmp(flag, "-o")) {
    if (match == 1) printf("%s%s\n", fname, template);
  } else if (!strcmp(flag, "-f")) {
    if (fflag(arr, template) == 1) {
      printf("%s%s\n", fname, arr);
    }
    if (fflag(arr, template) == 2) {
      ret = 0;
    }
  } else {
    ret = 0;
  }
  return ret;  // 1 by default, 0 if unknown flag or no file for f flag
}

int regex(char *pattern, char *arr) {
  pcre *re;
  const char *error;
  int erroffset;
  int count = 0;
  re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
  if (re != NULL) {
    int ovector[30];
    count = pcre_exec(re, NULL, arr, strlen(arr), 0, 0, ovector, 30);
    free(re);
  } else {
    printf("n/a\n");
  }
  return count;
}

int fflag(
    char *arr,
    char *template) {  // go through pattern file to find regexes in f flag
  FILE *pattern;
  int ret = 0;
  pattern = fopen(template, "r");
  if (pattern == NULL) {
    ret = 2;
  } else {
    char pat[N];
    int i = 0;
    while ((pat[i] = fgetc(pattern)) != EOF) {
      if (pat[i] == '\n') {
        pat[i] = '\0';
        if (regex(pat, arr) > 0) {
          ret = 1;
        }
        i = 0;
      } else {
        i++;
      }
    }
    pat[i] = '\0';
    if (regex(pat, arr) > 0) {
      ret = 1;
    }
    fclose(pattern);
  }
  return ret;  // 0 if pattern not found, 1 if pattern found, 2 if file not
               // found
}
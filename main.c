#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

char *read_file(char *path){
  FILE *fp = fopen(path, "r");

  if(fp == NULL) {
    fprintf(stderr, "[ERROR] error has occured (errno: %d)\n", errno);
    exit(-1);
  }

  fseek(fp, 0, SEEK_END);
  size_t flen = ftell(fp);

  rewind(fp);

  char *buf = (char *) malloc(sizeof(char)*flen);

  fread(buf, sizeof(char), flen, fp);
  fclose(fp);
  return buf;
}

// list implementation
typedef struct list_t {
  char *entry;
  struct list_t *next;
} MyList;


MyList *list_insert_prev(MyList *p, char *ptr) {
  MyList *n = (MyList *)malloc(sizeof(MyList));
  n->entry = ptr;
  n->next = NULL;
  
  if(p != NULL){
    p->next = n;
  }

  return n;
} 

MyList *list_destroy(MyList *h){
  MyList *c = h;
  MyList *p = NULL;
  while (c != NULL){
    p = c;
    c = c->next;
    free(p);
  }
}

MyList *csv_list(char *csv) {
  MyList *p = list_insert_prev(NULL, csv);
  MyList *h = p;
  h->next = NULL;
  
  bool flag = false;

  size_t csvlen = strlen(csv);
  for (size_t i=0; i<csvlen;i++) {
    if(csv[i] == ',' || csv[i] == '\n') {
      csv[i] = 0;
      flag = true;
      continue;
    }
    
    if(flag == true) {
      p = list_insert_prev(p, (char *) csv+i);
      flag = false;
    }
  }

  return h;
}

int main(int argc, char **argv) {
  if(argc < 2) { fprintf(stderr, "[ERROR] please provide file path\n");  exit(-1);}

  char *csv = read_file(argv[1]);

  MyList *h = csv_list(csv);
  
  list_destroy(h);
  
  free(csv);
  return 0;
}

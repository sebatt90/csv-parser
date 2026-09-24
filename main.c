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

// dynamic array implementation
typedef struct array_t {
  // data
  char **data;
  // metadata
  size_t size; // size of array
  size_t len; // used spaces
  char alpha;
} CSVArray;


#define CSVARRAY_SIZE 16

CSVArray *csvarray_init(){
  CSVArray *n = (CSVArray *) calloc(1,sizeof(CSVArray));
  n->data = (char **) calloc(CSVARRAY_SIZE, sizeof(char *));
  n->size = CSVARRAY_SIZE;
  n->len = 0;
  return n;
}

static void csvarray_insert(CSVArray *arr, char *data){
  if(arr->size == arr->len) {
    arr->size *= 2;    
    arr->data = (char **) realloc(arr->data, arr->size*sizeof(char *));
  }
  arr->data[arr->len] = data;
  arr->len++;
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

CSVArray *csv_list(char *csv) {
  CSVArray *arr = csvarray_init();
  csvarray_insert(arr, csv);
  
  bool flag = false;

  size_t csvlen = strlen(csv);
  for (size_t i=0; i<csvlen;i++) {
    if(csv[i] == ',' || csv[i] == '\n') {
      csv[i] = 0;
      flag = true;
      continue;
    }
    
    if(flag == true) {
      csvarray_insert(arr, csv+i);
      flag = false;
    }
  }

  return arr;
}

int main(int argc, char **argv) {
  if(argc < 2) { fprintf(stderr, "[ERROR] please provide file path\n");  exit(-1);}

  char *csv = read_file(argv[1]);

  CSVArray *csvarr = csv_list(csv);
  
  free(csv);
  free(csvarr->data);
  free(csvarr);
  return 0;
}

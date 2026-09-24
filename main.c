#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define THREAD_SLICE 10485760

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


#define CSVARRAY_SIZE 8192

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

typedef struct csvslice_t {
  size_t start;
  size_t end;
  CSVArray *arr;
} CSVSlice;

static char *csv;

void *process_csv(void *args) {
  CSVSlice *slice = (CSVSlice *) args;

  slice->arr = csvarray_init();

  bool flag = (slice->start == 0) ? true : false;

  for (size_t i=slice->start; i<slice->end;i++) {
    if(csv[i] == ',' || csv[i] == '\n' || csv[i] == '\r') {
      csv[i] = 0;
      flag = true;
      continue;
    }
    
    if(flag == true) {
      csvarray_insert(slice->arr, csv+i);
      flag = false;
    }
  }
  
}

int main(int argc, char **argv) {
  if(argc < 2) { fprintf(stderr, "[ERROR] please provide file path\n");  exit(-1);}

  csv = read_file(argv[1]);
  size_t csvlen = strlen(csv);

  size_t N_THREADS = (csvlen + THREAD_SLICE - 1) / THREAD_SLICE;

  printf("len: %d; slice: %d; threads: %d\n",csvlen,THREAD_SLICE,N_THREADS);
  
  pthread_t threads[N_THREADS];
  CSVSlice *slices[N_THREADS];
  
  for(size_t i=0; i<N_THREADS; i++){
    slices[i] = (CSVSlice *) malloc(sizeof(CSVSlice));
    slices[i]->start = i*THREAD_SLICE;
    slices[i]->end = (i == N_THREADS - 1) ? strlen(csv) - 1 : (i+1)*THREAD_SLICE - 1;
    pthread_create(&threads[i], NULL, process_csv, slices[i]);
  }

  
  for (int i = 0; i < N_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  // merge the array

  
  return 0;
}

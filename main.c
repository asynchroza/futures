/**
 * This is an extremely convoluted example of a future?
 * Goal is to get a better grasp of C and how futures
 * work.
 *
 * Each future task is spinning up a new phthread.
 *
 * TODO: Understand cost of phthread creation
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum Status { PENDING, FULFILLED, REJECTED };

struct Future {
  enum Status status;
  void *value; // Void pointer can point to any type of data but cannot be
               // dereferenced

  pthread_t thread;
  char *errorMsg; // TODO: Why should this be at the end of the struct?
};

// LOL: Don't pass the future as an argument to the function??
struct WriteToFileArgs {
  int timeout;
  struct Future *future;
  char file_name[64];
  char *message;
};

struct Future *startTask(void *task, void *args) {
  struct Future *future = malloc(sizeof(struct Future));

  future->status = PENDING;
  future->value = NULL;

  ((struct WriteToFileArgs *)args)->future = future;

  pthread_t thread;
  pthread_create(&thread, NULL, task, args);

  printf("Thread created and future initialized\n");

  future->thread = thread;

  return future;
}

void writeToFile(struct WriteToFileArgs *args) {
  sleep(args->timeout);

  FILE *file = fopen(args->file_name, "w");

  if (file == NULL) {
    args->future->status = REJECTED;
    args->future->errorMsg = "Error opening file";
    return;
  }

  int result = fprintf(file, "%s", args->message);

  if (result < 0) {
    args->future->status = REJECTED;
    args->future->errorMsg = "Error writing to file";
    return;
  }

  result = fclose(file);

  if (result != 0) {
    args->future->status = REJECTED;
    args->future->errorMsg = "Error closing file";
    return;
  }

  args->future->status = FULFILLED;
}

void pollFuture(struct Future *future, int index) {
  if (future->status == PENDING) {
    printf("Task is still running - index %d\n", index);
  }

  if (future->status == FULFILLED) {
    printf("Task completed successfully - index %d\n", index);
  }

  if (future->status == REJECTED) {
    printf("Task failed with error - index %d: %s\n", index, future->errorMsg);
  }
}

int main() {
  struct WriteToFileArgs args1 = {5, NULL, "file.txt", "Hello, World!\n"};
  struct WriteToFileArgs args2 = {10, NULL, "hello.txt", "Hello, World!\n"};

  struct Future *primo = startTask(writeToFile, &args1);
  struct Future *secondo = startTask(writeToFile, &args2);

  int size = 2;
  struct Future *futures[size];
  futures[0] = primo;
  futures[1] = secondo;

  while (1) {
    for (int i = 0; i < size; i++) {
      sleep(1);
      pollFuture(futures[i], i);
    }
  }

  // TODO: Memory should be freed
  return 0;
}

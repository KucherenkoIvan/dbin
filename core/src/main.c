#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h> // read(), write(), close()

#include "./io/io.h"
#include "./utils/utils.h"

void setup_files() {
  setup_lockfile();
  setup_indexfile();
  setup_datafile();
}

// CRUD Operations
void Create(const Blob *record) {
  int head_pos = write_new_record(record);
  write_index(head_pos, -1);
}

Blob *Read(int id) {
  int record_pos = get_record_pos_by_id(id);

  Blob *result = read_record(record_pos);

  return result;
}

// functions below require solution for fragmentation problem to work
// efficiently another index for free space and more complex allocation logic
// will be implemented in v0.2
void Update() { /* think of it later */ }
void Delete() { /* just set the flags */ }
void HardDelete() { /* write zeros over record OR clear index to make it
                       unsearchable OR both */
}

int main() {
  setup_files();

  // unsigned char test_payload[] =
  //     "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris nec "
  //     "tincidunt neque. Duis nulla lectus, tristique ac mattis quis, "
  //     "efficitur "
  //     "ac nunc. Integer mollis, dolor at dictum vulputate, arcu ipsum "
  //     "commodo "
  //     "neque, nec imperdiet diam dui feugiat libero. Donec eget tempus ante,
  //     " "id posuere turpis. Fusce molestie nisi tincidunt augue.";
  // int payload_size = sizeof(test_payload);
  //
  // for (int i = 0; i < 100; i++)
  //   Create(test_payload, payload_size);

  unsigned char words[][30] = {"one", "two", "three", "four", "five",
                               "f",   "u",   "gay",   "meow", "test"};
  int l = 10;

  printf("start\n");

  // for (int k = 0; k < 10000; k++) {
  //   for (int i = 0; i < l; i++) {
  //     int size = 0;
  //     unsigned char *t = Read(2 + i * k, &size);
  //
  //     printf("%s\n", t);
  //   }
  // }

  printf("end\n");

  for (int i = 0; i < 9; i++) {
    Blob *b = malloc(sizeof(Blob));

    b->size = 30;
    b->data = malloc(30);

    memcpy(b->data, words[i], 30);

    printf("Word: %s\n", words[i]);

    Create(b);

    free(b->data);
    free(b);
  }
  //
  // int size = -1;
  // unsigned char *data = Read(8, &size);
  // // Read(919);
  //
  // unsigned char str[size];
  // memcpy(str, data, size);
  //
  // printf("%s\n", str);

  // FILE *test;
  // test = fopen("test.bin", "r");
  // fseek(test, -16, SEEK_END);
  // char line[100];
  //
  // fgets(line, sizeof(line), test);
  // printf("%s", line);
}

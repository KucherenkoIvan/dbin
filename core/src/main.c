#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h> // read(), write(), close()

#include "./io/io.h"
#include "./utils/utils.h"

void save_files() {
  save_lock();
  save_index();
  save_data();
}

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

bool cmp_str(unsigned char *str1, unsigned char *str2, int len) {
  for (int i = 0; i < len; i++) {
    if (str1[i] != str2[i]) {
      return false;
    }
  }
  return true;
}

int main() {
  setup_files();

  unsigned char *line = NULL;
  int size;

  unsigned char *quit_cmd = "Q";

  while (true) {
    if (getline(&line, &size, stdin) != -1) {
      Blob *b = malloc(sizeof(Blob));

      b->size = size;
      b->data = malloc(size);

      memcpy(b->data, line, size);

      Create(b);

      free(b->data);
      free(b);

      save_files();
    }
  }

  return 0;
}

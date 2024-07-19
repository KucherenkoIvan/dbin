#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // read(), write(), close()

void debug_print_bytes(unsigned char *bytes, int size) {
  for (int i = 0; i < size; i++) {
    printf("Byte[%d]: %x\n", i, bytes[i]);
  }
}
bool file_exists(const char *filename) { return access(filename, F_OK) == 0; }

void create_file_if_not_exists(const char *filename) {
  if (file_exists(filename))
    return;

  FILE *t = fopen(filename, "w");
  fclose(t);
}

int get_nth_byte(int val, int n) {
  return (val >> (8 * (sizeof(int) - n - 1))) & 0x00ff;
}

int build_int(unsigned char *buffer) {
  if (buffer == NULL) {
    return 0;
  }

  int result = 0;

  for (int i = 0; i < 4; i++) {
    result += (buffer[i] << (8 * (3 - i)));
  }

  return result;
}

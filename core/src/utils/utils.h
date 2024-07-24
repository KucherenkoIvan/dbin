#include <stdbool.h>

typedef struct Blob {
  int size;
  unsigned char *data;
} Blob;

void debug_print_bytes(unsigned char *bytes, int size);
bool file_exists(const char *filename);
void create_file_if_not_exists(const char *filename);
int get_nth_byte(int val, int n);
int build_int(unsigned char *buffer);

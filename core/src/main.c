#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read(), write(), close()

#define MAX_BUF_SIZE 80

const unsigned char VERSION = 1;
const int FRAME_BUFFER_SIZE = 0xffff;
const unsigned char EXAMPLE_HEADER[] = {
    VERSION, // version
    0x00,    // 1st byte of flags
    0x00,    // 2nd byte of flags
    0x00,    // 1st byte of data_size
    0x00,    // 2nd byte of data_size
};
const int HEADER_SIZE = sizeof(EXAMPLE_HEADER);

const int LOCK_SIZE = 8;
unsigned long ID_LOCK = 0;
FILE *LOCKFILE = NULL;
FILE *DB_DATA = NULL;
FILE *INDEX = NULL;

void debug_print_bytes(unsigned char *bytes, int size) {
  for (int i = 0; i < size; i++) {
    printf("Byte[%d]: %x\n", i, bytes[i]);
  }
}

int encode_frame(const int flags, const unsigned char *payload,
                 int payload_size, unsigned char *write_buffer) {
  memset(write_buffer, 0, FRAME_BUFFER_SIZE);

  const int mask = 0x00ff;

  unsigned char b_flags[] = {(flags >> 8) & mask, flags & mask};

  unsigned char data_size[] = {(payload_size >> 8) & mask, payload_size & mask};

  unsigned char header[] = {VERSION, b_flags[0], b_flags[1], data_size[0],
                            data_size[1]};

  int header_size = sizeof(header);
  int frame_size = header_size + payload_size;

  memcpy(write_buffer, header, header_size);
  memcpy(&write_buffer[header_size], payload, payload_size);

  return frame_size;
}

int decode_frame(unsigned char *frame, int frame_length,
                 unsigned char *write_buffer) {
  memset(write_buffer, 0, FRAME_BUFFER_SIZE);

  int version = frame[0];
  if (version != VERSION) {
    return -1;
  }

  memcpy(write_buffer, &frame[HEADER_SIZE], frame_length - HEADER_SIZE);

  return frame_length - HEADER_SIZE;
}

bool file_exists(const char *filename) { return access(filename, F_OK) == 0; }

void create_file_if_not_exists(const char *filename) {
  if (file_exists(filename))
    return;

  FILE *t = fopen(filename, "w");
  fclose(t);
}

void setup_lockfile() {
  LOCKFILE = fopen("lockfile", "ab+");

  bool lockfile_exists = file_exists("lockfile");
  if (!lockfile_exists) {
    fwrite("", 0, 1, LOCKFILE);
  }
  fseek(LOCKFILE, 0, SEEK_END);
  int size = ftell(LOCKFILE);
  fclose(LOCKFILE);

  if (size != 0) {
    LOCKFILE = fopen("lockfile", "rb");
    unsigned char lock_buf[LOCK_SIZE];
    fread(lock_buf, LOCK_SIZE, 1, LOCKFILE);

    ID_LOCK = 0;
    for (int i = 0; i < LOCK_SIZE; i++) {
      ID_LOCK += lock_buf[i] << (8 * (LOCK_SIZE - i - 1));
    }
    fclose(LOCKFILE);
  }
}

void dump_id_lock() {
  LOCKFILE = fopen("lockfile", "wb");
  unsigned char buf[LOCK_SIZE];
  int mask = 0x00ff;

  for (int i = 0; i < LOCK_SIZE; i++) {
    buf[i] = (ID_LOCK >> (8 * (LOCK_SIZE - i - 1))) & mask;
  }

  fwrite(buf, LOCK_SIZE, 1, LOCKFILE);
  fclose(LOCKFILE);
}

int get_incremental_id() {
  ID_LOCK++;
  dump_id_lock();
  return ID_LOCK;
}

int write_new_record(unsigned char *payload, int payload_length) {
  unsigned char write_buffer[FRAME_BUFFER_SIZE];
  int flags = 0x00;

  int frame_size = encode_frame(flags, payload, payload_length, write_buffer);

  create_file_if_not_exists("data.dbin");

  FILE *write_ptr;
  write_ptr = fopen("data.dbin", "ab");

  int head_pos = ftell(write_ptr);

  fwrite(write_buffer, frame_size, 1, write_ptr);

  fclose(write_ptr);

  return head_pos;
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

void write_index(int head_pos, int id) {
  if (id <= 0)
    id = get_incremental_id();

  int size = sizeof(int);
  unsigned char write_buffer[size * 2];

  int i = 0;

  for (; i < size; i++) {
    write_buffer[i] = get_nth_byte(head_pos, i);
  }

  for (; i < size * 2; i++) {
    write_buffer[i] = get_nth_byte(id, i);
  }

  create_file_if_not_exists("index.idx");

  FILE *write_ptr;
  write_ptr = fopen("index.idx", "ab");

  fwrite(write_buffer, size * 2, 1, write_ptr);

  fclose(write_ptr);
}

void Create(unsigned char *payload, int size) {
  int head_pos = write_new_record(payload, size);
  write_index(head_pos, -1);
}

unsigned char *Read(int id, int *size) {
  // allocate buffer
  unsigned char *buffer = malloc(8);

  create_file_if_not_exists("index.idx");

  int parsed_id = 0;

  FILE *read_ptr;
  read_ptr = fopen("index.idx", "rb");

  // read index by 8 byte chuncks (offset-id) until found id or EOF
  do {
    fread(buffer, 8, 1, read_ptr);

    parsed_id = build_int(buffer + 4);
  } while (parsed_id != id && feof(read_ptr) == 0);

  fclose(read_ptr);

  int offset = build_int(buffer);

  free(buffer);
  buffer = malloc(HEADER_SIZE);

  // read data file or return null
  if (parsed_id != id) {
    return NULL;
  }

  read_ptr = fopen("data.dbin", "rb");
  // fseek to offset
  fseek(read_ptr, offset, SEEK_SET);
  // read HEADER_SIZE bytes
  // TODO: header validation
  // parse and validate header
  fread(buffer, HEADER_SIZE, 1, read_ptr);

  // read PAYLOAD_LENGTH bytes to buffer from 1st step
  unsigned char temp_buffer[4] = {0};
  memcpy(temp_buffer + 2, buffer + (HEADER_SIZE - 2), 2);
  *size = build_int(temp_buffer);

  free(buffer);
  buffer = malloc(*size);
  fread(buffer, *size, 1, read_ptr);

  fclose(read_ptr);

  return buffer;
  // return buffer (probably should use structure { p*, p_size })
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
  setup_lockfile();

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

  unsigned char words[][30] = {"one",  "two", "three", "four",
                               "five", "f",   "u",     "meow"};

  for (int i = 0; i < 9; i++) {
    printf("Word: %s\n", words[i]);

    Create(words[i], 30);
  }

  int size = -1;
  unsigned char *data = Read(8, &size);
  // Read(919);

  unsigned char str[size];
  memcpy(str, data, size);

  printf("%s\n", str);

  // FILE *test;
  // test = fopen("test.bin", "r");
  // fseek(test, -16, SEEK_END);
  // char line[100];
  //
  // fgets(line, sizeof(line), test);
  // printf("%s", line);
}

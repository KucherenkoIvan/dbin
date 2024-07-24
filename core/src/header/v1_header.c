#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../utils/utils.h"
#include "./header.h"

const unsigned char VERSION = 1;
const unsigned char EXAMPLE_V1_HEADER[] = {
    1,    // version
    0x00, // 1st byte of flags
    0x00, // 2nd byte of flags
    0x00, // 1st byte of data_size
    0x00, // 2nd byte of data_size
};
const int V1_HEADER_SIZE = sizeof(EXAMPLE_V1_HEADER);

FILE *HEADER_WRITE_PTR = NULL;
FILE *HEADER_READ_PTR = NULL;

const int mask = 0x00ff;

unsigned char *encode_v1_header(const V1_Header *header_data) {
  if (header_data == NULL) {
    return NULL;
  }

  unsigned char *header = malloc(V1_HEADER_SIZE);

  unsigned char flags[] = {(header_data->flags >> 8) & mask,
                           header_data->flags & mask};

  unsigned char data_size[] = {(header_data->data_size >> 8) & mask,
                               header_data->data_size & mask};

  unsigned char header_buff[] = {VERSION, flags[0], flags[1], data_size[0],
                                 data_size[1]};

  memcpy(header, header_buff, V1_HEADER_SIZE);

  return header;
}

V1_Header *decode_v1_header(const unsigned char *raw_header) {
  if (raw_header == NULL) {
    return NULL;
  }

  if (raw_header[0] != 1) {
    return NULL;
  }

  V1_Header *header_data = malloc(sizeof(V1_Header));

  unsigned char flags_buff[] = {0x0, 0x0, raw_header[1], raw_header[2]};
  unsigned char data_suze_buff[] = {0x0, 0x0, raw_header[3], raw_header[4]};

  header_data->flags = build_int(flags_buff);
  header_data->data_size = build_int(data_suze_buff);

  return header_data;
}

const unsigned char VERSION = 1;
const unsigned char EXAMPLE_V1_HEADER[] = {
    VERSION, // version
    0x00,    // 1st byte of flags
    0x00,    // 2nd byte of flags
    0x00,    // 1st byte of data_size
    0x00,    // 2nd byte of data_size
};
const int V1_HEADER_SIZE = sizeof(EXAMPLE_V1_HEADER);

typedef struct V1_Header {
  unsigned int flags;
  unsigned int data_size;
} V1_Header;

unsigned char *encode_v1_header(const V1_Header *header_data);

V1_Header *decode_v1_header(const unsigned char *raw_header);

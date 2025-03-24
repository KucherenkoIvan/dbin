extern const unsigned char VERSION;
extern const unsigned char EXAMPLE_V1_HEADER[5];
extern const int V1_HEADER_SIZE;

typedef struct V1_Header {
  unsigned int flags;
  unsigned int data_size;
} V1_Header;

unsigned char *encode_v1_header(const V1_Header *header_data);

V1_Header *decode_v1_header(const unsigned char *raw_header);

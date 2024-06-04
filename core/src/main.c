#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()

#define MAX_BUF_SIZE 80
#define PORT 8080
#define SA struct sockaddr

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

// Function designed for chat between client and server.
void echo(int connfd) {
  char buff[MAX_BUF_SIZE];
  int n;
  for (;;) {
    bzero(buff, MAX_BUF_SIZE);

    // read the message from client and copy it in buffer
    read(connfd, buff, sizeof(buff));
    // print buffer which contains the client contents
    printf("From client: %s\t To client : %s\n", buff, buff);

    // and send that buffer back to client
    write(connfd, buff, sizeof(buff));
  }
}

int create_tcp_socket() {
  int sockfd;
  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Socket creation failed...\n");
    return -1;
  }

  return sockfd;
}

int create_tcp_server(int _PORT, int sockfd) {
  int connfd, len;
  struct sockaddr_in servaddr, cli;

  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(_PORT);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
    printf("Socket bind failed...\n");
    return -1;
  }

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    return -1;
  }
  len = sizeof(cli);

  // Accept the data packet from client and verification
  connfd = accept(sockfd, (SA *)&cli, &len);
  if (connfd < 0) {
    printf("Server accept failed...\n");
    return -1;
  }

  return connfd;
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

  memcpy(write_buffer, &frame[5], frame_length - 5);

  return frame_length - 5;
}

bool file_exists(const char *filename) { return access(filename, F_OK) == 0; }

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

// Driver function
int main() {
  // int sockfd = create_tcp_socket();
  // int connfd = create_tcp_server(PORT, sockfd);
  // Function for chatting between client and server
  // echo(connfd);

  // After chatting close the socket
  // close(sockfd);

  setup_lockfile();

  for (int i = 0; i <= 0xff; i++)
    get_incremental_id();

  unsigned char write_buffer[FRAME_BUFFER_SIZE];
  unsigned char read_buffer[FRAME_BUFFER_SIZE];

  unsigned char test_payload[] =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris nec "
      "tincidunt neque. Duis nulla lectus, tristique ac mattis quis, "
      "efficitur "
      "ac nunc. Integer mollis, dolor at dictum vulputate, arcu ipsum "
      "commodo "
      "neque, nec imperdiet diam dui feugiat libero. Donec eget tempus ante, "
      "id posuere turpis. Fusce molestie nisi tincidunt augue.";

  int flags = 0x00;

  FILE *write_ptr;
  write_ptr = fopen("test.bin", "wb"); // w for write, b for binary

  int frame_size =
      encode_frame(flags, test_payload, sizeof(test_payload), write_buffer);

  fwrite(write_buffer, frame_size, 1,
         write_ptr); // write all bytes from our buffer
  decode_frame(write_buffer, frame_size, read_buffer);
  // printf("%s\n", read_buffer);
  fclose(write_ptr);

  FILE *test;
  test = fopen("test.bin", "r");
  fseek(test, -16, SEEK_END);
  char line[100];

  fgets(line, sizeof(line), test);
  printf("%s", line);

  // fclose(test);

  // unsigned char tb[] = {0, 0, 0, 0, 0, 0, 0};
  // fseek(write_ptr, 0, SEEK_SET);
  // fwrite(tb, sizeof(tb), 1, write_ptr);
}

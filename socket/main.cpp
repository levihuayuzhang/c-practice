#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

auto main(int argc, const char *argv[]) -> int {
  std::cout << "this is socket\n"; 
  struct sockaddr_in aaa{};
}

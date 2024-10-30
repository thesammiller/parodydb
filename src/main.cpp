#include "node.h"
#include <unistd.h>

int main() {
  printf("Hello World!\n");
  FileHeader myFileHeader;
  char* filename = new char[32];
  strcpy(filename, "./myfile.txt");
  NodeFile myNodeFile(filename);
  bool newfile = access(filename, 0) != 0;
  if (newfile) {
    printf("File not found");
  }
  else {
    printf("File found");
  }
  remove(filename);
}
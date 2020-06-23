#include "async.h"

#include <iostream>
#include <string>
#include <memory>

int main(int argc, const char** argv) 
{  
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
    return 1;
  }

  long lBlockSize{0};
  try {
    lBlockSize = std::stoi(argv[1]);
  }
  catch (const std::invalid_argument& ex) {
    std::cerr << "Error: Block size is incorrect!" << std::endl;
    return 1;
  }
  catch (const std::out_of_range& ex) {
    std::cerr << "Error: Block size is out of range!" << std::endl;
    return 1;
  }

  if (lBlockSize == 1) {
    std::cerr << "Error: Block size must be greater then zero!" << std::endl;
    return 1;
  }

  if (lBlockSize < 0) {
    std::cerr << "Error: Block size must not be negative!" << std::endl;
    return 1;
  }


  
  async::handle_t h1 = async::connect(lBlockSize);
  async::handle_t h2 = async::connect(5);

  std::string strLine;
  while ( std::getline(std::cin, strLine) ) {
    async::receive(h1, strLine.c_str(), strLine.size());
    async::receive(h2, strLine.c_str(), strLine.size());
  }
  async::disconnect(h1);
  async::disconnect(h2);

  return 0;
}

#include "reader.h"
#include "executer.h"
#include "logger.h"

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

  auto pReader = std::make_shared<Otus::Reader>("main", static_cast<std::size_t>(lBlockSize));
  auto pLogger = Otus::Logger::Create("file", pReader);
  auto pExecuter = Otus::Executer::Create("log", pReader);  
  
  pReader->Exec();

  return 0;
}

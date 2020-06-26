#pragma once

#include "observer.h"
#include "command.h"
#include <counter.h>

#include <iostream>
#include <string>

namespace Otus {

class Commander : public BaseObservable<CommandBlock>
{
public:
  static std::shared_ptr<Commander> Create(const std::string& a_strName, std::size_t a_szBlockSize, std::ostream& a_osMetricsOut = std::cout);
  ~Commander();

  void Exec();
  void ProccessLine(const std::string& a_strLine);
  void SetContext(void* a_context);

private:
  Commander(const std::string& a_strName, std::size_t a_szBlockSize, std::ostream& a_osMetricsOut = std::cout);

  void Flush();

private:
  void* m_context;
  std::ostream& m_osMetricsOut;
  std::size_t m_szBlockSize;  
  std::size_t m_szBlockDepth;
  Counters m_counters;
  CommandBlock m_CommandBlock;  
};

} // Otus::

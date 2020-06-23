#include "commander.h"
#include "counter.h"

#include <string>

namespace Otus {

Commander::Commander(const std::string& a_strName, std::size_t a_szBlockSize, std::ostream& a_osMetricsOut) 
  : m_osMetricsOut{a_osMetricsOut}
  , m_szBlockSize{a_szBlockSize}
  , m_szBlockDepth{0}
  , m_counters{a_strName}
  , m_CommandBlock{a_szBlockSize}
{ }

Commander::~Commander()
{
  m_osMetricsOut << m_context << ' ' << m_counters << std::endl; 
}

void Commander::ProccessLine(const std::string& a_strLine)
{
  if (a_strLine.empty()) {
    return;
  }

  ++m_counters.lineCounter;
  if (a_strLine == "{") {
    if (m_szBlockDepth == 0) {
      Flush();
    }
    m_szBlockDepth++;
  } 
  else if (a_strLine == "}") {
    if (m_szBlockDepth == 0) {
      throw std::logic_error("unexpected symbol '}'");
    }
    m_szBlockDepth--;
  }
  else {
    m_CommandBlock << a_strLine;
  }

  if (!m_szBlockDepth && m_CommandBlock.Size() >= m_szBlockSize) {
    Flush();
  }
}

void Commander::Flush() 
{
  if (m_CommandBlock.Size() > 0) {
    Notify(m_CommandBlock);    
    m_counters.commandCounter += m_CommandBlock.Size();
    ++m_counters.blockCounter;
    m_CommandBlock.Clear();
  }
}

void Commander::SetContext(void* a_context)
{
  m_context = a_context;
}

} // Otus::

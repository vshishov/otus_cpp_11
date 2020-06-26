#include "context.h"

#include <string>

namespace Otus {

Context::Context(
  std::size_t a_szBlockSize, 
  std::ostream& a_osLoggerOut, 
  std::ostream& a_osMainMetricsOut, 
  std::ostream& a_osLogMetricsOut, 
  std::ostream& a_oFileMetricsOut
)
  : m_bDone{false}
  , m_thread{&Context::Procces, this}
{ 
  m_pCommander = Otus::Commander::Create("main", static_cast<std::size_t>(a_szBlockSize), a_osMainMetricsOut);
  m_pLogger = Otus::Logger::Create("file", m_pCommander, a_oFileMetricsOut);
  m_pExecuter = Otus::Executer::Create("log", m_pCommander, a_osLoggerOut, a_osLogMetricsOut); 

  m_pCommander->SetContext(this); 
  m_pLogger->SetContext(this); 
  m_pExecuter->SetContext(this); 
}

Context::~Context()
{
  JoinThred();
}

void Context::ProccessBuffer(const char* a_Buffer, std::size_t a_szSize)
{
  {
    std::unique_lock<std::mutex> lock(m_streamLock);
    m_ssInputStream.write(a_Buffer, a_szSize);
  }
  m_bNotified = true;
  m_streamCheck.notify_one();
}

void Context::Procces()
{
  while (!m_bDone) {
    std::unique_lock<std::mutex> locker(m_streamLock);
    m_streamCheck.wait(locker, [&](){return m_bNotified || m_bDone;});
    std::string strLine;
    while ( std::getline(m_ssInputStream, strLine) ) {
      m_pCommander->ProccessLine(strLine);
    }
    m_ssInputStream.clear();
    m_ssInputStream.str("");
    m_bNotified = false;
  }
}

void Context::JoinThred()
{
  m_bDone = true;
  m_streamCheck.notify_all();
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

} // Otus::

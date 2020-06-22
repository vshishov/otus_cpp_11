#include "context.h"

namespace Otus {

Context::Context(const std::shared_ptr<Commander>& a_pCommander)
  : m_pCommander{a_pCommander}
  , m_bDone{false}
  , m_thread{&Context::Procces, this}
{ }

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
  std::cout << m_ssInputStream.str() << std::endl;
  m_streamCheck.notify_one(); 
}

void Context::Procces()
{
  while (!m_bDone) {
    std::unique_lock<std::mutex> locker(m_streamLock);
    // m_streamCheck.wait(locker, [&](){return !m_ssInputStream.eof() || m_bDone;});
    // m_streamCheck.wait(locker, [&](){return m_bNotified || m_bDone;});
    m_streamCheck.wait(locker);
    
    std::string strLine;
    while ( !std::getline(m_ssInputStream, strLine).eof() ) {
      std::cout << "Commander - " << strLine << std::endl;
      m_pCommander->ProccessLine(strLine);
    }
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

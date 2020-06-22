#include "executer.h"

namespace Otus {

Excuter::Excuter(const std::string& a_strName, std::ostream& a_osOut, std::ostream& a_osMetricsOut)
  : m_osOut{a_osOut}
  , m_osMetricsOut{a_osMetricsOut}
  , m_bDone{false}
  , m_thread{&Excuter::Procces, this, a_strName}
{ }

Excuter::~Excuter()
{
  JoinThred();
}

std::shared_ptr<Excuter> Excuter::Create(const std::string& a_strName, std::shared_ptr<Reader>& a_pReader, std::ostream& a_osOut, std::ostream& a_osMetricsOut)
{
  auto ptr = std::shared_ptr<Excuter>(new Excuter(a_strName, a_osOut, a_osMetricsOut));
  ptr->SetReader(a_pReader);
  return ptr;
}

void Excuter::Update(const CommandBlock& a_CommandBlock) 
{
  {
    std::unique_lock<std::mutex> lock(m_queueLock);
    m_queueCommand.push(a_CommandBlock);
  }
  m_queueCheck.notify_all(); 
}

void Excuter::Procces(std::string a_strName)
{
  Counters counters{a_strName};
  CommandBlock commandBlock;
  while (!m_bDone) {
    {
      std::unique_lock<std::mutex> locker(m_queueLock);
      m_queueCheck.wait(locker, [&](){return !m_queueCommand.empty() || m_bDone;});
      if (!m_queueCommand.empty()) {
        commandBlock = m_queueCommand.front();
        m_queueCommand.pop();
      }
    }
      
    if (commandBlock.Size()){
      {
        std::unique_lock<std::mutex> locker(m_printLock);
        m_osOut << "bulk: " << commandBlock << std::endl;
      }

      ++counters.blockCounter;
      counters.commandCounter += commandBlock.Size();
      commandBlock.Clear();
    }
  }
  
  m_osMetricsOut << counters << std::endl;
}

void Excuter::JoinThred()
{
  m_bDone = true;
  m_queueCheck.notify_all();
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void Excuter::SetReader(std::shared_ptr<Reader>& a_pReader)
{
  m_pReader = a_pReader;
  auto ptrReader = m_pReader.lock();
  if (ptrReader) {
    ptrReader->Subscribe(shared_from_this());
  }
}

} // Otus::

#include "logger.h"
#include "counter.h"

#include <sstream>

namespace Otus {

const std::string SEPARATOR = "_";

Logger::Logger(const std::string& a_strName, std::ostream& a_osMetricsOut)
  : m_osMetricsOut{a_osMetricsOut}
  , m_bDone{false}
{
  
  for (size_t i = 0; i < 2; ++i) {
    std::string strThreadName = a_strName + std::to_string(i + 1);
    m_threads.emplace_back( std::thread(&Logger::Process, this, strThreadName) );
  }
}

Logger::~Logger()
{
  JoinTread();
}

std::shared_ptr<Logger> Logger::Create(const std::string& a_strName, std::shared_ptr<Commander>& a_pCommander, std::ostream& a_osMetricsOut)
{
  auto ptr = std::shared_ptr<Logger>{new Logger{a_strName, a_osMetricsOut}};
  ptr->SetCommander(a_pCommander);
  return ptr;
}

void Logger::Update(const CommandBlock& a_CommandBlock)
{
  {
    std::unique_lock<std::mutex> lock(m_queueLock);
    m_queueCommand.push(a_CommandBlock);
  }
  m_queueCheck.notify_all();
}

void Logger::Process(std::string a_strName)
{
  CommandBlock commandBlock;
  Counters counters{a_strName};
  while (!m_bDone) {
    {
      std::unique_lock<std::mutex> locker(m_queueLock);
      m_queueCheck.wait(locker, [&](){return !m_queueCommand.empty() || m_bDone;});
      if (!m_queueCommand.empty()) {
        commandBlock = m_queueCommand.front();
        m_queueCommand.pop();
      }
    }

    if (commandBlock.Size()) {
      std::stringstream ssFileName;
      ssFileName << "bulk" << SEPARATOR << a_strName << SEPARATOR << m_context << SEPARATOR << commandBlock.GetTimeStamp() << ".log";
      
      std::ofstream log(ssFileName.str(), std::ios::out);

      log << "bulk: " << commandBlock << std::endl;
      ++counters.blockCounter;
      counters.commandCounter += commandBlock.Size();
      commandBlock.Clear();
      log.close();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
  
  {
    std::unique_lock<std::mutex> locker(m_printLock);
    m_osMetricsOut << m_context << ' ' << counters << std::endl; 
  }
}

void Logger::JoinTread()
{
  m_bDone = true;
  m_queueCheck.notify_all();
  for ( auto& thread : m_threads ) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void Logger::SetCommander(std::shared_ptr<Commander>& a_pCommander)
{
  m_pCommander = a_pCommander;
  auto ptrCommander = m_pCommander.lock();
  if (ptrCommander) {
    ptrCommander->Subscribe(shared_from_this());
  }
}

void Logger::SetContext(void* a_context)
{
  m_context = a_context;
}

} // Otus::

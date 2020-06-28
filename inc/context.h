#pragma once

#include "commander.h"
#include "executer.h"
#include "logger.h"

#include <sstream>
#include <memory>
#include <string>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace Otus {

class Context
{
public:
  Context(
    std::size_t a_szBlockSize, 
    std::ostream& a_osLoggerOut = std::cout, 
    std::ostream& a_osMainMetricsOut = std::cout, 
    std::ostream& a_osLogMetricsOut = std::cout, 
    std::ostream& a_oFileMetricsOut = std::cout
  );
  ~Context();

  void ProccessBuffer(const char* a_Buffer, std::size_t a_szSize);

private:
  void Procces();
  void JoinThred();
  
private:
  std::shared_ptr<Commander> m_pCommander;
  std::shared_ptr<Executer> m_pExecuter;
  std::shared_ptr<Logger> m_pLogger;

  std::stringstream m_ssInputStream;

  std::atomic<bool> m_bDone;
  std::atomic<bool> m_bNotified;
  std::thread m_thread;
  std::mutex m_streamLock;
  std::condition_variable m_streamCheck;
};

} // Otus::

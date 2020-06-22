#pragma once

#include "command.h"
#include "observer.h"
#include "reader.h"

#include <memory>
#include <fstream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace Otus {

class Logger : public IObserver<CommandBlock>, public std::enable_shared_from_this<Logger>
{
public:
  static std::shared_ptr<Logger> Create(const std::string& a_strName, std::shared_ptr<Reader>& a_pReader, std::ostream& a_osMetricsOut = std::cout);
  ~Logger();

  void Update(const CommandBlock& a_Commands) override;
  void SetReader(std::shared_ptr<Reader>& a_pReader);

private:
  Logger(const std::string& a_strName, std::ostream& a_osMetricsOut = std::cout);

  void Process(std::string a_strName);
  void JoinTread();

private:
  std::weak_ptr<Reader> m_pReader;
  std::ostream& m_osMetricsOut;

  std::vector<std::thread> m_threads;
  std::atomic<bool> m_bDone;

  std::mutex m_queueLock;
  std::mutex m_printLock;
  std::condition_variable m_queueCheck;
  QueueCommandBllock_t m_queueCommand;
};

} // Otus::

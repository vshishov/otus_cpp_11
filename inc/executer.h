#pragma once

#include "command.h"
#include "observer.h"
#include "reader.h"

#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace Otus {

class Excuter : public IObserver<CommandBlock>, public std::enable_shared_from_this<Excuter>
{
public:
  static std::shared_ptr<Excuter> Create(const std::string& a_strName, std::shared_ptr<Reader>& a_pReader, std::ostream& a_osOut = std::cout, std::ostream& a_osMetricsOut = std::cout);
  ~Excuter();

  void Update(const CommandBlock& a_CommandBlock) override;

private:
  Excuter(const std::string& a_strName, std::ostream& a_osOut, std::ostream& a_osMetricsOut);
  void SetReader(std::shared_ptr<Reader>& a_pReader);
  
  void Procces(std::string a_strName);
  void JoinThred();
  
private:
  std::ostream& m_osOut;
  std::ostream& m_osMetricsOut;
  std::weak_ptr<Reader> m_pReader;

  std::atomic<bool> m_bDone;
  std::thread m_thread;
  std::mutex m_queueLock;
  std::mutex m_printLock;
  std::condition_variable m_queueCheck;
  QueueCommandBllock_t m_queueCommand;
};

} // Otus::

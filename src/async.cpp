#include "async.h"

#include "logger.h"
#include "executer.h"
#include "context.h"


#include <map>
#include <memory>

namespace async {

std::map<handle_t, std::shared_ptr<Otus::Context> > _Contexts;
std::mutex _contextLock;

static handle_t RegisterContext(const std::shared_ptr<Otus::Context>& a_pContext)
{
  std::lock_guard<std::mutex> lock(_contextLock);  
  _Contexts.emplace(std::make_pair(a_pContext.get(), a_pContext));
  return a_pContext.get();
}

static void ProccessBuffer(handle_t handle, const char* data, std::size_t size)
{
  std::shared_ptr<Otus::Context> context;
  {
    std::lock_guard<std::mutex> lock(_contextLock);
    auto iter = _Contexts.find(handle);
    if (iter != _Contexts.end()) {
      context = iter->second;
    }
  }
  context->ProccessBuffer(data, size);
}

static void UnRegisterContext(handle_t handle)
{
  std::lock_guard<std::mutex> lock(_contextLock);
  auto iter = _Contexts.find(handle);
  if (iter != _Contexts.end()) {
  _Contexts.erase(iter);
  }
}

handle_t connect(std::size_t bulk)
{
  auto pContext = std::make_shared<Otus::Context>(bulk);
  if (pContext) {
    return RegisterContext(pContext);
  }
  return nullptr;
}

void receive(handle_t handle, const char* data, std::size_t size)
{
  ProccessBuffer(handle, data, size);
}

void disconnect(handle_t handle)
{
  UnRegisterContext(handle);
}

} // async::

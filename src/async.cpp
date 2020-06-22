#include "async.h"

#include "context.h"

#include <map>
#include <memory>

namespace async {

std::map<handle_t, std::shared_ptr<Otus::Context> > _Contexts;

handle_t connect(std::size_t bulk) {
  bulk = bulk;
  return nullptr;
}

void receive(handle_t handle, const char* data, std::size_t size) {
  handle = handle;
  data = data;
  size = size;
}

void disconnect(handle_t handle) {
  handle = handle;
}

} // async::

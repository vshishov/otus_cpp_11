#include "async.h"

namespace async {

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

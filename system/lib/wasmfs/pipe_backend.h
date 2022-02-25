#include <iostream>
// Copyright 2022 The Emscripten Authors.  All rights reserved.
// Emscripten is available under two separate licenses, the MIT license and the
// University of Illinois/NCSA Open Source License.  Both these licenses can be
// found in the LICENSE file.

#pragma once

#include <queue>

#include "file.h"
#include "support.h"

namespace wasmfs {

// TODO: something more optimal.
using PipeData = std::queue<uint8_t>;

class PipeFile : public DataFile {
  std::shared_ptr<PipeData> data;

  __wasi_errno_t write(const uint8_t* buf, size_t len, off_t offset) override {
//std::cout << "pipe write " << len << " to " << data.get() << '\n';
    for (size_t i = 0; i < len; i++) {
//std::cout << "  pipe write " << int(buf[i]) << '\n';
      data->push(buf[i]);
    }

    return __WASI_ERRNO_SUCCESS;
  }

  __wasi_errno_t read(uint8_t* buf, size_t len, off_t offset) override {
//std::cout << "pipe read up to " << len << " from " << data.get() << " which has " << data->size() << '\n';
    for (size_t i = 0; i < len; i++) {
      if (data->empty()) {
        return __WASI_ERRNO_INVAL;
      }
      buf[i] = data->front();
//std::cout << "  pipe read " << int(buf[i]) << '\n';
      data->pop();
    }

    return __WASI_ERRNO_SUCCESS;
  }

  void flush() override {}

  size_t getSize() override { return data->size(); }

  void setSize(size_t size) override {
    // no-op
  }

public:
  PipeFile(mode_t mode, backend_t backend, std::shared_ptr<PipeData> data) : DataFile(mode, backend), data(data) {}

  bool seekable() const override { return false; }
};

class PipeBackend : public Backend {
public:
  std::shared_ptr<DataFile> createFile(mode_t mode) override {
    WASMFS_UNREACHABLE("PipeBackend cannot create normal files");
  }
};

} // namespace wasmfs

#pragma once

#include "rust/cxx.h"
#include "libnixstore/src/lib.rs.h"

namespace libnixstore {
void init();
rust::String get_store_dir();
rust::String get_real_store_dir();

} // namespace libnixstore

#pragma once

#include "rust/cxx.h"
#include "libnixstore/src/lib.rs.h"

namespace libnixstore {
void init();
bool is_valid_path(rust::Str path);
InternalPathInfo query_path_info(rust::Str path, bool base32);
rust::String query_path_from_hash_part(rust::Str hash_part);
rust::String get_store_dir();
rust::String get_real_store_dir();
rust::String get_build_log(rust::Str derivation_path);

} // namespace libnixstore

#include "libnixstore/include/nix.h"

#include <nix/canon-path.hh>
#include <nix/config.h>
#include <nix/derivations.hh>
#include <nix/globals.hh>
#include <nix/shared.hh>
#include <nix/store-api.hh>
#include <nix/local-fs-store.hh>
#include <nix/log-store.hh>
#include <nix/content-address.hh>
#include <nix/util.hh>

#include <nix/nar-accessor.hh>

#include <nlohmann/json.hpp>

#include <stdlib.h>

// C++17 std::visit boilerplate
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

static nix::ref<nix::Store> get_store() {
  static std::shared_ptr<nix::Store> _store;
  if (!_store) {
    nix::initLibStore(true);

    nix::Store::Params params;
    // Disable caching since we run as a deamon and non-reproduceable builds
    // might have a different result for hashes
    params["path-info-cache-size"] = "0";

    // Disable substituting since we don't want to pull from other caches,
    // this also avoids potential recursion
    params["substitute"] = "false";
    _store = openStore(nix::settings.storeUri, params);
  }
  return nix::ref<nix::Store>(_store);
}

// shorthand to create std::string_view from rust::Str, we dont wan't to create
// std::string because that involves allocating memory
#define STRING_VIEW(rstr) std::string(rstr.data(), rstr.length())

namespace libnixstore {
void init() {
  get_store();
}

rust::String get_store_dir() {
  return nix::settings.nixStore;
}

rust::String get_real_store_dir() {
  auto store = get_store();
  auto *fsstore = dynamic_cast<nix::LocalFSStore *>(&(*store));

  if (fsstore != nullptr)
    return fsstore->getRealStoreDir();
  else
    return get_store_dir();
}

} // namespace libnixstore

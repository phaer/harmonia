# libnixstore

These are libnix bindings required by harmonia to communicate with the local nix daemon.
Over time we will replace the dependencies on libnix with rust-native code.

Note: This project provides bindings, this makes the project automatically unsafe.

Supported nix version:
- nix 2.24

## Requirements

It is only available for systems that have the nix package manager installed.
To achieve this you should setup a simple shell.nix

```nix
with import <nixpkgs> { };
stdenv.mkDerivation {
  name = "xyz";
  nativeBuildInputs = [ rustc cargo gcc pkg-config ];
  buildInputs = [
    # required
    nix
    nlohmann_json
    boost

    # additional packages you might need
    rustfmt
    clippy
    # ...
  ];

  RUST_SRC_PATH = "${pkgs.rust.packages.stable.rustPlatform.rustLibSrc}";
}
```

## Example

```rust
libnixstore::init();
println!("{}", libnixstore::get_store_dir());
```

#![warn(clippy::dbg_macro)]
#![doc = include_str!("../README.md")]
#![cfg_attr(docsrs, feature(doc_cfg))]
#![deny(
    nonstandard_style,
    rust_2018_idioms,
    rustdoc::broken_intra_doc_links,
    rustdoc::private_intra_doc_links
)]
#![forbid(non_ascii_idents)]

#[cxx::bridge(namespace = "libnixstore")]
mod ffi {
    unsafe extern "C++" {
        include!("libnixstore/include/nix.h");

        fn init();
        fn get_store_dir() -> String;
        fn get_real_store_dir() -> String;
    }
}

#[inline]
/// Perform any necessary effectful operation to make the store up and running.
pub fn init() {
    ffi::init();
}

#[inline]
#[must_use]
/// Returns the path to the directory where nix store sources and derived files.
pub fn get_store_dir() -> String {
    ffi::get_store_dir()
}

#[inline]
#[must_use]
/// Returns the physical path to the nix store.
pub fn get_real_store_dir() -> String {
    ffi::get_real_store_dir()
}

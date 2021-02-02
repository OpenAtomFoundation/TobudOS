#![no_std]

extern crate cortex_m;

mod bridge;

use crate::bridge::*;
use cty::*;

#[no_mangle]
pub extern "C" fn application_entry_rust() -> c_void {
    unsafe {
        rust_print(b"[+] Welcome to the RUST-WORLD in TencentOS :)".as_ptr());
        rust_mqtt_daemon();
    }

    loop {
        unsafe {
            rust_print(b"[+] This is a mqtt demo!".as_ptr());
            rust_sleep(5000u32);
        }
    }
}

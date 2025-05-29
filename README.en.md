# CH32 Loongson EJTAG

**This English version README is translated using DeepSeek-V3; when discrepancies exist the Chinese version should be considered effective.**

This project utilizes the CH32V203F6P6 microcontroller and is based on the successful experience and core source code of [pico-loongson-ejtag](https://github.com/RigoLigoRLC/pico-loongson-ejtag), aiming to provide a cost-optimized implementation of the Loongson EJTAG debug probe. It strives for full compatibility with the official `la_dbg_tool_usb` implementation, reducing developer costs and expanding the Loongson embedded ecosystem.

## Hardware

Located in the `hardware` directory. Designed using EasyEDA Professional for ease of study and research.

## Firmware Feature Completeness

The current version does not fully implement all command executions, which may result in certain debugging features being unavailable:

- **0xe, 0xf: Fast Memory Read Instructions**  
  This will disable bulk memory read operations.

Some features have not been thoroughly validated and may exhibit anomalies on certain machines:

- **0xc, 0xd: Fast Memory Write Instructions**  
  Untested on machines other than the 2K0300, with unknown compatibility for multi-core processors. SPI Flash programming has only been verified on the 2K0300.

GPIO operation functions are unimplemented.  
JTAG communication speed is currently fixed at 2MHz.

## Building

You need to use MounRiver Studio II for standard WCH RISC-V microcontroller development or manually build using the MRS Toolchain (the latter is never tested). This will not be elaborated further here.

## License

This project incorporates the following components:  
- **CherryUSB** and **CherryRB**, both distributed under the Apache 2.0 License.  
- The CH32 foundational library provided by WCH.  
- The Loongson EJTAG implementation (located in `Library/lsejtag`, `User/lsejtag_impl.c`, and `User/lsejtag_impl.h`) is distributed under the GPL 3.0 License.  

The entire project is distributed under the **GPL 3.0 License**.  
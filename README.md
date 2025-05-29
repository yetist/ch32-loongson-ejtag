
# CH32 Loongson EJTAG

[ENGLISH](README.en.md)

本项目是采用了 CH32V203F6P6 单片机、基于 [pico-loongson-ejtag](https://github.com/RigoLigoRLC/pico-loongson-ejtag) 的成功经验与核心源码制作的、压缩成本版本的龙芯 EJTAG 调试探头实现。力求与官方 `la_dbg_tool_usb` 实现完全兼容，降低开发者使用成本、扩展龙芯嵌入式生态。

## 硬件

位于 hardware 目录中。为方便大家学习研究，使用了立创 EDA 专业版设计。

## 固件功能完整度

当前版本还未完整实现所有命令的执行，会导致部分调试功能无法使用：

- 0xe，0xf：快速读内存指令

  将导致批量读取内存不可用

部分功能未完整验证，可能在部分机器上工作异常：

- 0xc，0xd：快速写内存指令

  未在 2K0300 以外的机器上测试，多核心处理器的适应性不明。仅在 2K0300 上测试了 SPI Flash 烧写功能。

GPIO 操作函数未实现；
JTAG 通信速度目前固定为 2MHz；

## 构建

你需要使用 MounRiver Studio II 进行标准的沁恒 RISC-V 单片机开发流程，或者使用 MRS Toolchain 手动构建（从未测试）。此处不再多赘述。

## 许可证

本项目使用到了 CherryUSB、CherryRB ；它们均以 Apache 2.0 协议分发；
本项目使用到了沁恒提供的 CH32 基础函数库；
本项目的龙芯 EJTAG 实现（Library/lsejtag；User/lsejtag_impl.c；User/lsejtag_impl.h）以 GPL 3.0 协议分发。

项目整体以 GPL 3.0 协议分发。

# frostwm

A hobby project that I'm working on. A window manager that utilizes the wayland
protocol and aims to be easily customizable.

Requirements:
- A C++20 compiler
- Meson
- Wayland
- wlroots v0.19

## Building

```
meson setup build && cd build
ninja
./frostwm
```

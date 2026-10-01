# Monitoring-System

## Build

Requires CMake, a C++20 compiler, GTKmm 4, Boost, Mailio, TgBot, OpenSSL, and
toml++.

```sh
cmake -S . -B build
cmake --build build --parallel
```

The executable is written to `build/monitoring`; agent shared libraries are
written to `build/agents/`. Run the application from any working directory with:

```sh
./build/monitoring
```

# FALCON
## FPGA-Accelerated Low-latency Computing Order-book Network
FPGA-accelerated low-latency electronic trading platform with a C++ matching engine, hardware packet parser, lock-free queues, and performance benchmarking utilizing a CMOD A7.

## Current Status
Software matching engine with a price-time priority limit order book, trade execution (full, partial, and multi-level fills), and GoogleTest coverage for core matching scenarios.

## Build
```powershell
cmake -S . -B build
cmake --build build --config Debug
```

## Test
```powershell
ctest --test-dir build -C Debug --output-on-failure
```

## Layout
- `include/` / `src/` — types, order book, matching engine
- `tests/` — matching engine unit tests
- `CMakeLists.txt` — app (`falcon`) and test (`falcon_tests`) targets

## Roadmap
Cancel support, market orders, market-data / networking path, then FPGA acceleration and benchmarking on the Digilent Cmod A7.

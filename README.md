# teks

`teks` is a text editor project in modern C++20 using Qt Widgets.
This is not a production text editor; the purpose is education and recreation.

## Dependencies

- C++20 compiler
- CMake 3.28+
- Ninja
- Qt 6 (Widgets)
- Git submodules

## Setup

```bash
git submodule update --init --recursive
export TEKS_QT6_DIR="$HOME/Qt/6.10.2/macos/lib/cmake/Qt6"
```

## Build, Test, Run (Scripts)

```bash
./build
./test
./run
```

## Manual Commands

```bash
cmake -S . -B ./cmake-build/debug-test -G Ninja \
  -DTEKS_UNIT_TEST=TRUE \
  -DQt6_DIR="$TEKS_QT6_DIR"

cmake --build ./cmake-build/debug-test
ctest --test-dir ./cmake-build/debug-test --output-on-failure
./cmake-build/debug-test/modules/app/teks_app
```

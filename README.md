# Dying Light 2 Developer Menu PoC

A proof of concept for using the developer menu in the Steam version of the Dying Light 2 game by Techland.

This approach directly uses DevTools to play the main game with a wrapper DLL that applies some memory editing for enabling the developer menu.

## Building

```
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --target package
```

## Installation

See `packaging/readme.txt` for installation details.

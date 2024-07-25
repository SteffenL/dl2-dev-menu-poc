# Dying Light 2 Developer Menu PoC

A proof of concept for using the developer menu in the Dying Light 2 game by Techland.

This approach directly uses DevTools to play the main game with a wrapper DLL that applies some memory editing for enabling the developer menu.

No game files are modified, and no files from the game are redistributed. Online features are and should remain unavailable.

## Development

### Prerequisites

* Compiler supporting C++20 (VC++ 2022).
* Steamworks SDK (1.60).

### Building

```
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Release -Dsteamworks_ROOT=<Steamworks SDK path>
cmake --build build --target package
```

### Installation

See `packaging/readme.txt` for installation details.

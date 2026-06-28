# Doom-C

A Doom-style raycasting game written in C.  
Uses **SDL2**, **SDL2_image** and **SDL2_mixer**.

---

## Prerequisites

The build relies on `pkg-config` to locate SDL2. Install the toolchain and the
SDL2 libraries for your platform.

### Linux

```bash
# Debian / Ubuntu
sudo apt install build-essential pkg-config \
                 libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev

# Arch
sudo pacman -S base-devel pkgconf sdl2 sdl2_image sdl2_mixer
```

### Windows (native, MSYS2)

Install [MSYS2](https://www.msys2.org/), then open the **MSYS2 MinGW64**
terminal and run:

```bash
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-make \
          mingw-w64-x86_64-pkg-config \
          mingw-w64-x86_64-SDL2 \
          mingw-w64-x86_64-SDL2_image \
          mingw-w64-x86_64-SDL2_mixer
```

### Cross-compiling a Windows `.exe` from Linux

```bash
# Arch (MinGW-w64 + SDL2 packages, e.g. from the AUR / repos)
sudo pacman -S mingw-w64-gcc mingw-w64-pkg-config \
               mingw-w64-sdl2 mingw-w64-sdl2_image mingw-w64-sdl2_mixer
```

---

## Build

The `Makefile` (in the `doomC/` directory) is cross-platform. Header files live
in `entete/` and are picked up automatically via `-Ientete`.

```bash
git clone <your-repo-url>
cd c/doomC

# Native build for the current OS (produces ./doom, or doom.exe on Windows)
make

# Cross-compile a Windows binary from Linux (produces doom.exe)
make windows

# Remove object files and binaries
make clean
```

> On the **MSYS2 MinGW64** terminal, use `mingw32-make` instead of `make`.

The `resources/` folder must sit next to the executable at runtime. It already
lives in `doomC/`, so running from there works out of the box.

---

## Run

```bash
./doom          # Linux
./doom.exe      # Windows
```

---

## Controls

| Key / Input       | Action              |
|-------------------|---------------------|
| `W` `A` `S` `D`  | Move                |
| Mouse move        | Look left / right   |
| Left click        | Shoot               |
| `Escape`          | Quit                |

---

## Project structure

```
doomC/
├── Makefile             # Cross-platform build (Linux native + Windows)
├── main.c               # .c source files at the root
├── map.c
├── player.c
├── raycasting.c
├── object_renderer.c
├── sprite_object.c
├── npc.c
├── pathfinding.c
├── weapon.c
├── sound.c
├── object_handler.c
├── entete/              # All header (.h) files (included via -Ientete)
│   ├── main.h
│   ├── map.h
│   ├── player.h
│   ├── raycasting.h
│   ├── object_renderer.h
│   ├── sprite_object.h
│   ├── npc.h
│   ├── pathfinding.h
│   ├── weapon.h
│   ├── sound.h
│   ├── object_handler.h
│   ├── settings.h
│   └── types.h
└── resources/           # Textures, sprites and sounds
    ├── textures/
    ├── sprites/
    └── sound/
```

---

## Architecture

Each Python class was translated to a C struct with associated functions:

| Python class       | C equivalent                          |
|--------------------|---------------------------------------|
| `Game`             | `struct Game` (main.h)                |
| `Map`              | `struct Map` (map.h)                  |
| `Player`           | `struct Player` (player.h)            |
| `RayCasting`       | `struct RayCasting` (raycasting.h)    |
| `ObjectRenderer`   | `struct ObjectRenderer`               |
| `SpriteObject`     | `struct SpriteObject`                 |
| `AnimatedSprite`   | `struct AnimatedSprite` (base of NPC) |
| `NPC`              | `struct NPC` (npc.h)                  |
| `PathFinding`      | `struct PathFinding` (pathfinding.h)  |
| `Weapon`           | `struct Weapon` (weapon.h)            |
| `Sound`            | `struct Sound` (sound.h)              |
| `ObjectHandler`    | `struct ObjectHandler`                |

`pygame` → **SDL2**, `pygame.mixer` → **SDL2_mixer**, `pygame.image` → **SDL2_image**

## Use of AI :

`MakeFile`  ||  `Commentary`  ||  `Readme`
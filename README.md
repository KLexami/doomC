# Doom-C

A Doom-style raycasting game written in C.  
Uses **SDL2**, **SDL2_image** and **SDL2_mixer**.

---

## Prerequisites

Install [MSYS2](https://www.msys2.org/), then open the **MSYS2 MinGW64** terminal and run:

```bash
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-make \
          mingw-w64-x86_64-SDL2 \
          mingw-w64-x86_64-SDL2_image \
          mingw-w64-x86_64-SDL2_mixer
```

---

## Build

In the **MSYS2 MinGW64** terminal:

```bash
git clone <your-repo-url>
cd doom-c/doomC

# Copy the resources folder next to the executable
cp -r ../resources .

mingw32-make
```

---

## Run

```bash
./doom.exe
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
.
├── doomC/               # C source files
│   ├── main.c / main.h
│   ├── map.c / map.h
│   ├── player.c / player.h
│   ├── raycasting.c / raycasting.h
│   ├── object_renderer.c / object_renderer.h
│   ├── sprite_object.c / sprite_object.h
│   ├── npc.c / npc.h
│   ├── pathfinding.c / pathfinding.h
│   ├── weapon.c / weapon.h
│   ├── sound.c / sound.h
│   ├── object_handler.c / object_handler.h
│   ├── settings.h
│   ├── types.h
│   └── Makefile
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

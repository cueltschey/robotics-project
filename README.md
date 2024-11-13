# AI robotics project 2024:

## Dependencies:

- OpenGL
- libgl-dev
- GLM
- GLFW
- GLEW
- Assimp
- SDL2

``` bash
sudo apt-get install libgl-dev libglm-dev libglfw3-dev libglew-dev libassimp-dev libsdl2-dev libsdl2-mixer-dev
```

## Building:
```bash
git clone https://github.com/cueltschey/robotics-project
mkdir -p build
cd build
cmake ../ && make -j$(nproc)
```

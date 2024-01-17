# HierarchicalScenes

Example of hierarchical queries with Flecs to process nested transforms on a scene

## Building
First install all dependencies needed to build raylib from source, then fetch code with submodules:
```bash
git clone --recurse-submodules https://github.com/cedmundo/HierarchicalScenes.git
```
Build with CMake:
```bash
mkdir -p cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=debug -B cmake-build-debug
cmake --build cmake-build-debug 
```
Done.
# ArcStudio

## Install
```
git clone --recursive https://github.com/albers4/arc-studio
```

```
cd arc-studio
mkdir build && cd build
```

```
cmake ..
# or cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
```

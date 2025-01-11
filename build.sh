source ./emsdk/emsdk_env.sh
mkdir -p bin
em++ src/main.cpp -s WASM=1 -o bin/runtime.mjs
em++ src/main.cpp -s WASM=1 -o bin/runtime.cjs

source ./emsdk/emsdk_env.sh
mkdir -p bin
compile(){
    em++ src/main.cpp -O3 -s WASM=1 -s ASSERTIONS=0 -s ALLOW_MEMORY_GROWTH=1 -o $1
}
compile bin/runtime.cjs
compile bin/runtime.mjs
wget https://github.com/emscripten-core/emsdk/archive/refs/heads/main.zip
unzip main.zip
rm main.zip
mv emsdk-main emsdk
emsdk/emsdk install latest
emsdk/emsdk activate latest
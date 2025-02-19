#!/bin/bash

if [ ! -d cmake-build-debug ]; then
    mkdir cmake-build-debug
fi

cmake -S . -B cmake-build-debug/ -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

if [ ! -f game ]; then
    ln -s cmake-build-debug/game game
fi

if [ ! -f compile_commands.json ]; then
    ln -s cmake-build-debug/compile_commands.json compile_commands.json
fi


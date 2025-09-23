#!/usr/bin/env bash
set -e


sudo apt update
sudo apt install -y build-essential cmake git \
    libfreetype6-dev libwayland-dev libx11-dev libxrandr-dev \
    libxcursor-dev libxi-dev libegl-dev libgbm-dev libasound2-dev \
    libpulse-dev pkg-config


if [ ! -d "SDL" ]; then
  git clone https://github.com/libsdl-org/SDL.git
fi

cd SDL
git pull

# Optionally checkout a specific stable tag, e.g.:
git checkout release-3.2.22

mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j"$(nproc)"

sudo make install
sudo ldconfig

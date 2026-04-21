#!/bin/bash

set -e

echo "Building Coppric..."
gcc src/*.c -o coppric

echo "Installing syntax highlighting..."
mkdir -p ~/.config/nvim/syntax
mkdir -p ~/.config/nvim/ftdetect
cp tools/cpr.vim ~/.config/nvim/syntax/
cp tools/ftdetect-cpr.vim ~/.config/nvim/ftdetect/cpr.vim

echo "Done. Move ./coppric to a directory in your PATH (e.g. /usr/local/bin) to use globally."

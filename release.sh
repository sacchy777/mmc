#!/bin/sh
rm -rf release
mkdir release
cp ctplay_bin/* release/
cp Mmcg/Mmcg/bin/Release/* release/
cp mmc.dll release

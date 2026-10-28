#!/bin/bash
# Build sngrep natively on Windows (MSYS2 UCRT64 shell) and assemble dist/
# Prerequisites: Npcap installed, MSYS2 packages: gcc cmake ninja ncurses libpcap
#                pcre2 libsystre pkgconf mingw-w64-tools
set -e
root="$(cd "$(dirname "$0")" && pwd)"
cd "$root"

# Import library for Npcap's wpcap.dll (MSYS2 libpcap cannot talk to Npcap)
if [ ! -f tools/libwpcap.a ]; then
    gendef - /c/Windows/System32/Npcap/wpcap.dll > tools/wpcap.def
    dlltool -d tools/wpcap.def -D wpcap.dll -y tools/libwpcap.a
fi

cmake -S src -B build -G Ninja -DWITH_UNICODE=yes -DWITH_PCRE2=yes -DWITH_OPENSSL=yes -DWITH_ZLIB=yes -DUSE_EEP=yes -DUSE_IPV6=yes \
    -DDISABLE_LOGO=yes -DNPCAP_IMPORT_LIB="$root/tools/libwpcap.a"
cmake --build build

# Single self-contained exe (only Npcap must be installed)
rm -rf dist && mkdir dist
cp build/sngrep.exe dist/

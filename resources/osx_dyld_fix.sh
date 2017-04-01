#!/bin/bash

binary="$1"
if [ -z "$binary" ]; then
  echo "Usage: $0 binary"
  exit 1
fi
if [ ! -f "$binary" ]; then
  echo "Could not find $binary"
  exit 1
fi

reldir="../Frameworks"

libs=$(otool -L "$binary" | fgrep '/usr/local' | cut -f 2 | cut -d ' ' -f 1)
for lib in $libs; do
  echo "Processing $lib..."
  cp -v "$lib" "$(dirname $binary)/$reldir"
  install_name_tool -change \
    "$lib" \
    @executable_path/"$reldir/$(basename "$lib")" \
    "$binary"
done

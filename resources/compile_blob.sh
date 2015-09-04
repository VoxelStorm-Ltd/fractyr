#!/bin/bash
# Compile binary resources into linkable object files for loading with blobloader.h
# for use with VoxelStorm projects primarily on OS X
# Example for use in Code::Blocks: ./compile_blob.sh resources/$file_name.$file_ext $object

infile="$1"
outfile="$2"
compiler="$3"
if [ "$compiler" = "" ]; then
  compiler="g++5"
  if [ -z "$(which "$compiler")" ]; then
    compiler="g++"
  fi
fi

if echo "$MACHTYPE" | grep -iq "apple"; then
  # os x requires special treatment
  xxd -i "$infile" | sed 's/\(resources[a-zA-Z0-9_]*\)/_binary_\1_x/' | "$compiler" -c -o "$outfile" -x c++ -
else
  # everything else works with gnu ld
  ld -r -b binary -o "$outfile" "$infile"
fi

#!/bin/bash
# Compile binary resources into linkable object files for loading with blobloader.h
# for use with VoxelStorm projects primarily on OS X
# Example for use in Code::Blocks: ./compile_blob.sh resources/$file_name.$file_ext $object

infile="$1"
outfile="$2"
compiler="$3"
options="$4"
link_options="$5"
#echo "Infile: $infile (eol)"
#echo "Outfile: $outfile (eol)"
#echo "Compiler: $compiler (eol)"
#echo "Options: $options (eol)"
#echo "Link options: $link_options (eol)"

if [ "$compiler" = "" ]; then
  compiler="g++-6"
fi
if [ -z "$(which "$compiler")" ]; then
  compiler="g++-5"
fi
if [ -z "$(which "$compiler")" ]; then
  compiler="g++"
fi

# convert absolute path to relative if it's a subdirectory
pwd="$(pwd | sed 's/\//\\\//g')\/"
infile="$(sed "s/$pwd//" <<< "$infile")"
#echo "Relative path $infile"

if grep -iq ".glsl$" <<< "$infile"; then
  # additional validation step for glsl shaders
  shortfilename="$(basename "$infile" | sed 's/_vert.glsl$//;s/_frag.glsl$//')"
  resources/validate_shader.sh "$shortfilename"
fi

if grep -iq "apple" <<< "$MACHTYPE"; then
  # os x requires special treatment
  xxd -i "$infile" | sed 's/\(resources[a-zA-Z0-9_]*\)/_binary_\1_x/' | "$compiler" -c -o "$outfile" -x c++ -
elif grep -iq "linux" <<< "$MACHTYPE"; then
  # de-parallelise blob compiles on linux to prevent OOM situations with high parallelism
  #process_limit=4
  process_limit=3
  process_to_watch="xxd\|cc1plus"
  process_list=$(ps aux | grep "$process_to_watch")
  process_count=$(wc -l <<< "$process_list")
  while [ "$process_count" -ge "$process_limit" ]; do
    other_pid=$(tail -1 <<< "$process_list" | tr -s ' ' | cut -d ' ' -f 2)
    #echo "De-parallelising blob compiler: waiting for process $other_pid to finish ($process_count total)..."
    sleep 2
    process_list=$(ps aux | grep "$process_to_watch")
    process_count=$(wc -l <<< "$process_list")
  done

  # everything else just works with gnu ld, but we need to select bitness
  if grep -Fq -- '-m32' <<< "$options"; then
    #ld -m elf_i386 -r -b binary -o "$outfile" "$infile"
    xxd -i "$infile" | sed 's/\[\] = {/\[\] __attribute__((__aligned__(16))) = {/;s/\(resources[a-zA-Z0-9_]*\)/_binary_\1_x/' | "$compiler" -m32 -c -o "$outfile" -x c++ -
  else
    #ld -r -b binary -o "$outfile" "$infile"
    xxd -i "$infile" | sed 's/\[\] = {/\[\] __attribute__((__aligned__(16))) = {/;s/\(resources[a-zA-Z0-9_]*\)/_binary_\1_x/' | "$compiler" -c -o "$outfile" -x c++ -
  fi
else
  # ...except if we're on windows, and then hoops need to be jumped through for backslashes
  outfile="$(sed 's/\\/\//g' <<< "$outfile")"
  ld -r -b binary -o "$outfile" "$infile"
fi

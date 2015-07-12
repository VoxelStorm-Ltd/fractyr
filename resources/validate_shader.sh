#!/bin/bash
# Helper script to call the shader validator with the right shaders
#
# Validate and build shaders in code::blocks by assigning the following build commands to .glsl files:
#   bash -c "resources/validate_shader.sh $file_name"
#   ld -r -b binary -o $object resources/shaders/$file_name.$file_ext
# or under OS X, instead of ld, use:
#   resources/compile_blob.sh resources/shaders/$file_name.$file_ext $object

name=${1%_*}

if [ "${1##*_}" = "vert" ]; then
  # skip on vertex shader; validate vert and frag together when called on frag
  #exit
  :
  # not doing this means all glsl is validated twice, but this is fast and cheap
  # and ensures that whichever shader is modified, both are validated together.
fi

shader_vert="resources/shaders/$name""_vert.glsl"
shader_frag="resources/shaders/$name""_frag.glsl"

#echo "DEBUG: shader_vert $shader_vert"
#echo "DEBUG: shader_frag $shader_frag"

if echo "$MACHTYPE" | grep -iq "apple"; then
  platform="Mac"
  extension=""
elif echo "$MACHTYPE" | grep -iq "linux"; then
  platform="Linux"
  extension=""
else
  platform="Win"
  extension=".exe"
fi
if [ "$(uname -m)" = x86_64 ]; then
  bitness="64"
else
  bitness="32"
fi

./resources/ShaderValidator_"$platform""$bitness""$extension" "$shader_vert" "$shader_frag"

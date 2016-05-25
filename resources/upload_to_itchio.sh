#!/bin/bash

scriptdir=$(dirname "${BASH_SOURCE[0]}")
repodir=$(cd "$scriptdir/.."; pwd)
repo=${repodir##*/}

if grep -iq "apple" <<< "$MACHTYPE"; then
  # os x
  platform="osx-64"
  platform_cbx="Mac64_Release"
  butler=~/Library/Application\ Support/itch/bin/butler
elif grep -iq "linux" <<< "$MACHTYPE"; then
  # linux
  platform="linux-64"
  platform_cbx="Linux64_Release"
  butler=~/.config/itch/bin/butler
else
  # windows
  platform="windows-32"
  platform_cbx="Win32_Release"
  butler=%APPDATA%/Roaming/itch/bin/butler.exe
fi
channel="$repo-for-$platform"

version=$(grep "FULLVERSION_STRING" "$repodir/version.h" | cut -d '"' -f 2)
oldversion=$(
  wget -q -O - "https://itch.io/api/1/x/wharf/latest?target=voxelstorm/$repo&channel_name=$channel" \
    --user-agent="VoxelStorm Game Uploader" \
    | jq -r .latest
  )

echo "Existing version of $repo is $oldversion in $channel."
if [ "$oldversion" == "$version" ]; then
  echo "Nothing to do - our version is the latest."
  exit
fi

binpath=$(fgrep '<Target title="'"$platform_cbx"'">' *.cbp -A5 | fgrep 'Option output=' | cut -d '"' -f 2)
echo "Copying $binpath to temporary location..."
tempdir="/dev/shm/temp_itchio"
mkdir -p "$tempdir"
cp "$repodir/$binpath" "$tempdir/" || exit 1
echo "Uploading $repo version $version to $channel..."

"$butler" push "$tempdir" "voxelstorm/$repo:$repo-for-$platform" --userversion "$version"

rm -r "$tempdir"&

echo "Verifying uploaded version..."
sleep 1
newversion=$(
  wget -q -O - "https://itch.io/api/1/x/wharf/latest?target=voxelstorm/$repo&channel_name=$channel" \
    --user-agent="VoxelStorm Game Uploader" \
    | jq -r .latest
  )

if [ "$newversion" == "$version" ]; then
  echo "Uploaded version of $repo verified as $newversion."
else
  echo "Uploaded version of $repo NOT CORRECT!  Version $newversion is online, while it should be $version."
fi

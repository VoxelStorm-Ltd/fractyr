#!/bin/bash

scriptdir=$(dirname "${BASH_SOURCE[0]}")
repodir=$(cd "$scriptdir/.."; pwd)
repo=${repodir##*/}

if grep -iq "apple" <<< "$MACHTYPE"; then
  # os x
  platform="osx-64"
  platform_cbx="Mac64_Release"
  butler=~/Library/Application\ Support/itch/bin/butler
  if ! [ -f "$butler" ]; then
    butler="resources/itchio_butler_mac64.exe"
  fi
  tempdir="$TMPDIR"
elif grep -iq "linux" <<< "$MACHTYPE"; then
  # linux
  platform="linux-64"
  platform_cbx="Linux64_Release"
  butler=~/.config/itch/bin/butler
  if ! [ -f "$butler" ]; then
    butler="resources/itchio_butler_linux64.exe"
  fi
  tempdir="/dev/shm/temp_itchio"
else
  # windows
  platform="windows-32"
  platform_cbx="Win32_Release"
  butler="$APPDATA/Roaming/itch/bin/butler.exe"
  if ! [ -f "$butler" ]; then
    butler="resources/itchio_butler_win32.exe"
  fi
  tempdir="./temp_itchio"
fi
channel="$repo-for-$platform"

if ! [ -f "$butler" ]; then
  echo "Could not find itch.io butler on your system, expected it at $butler - cannot continue.  You need to install itch.io client or download the butler manually."
  exit 1
fi

function get_online_version {
  #wget -q -O - "https://itch.io/api/1/x/wharf/latest?target=voxelstorm/$repo&channel_name=$channel" \
  #  --user-agent="VoxelStorm Game Uploader" \
  #  | jq -r .latest
  # compatible with windows toolchains:
  curl -s "https://itch.io/api/1/x/wharf/latest?target=voxelstorm/$repo&channel_name=$channel" \
    -A "VoxelStorm Game Uploader" \
    | cut -d '"' -f 4
}

version=$(grep "FULLVERSION_STRING" "$repodir/version.h" | cut -d '"' -f 2)
oldversion=$(get_online_version)

echo "Existing version of $repo is $oldversion in $channel."
if [ "$oldversion" == "$version" ]; then
  echo "Nothing to do - our version is the latest."
  exit
fi

binpath=$(grep -F '<Target title="'"$platform_cbx"'">' "$repodir/"*.cbp -A5 | grep -F 'Option output=' | head -1 | cut -d '"' -f 2)
echo "Copying $binpath to temporary location..."
mkdir -p "$tempdir" || exit 1
cp "$repodir/$binpath" "$tempdir/" || exit 1
echo "Uploading $repo version $version to $channel..."

"$butler" push "$tempdir" "voxelstorm/$repo:$repo-for-$platform" --userversion "$version"

rm -r "$tempdir"&

echo "Verifying uploaded version..."
sleep 1
newversion=$(get_online_version)
echo -n "Waiting to confirm processed version has updated correctly..."
while [ "$newversion" = "$oldversion" ]; do
  echo -n "."
  sleep 2
  newversion=$(get_online_version)
done
echo

if [ "$newversion" == "$version" ]; then
  echo "Uploaded version of $repo verified as $newversion."
else
  echo "Uploaded version of $repo NOT CORRECT!  Version $newversion is online, while it should be $version."
fi

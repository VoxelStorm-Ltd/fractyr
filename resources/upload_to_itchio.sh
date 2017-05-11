#!/bin/bash

scriptdir=$(dirname "${BASH_SOURCE[0]}")
repodir=$(cd "$scriptdir/.."; pwd)
repo=${repodir##*/}

if grep -iq "apple" <<< "$MACHTYPE"; then
  # macos
  platforms=("mac-64")
  platforms_cbx=("Mac64_Release")
  butler=~/Library/Application\ Support/itch/bin/butler
  if ! [ -f "$butler" ]; then
    butler="resources/itchio_butler_mac64.exe"
  fi
  tempdir="$TMPDIR"
  if [ -z "$tempdir" ]; then
    tempdir="./temp_itchio_$repo"
  else
    tempdir="$tempdir/temp_itchio_$repo"
    mkdir -p "$tempdir"
  fi
elif grep -iq "linux" <<< "$MACHTYPE"; then
  # linux
  #platforms=("linux-64" "linux-32")
  #platforms_cbx=("Linux64_Release" "Linux32_Release")
  platforms=("linux-64" "linux-32" "windows-64" "windows-32")
  platforms_cbx=("Linux64_Release" "Linux32_Release" "Win64_Release" "Win32_Release")
  butler=~/.config/itch/bin/butler
  if ! [ -f "$butler" ]; then
    butler="resources/itchio_butler_linux64.exe"
  fi
  tempdir="$(mktemp -d -p /dev/shm -t "itchio-$repo-XXXXXX")"
else
  # windows
  platforms=("windows-64" "windows-32")
  platforms_cbx=("Win64_Release" "Win32_Release")
  butler="$APPDATA/Roaming/itch/bin/butler.exe"
  if ! [ -f "$butler" ]; then
    butler="resources/itchio_butler_win32.exe"
  fi
  tempdir="./temp_itchio"
fi
if ! [ -f "$butler" ]; then
  echo "Could not find itch.io butler on your system, expected it at $butler - cannot continue.  You need to install itch.io client or download the butler manually."
  exit 1
fi

"$butler" upgrade --head --assume-yes && \
if grep -q "^resources/" <<< "$butler"; then
  # remove cached butler versions from the repository if they're saved here
  rm -f "$butler.new.gz" "$butler.old"
fi

function get_online_version {
  channel_to_check="$1"
  #wget -q -O - "https://itch.io/api/1/x/wharf/latest?target=voxelstorm/$repo&channel_name=$channel" \
  #  --user-agent="VoxelStorm Game Uploader" \
  #  | jq -r .latest
  # compatible with windows toolchains:
  curl -s "https://itch.io/api/1/x/wharf/latest?target=voxelstorm/$repo&channel_name=$channel_to_check" \
    -A "VoxelStorm Game Uploader" \
    | cut -d '"' -f 4
}

version_repo=$(grep "FULLVERSION_STRING" "$repodir/version.h" | cut -d '"' -f 2)

platforms_updated=""

for i in $(seq 0 $((${#platforms[@]} - 1))); do
  platform=${platforms[i]}
  platform_cbx=${platforms_cbx[i]}
  channel="$platform"
  oldversion=$(get_online_version "$channel")

  if grep -iq "apple" <<< "$MACHTYPE"; then
    # on MacOS, we upload the packaged disk image
    app_skel_dirs=("$repodir/resources/osx_app/"*.app)
    app_skel_dir=${app_skel_dirs[0]}
    binpath=~/Desktop/"$(basename "$app_skel_dir" .app)".dmg
  else
    if ! grep -q '<Target title="'"$platform_cbx"'">' "$repodir/"*.cbp; then
      echo "Could not find a target $platform_cbx in any project!  Skipping."
      continue
    fi
    binpath="$repodir/$(grep -F '<Target title="'"$platform_cbx"'">' "$repodir/"*.cbp -A5 | grep -F 'Option output=' | head -1 | cut -d '"' -f 2)"
    if grep -q "windows" <<< "$platform"; then
      binpath="$binpath.exe"
    fi
  fi
  if [ ! -f "$binpath" ] || [ ! -s "$binpath" ]; then
    echo "Binary $binpath has not been built!  Skipping."
    continue
  fi
  if grep -q "windows" <<< "$platform"; then
    version=$(
      timeout 10 \
        wine "$binpath" --version 2>/dev/null \
        | head -1 \
        | grep -o "version [^ ]* [^ ]*"
    )
  elif grep -iq "apple" <<< "$MACHTYPE"; then
    test_binpath="$repodir/$(grep -F '<Target title="'"$platform_cbx"'">' "$repodir/"*.cbp -A5 | grep -F 'Option output=' | head -1 | cut -d '"' -f 2)"
    version=$(
      "$test_binpath" --version \
        | head -1 \
        | grep -o "version [^ ]* [^ ]*"
    )
  else
    version=$(
      timeout 5 \
        "$binpath" --version \
        | head -1 \
        | grep -o "version [^ ]* [^ ]*"
    )
  fi
  if grep ':' <<< "${version%%* }"; then
    # remove the last field, as it's the git version
    version=${version% *}
  fi
  # remove all but the last field of the remaining version
  version=${version##* }
  if [ -z "$version" ]; then
    version="$version_repo"
    echo "Binary $binpath does not report version information, falling back to repo version $version"
  fi
  if [ "$version" != "$version_repo" ]; then
    echo "Warning: $binpath reports a different version from the repo!"
    echo "  Binary version: $version"
    echo "  Repo version:   $version_repo"
  fi

  if [ "$oldversion" == "invalid channel" ]; then
    echo "No previous uploads for channel $channel, this will be our first."
  else
    echo "Existing version of $repo is $oldversion in $channel."
    if [ "$oldversion" == "$version" ]; then
      if [ "$1" = "--force" ]; then
        echo "Versions are the same, but forcing an update anyway."
      else
        echo "Nothing to do for this channel - our version is the latest."
        continue
      fi
    fi
  fi

  echo "Copying $binpath to temporary location..."
  mkdir -p "$tempdir" || exit 1
  cp "$binpath" "$tempdir/" || exit 1
  echo "Uploading $repo version $version to $channel..."

  echo "Pushing temporary directory $tempdir"
  ls -alh "$tempdir"
  "$butler" push "$tempdir" "voxelstorm/$repo:$channel" --userversion "$version"

  rm -rf "$tempdir"

  echo -n "Verifying uploaded version..."
  newversion=$(get_online_version "$channel")
  while [ "$newversion" = "$oldversion" ] && [ "$oldversion" != "$version" ]; do
    echo -n "."
    sleep 2
    newversion=$(get_online_version "$channel")
  done
  echo

  if [ "$newversion" == "$version" ]; then
    echo "Uploaded version of $repo verified as $newversion."
    if [ "$oldversion" != "$version" ]; then
      platforms_updated="$platforms_updated$platform, "
    else
      echo "Not announcing updated build with the same version as the last."
    fi
  else
    echo "Uploaded version of $repo NOT CORRECT!  Version $newversion is online, while it should be $version."
  fi
done
# last ditch attempt to clean up the windows remnants regardless of what platform we're running on
rm resources/itchio_butler_win32.exe.* 2>/dev/null

sendtotelegram=""
if [ -f "../scripts/send_to_telegram.sh" ]; then
  sendtotelegram="../scripts/send_to_telegram.sh"
elif [ -f ~"/scripts/send_to_telegram.sh" ]; then
  sendtotelegram=~"/scripts/send_to_telegram.sh"
fi
if [ -f "$sendtotelegram" ]; then
  if [ ! -z "$platforms_updated" ]; then
    echo "Announcing updated platforms on Telegram..."
    # strip the trailing comma, and replace the last remainig separator comma of the list with an "and"
    platforms_updated=$(sed 's/, $//;s/\(.*\), /\1 and /' <<< "$platforms_updated")
    "$sendtotelegram" "Uploaded $repo for $platforms_updated version $version_repo to http://voxelstorm.itch.io/$repo" >/dev/null
  fi
fi

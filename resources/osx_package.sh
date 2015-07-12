#!/bin/bash
# Automatically create and populate an .app directory structure and package
# it as a standard OS X .dmg disk image.

# use the desktop as our temporary directory
target=~/Desktop

# use the default osx_app structure
app_skel_dirs=(resources/osx_app/*.app)
# select the first app file if there are more than one
app_skel_dir=${app_skel_dirs[0]}
# cache the app name
app_name=$(basename "$app_skel_dir" .app)
echo "Copying release files into $app_skel_dir (chosen from ${#app_skel_dirs[@]} app dirs)"

binary_dir="$app_skel_dir/Contents/MacOS"
mkdir -p "$binary_dir"
cp bin/Release/* "$binary_dir"/

# find out the size of our app
size_blocks=$(du -s "$app_skel_dir" | cut -f 1)
size_kb=$(du -s -k "$app_skel_dir" | cut -f 1)
# allocate an extra margin in the image
size_alloc_kb=$((size_kb + 4096))
echo "App name: $app_name, size: $size_kb""KB ($size_blocks blocks), allocating $size_alloc_kb""KB"

# create a writeable disk image of the correct size
dmg="$target/$app_name.dmg"
rm "$dmg" 2>/dev/null
hdiutil create -size "$size_alloc_kb"k -fs HFS+ -volname "$app_name" "$dmg"
if [ "$?" != "0" ]; then
  # if creation failed, don't continue
  echo "Aborting."
  exit 1;
fi
# mount it
devices=$(hdiutil attach "$dmg" | cut -f 1)
device=$(echo $devices | cut -f 2 -d ' ')
mountpoint=$(mount | grep "^$device on" | cut -d ' ' -f 3)
echo "$dmg mounted as $device on $mountpoint, copying app..."

cp -r "$app_skel_dir" "$mountpoint"

# unmount it
hdiutil detach "$device"

# clean up the duplicate binary files
rm "$binary_dir"/*

# convert to read-only
tempimage="/tmp/temp.dmg"
hdiutil convert "$dmg" -format UDZO -o "$tempimage"
mv "$tempimage" "$dmg"
final_size_kb=$(du -k "$dmg" | cut -f 1)
echo "Finished, final image size $final_size_kb""KB"

#!/bin/bash
# To automatically call this script, add the following to .git/config:
# [merge "highest_version_merge"]
# 	name = Keep the highest of two version.h files
# 	driver = resources/merge_highest_version.sh %O %A %B %P

ancestor="$1"
current="$2"
other="$3"
target="$4"

function get_revision {
  fgrep REVISION "$1" | cut -d '=' -f 2- | cut -d ';' -f 1 | tr -d ' '
}

revision_ancestor=$(get_revision "$ancestor")
revision_current=$(get_revision "$current")
revision_other=$(get_revision "$other")
#revision_target=$(get_revision "$target")
#echo "Ancestor is revision $revision_ancestor"
#echo "Current is revision $revision_current"
#echo "Other is revision $revision_other"
#echo "Target is revision $revision_target"

if [ "$revision_ancestor" -gt "$revision_current" ]; then
  if [ "$revision_ancestor" -gt "$revision_other" ]; then
    # ancestor has the highest revision, warn about this
    echo "Version control: ancestor's version was higher than either branch, $revision_other in $target - you may need to reload the project!"
    cat "$ancestor" > "$current"
  else
    # other branch has the highest revision
    echo "Version control: automatically merging other branch with highest version $revision_other in $target"
    cat "$other" > "$current"
  fi
else
  if [ "$revision_current" -gt "$revision_other" ]; then
    # current is the highest revision
    echo "Version control: current version $revision_current is highest, automatically keeping $target through merge"
  else
    # other branch has the highest revision
    echo "Version control: automatically merging other branch with highest version $revision_other in $target"
    cat "$other" > "$current"
  fi
fi

exit 0

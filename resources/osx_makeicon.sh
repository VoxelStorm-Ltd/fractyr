#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 appname" >&2
  exit 1
fi

iconutil -c icns "$1.iconset" && echo "Successfully generated $1.icns"

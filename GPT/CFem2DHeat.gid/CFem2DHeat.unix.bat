#!/bin/bash -f
rm -f "$2/$1.err"
rm -f "$2/$1.log"
rm -f "$2/$1.post.res"
"$3/CFem2DHeat" "$2/$1" -vvv > "$2/$1.log"

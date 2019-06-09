#!bin/bash/


find . -iname "*$1" -type f -printf '%p %s\n' | sort -k 2 -n -r | head -n $2| awk '{ x += $2 ; print } END { print "Total size: " x}'

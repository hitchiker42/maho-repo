#!/bin/bash
scilisp = ./src/Scilisp
#sanity test

$scilisp

if [ $? -ne 0 ]; then
    exit 1;
fi

exit 0

# Local Variables:
# mode: sh
# End:
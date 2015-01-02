#! /bin/sh

set -e

exec >gen-ft2.log
exec 2>&1

echo "Generating configuration for FreeType 2" >&2
cd freetype2
./autogen.sh
rm -f config.mk builds/unix/unix-def.mk builds/unix/unix-cc.mk
cd ..

echo "Done" >&2

cd ports/lua

cp Makefile src/

cd ../..
./manifest_parser ports/lua/lua.manifest ports/lua/lua.manifest.h
cd ports/lua

cd src
make clean
make lua
cd ..

cp src/lua ../../bin/Programs/Lua/Lua.esx

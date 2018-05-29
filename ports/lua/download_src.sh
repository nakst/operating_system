if [ ! -d "ports/lua/src" ]; then
	echo "    Downloading source for Lua..."
	curl https://www.lua.org/ftp/lua-5.3.4.tar.gz > lua-5.3.4.tar.gz
	gunzip lua-5.3.4.tar.gz
	tar -xf lua-5.3.4.tar
	cp -r lua-5.3.4/src/ ports/lua
	rm -rf lua-5.3.4 lua-5.3.4.tar
fi


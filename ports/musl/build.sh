cd ports/musl
make lib/libc.a
mkdir -p "../../bin/Programs/C Standard Library/Headers"

cp lib/libc.a "../../bin/Programs/C Standard Library/libc.a"
cp --recursive obj/include/* "../../bin/Programs/C Standard Library/Headers/"
cp --recursive arch/x86_64/* "../../bin/Programs/C Standard Library/Headers/"
cp --recursive arch/generic/* "../../bin/Programs/C Standard Library/Headers/"
cp --recursive include/* "../../bin/Programs/C Standard Library/Headers/"

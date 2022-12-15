if [ ! -d build/linux ];then
  mkdir -p build/linux
fi
cd build/linux

# download luajit source code
if [ ! -d "LuaJIT-2.1.0-beta3" ];then
  wget https://luajit.org/download/LuaJIT-2.1.0-beta3.tar.gz
  tar -xvf LuaJIT-2.1.0-beta3.tar.gz
fi

# build luajit
cd LuaJIT-2.1.0-beta3/src
make
cd ../..
cp -f LuaJIT-2.1.0-beta3/src/libluajit.so LuaJIT-2.1.0-beta3/src/libluajit-5.1.so.2

# make
cmake -S../.. -B. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DFLOWCHART_DEBUG=ON \
  -DBUILD_LUAJIT=ON -DLUAJIT_INCLUDE_PATH=./LuaJIT-2.1.0-beta3/src -DLUAJIT_LIB=./LuaJIT-2.1.0-beta3/src/libluajit.so \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 
cmake --build . -- -j 4

cd ..

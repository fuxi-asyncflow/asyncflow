if [ ! -d linux-build ];then
  mkdir -p linux-build
fi
cd linux-build

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
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_PYTHON=ON -DBUILD_LUAJIT=ON -DLUAJIT_INCLUDE_PATH=./LuaJIT-2.1.0-beta3/src -DLUAJIT_LIB=./LuaJIT-2.1.0-beta3/src/libluajit.so -DBUILD_TEST=OFF \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DFLOWCHART_DEBUG=ON
make -j8

# run test and generate coverage report
./lua_test
lcov --capture --directory . --output-file coverage0.info
lcov --remove coverage0.info "*thirdparty/*" "/usr/include/*" "/usr/lib/*" --output-file coverage.info
genhtml coverage.info --output-directory coverage
cd ..

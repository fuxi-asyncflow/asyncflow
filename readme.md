![example workflow](https://github.com/fuxi-asyncflow/asyncflow/actions/workflows/cmake.yml/badge.svg?branch=main)

# 编译

## 环境要求

+ 需要使用 cmake (version > 3.12) 
+ c++编译器支持`c++11`
+ 如需生成 code coverage 报告，则需要安装 lcov
+ 编译 python 版本需安装 python3

## 编译选项

项目支持的 cmake 编译选项如下：

| 编译选项                | 默认值 | 说明                                                          |
| ------------------- | --- | ----------------------------------------------------------- |
| FLOWCHART_DEBUG     | OFF | 开启远程调试功能，外放版本请不要打开                                          |
| BUILD_PYTHON        | OFF | 编译 python 模块                                                |
| BUILD_LUAJIT        | OFF | 编译 luajit 模块，为ON时，需要配置 `LUAJIT_INCLUDE_PATH` 与 `LUAJIT_LIB` |
| LUAJIT_INCLUDE_PATH | -   | luajit 的头文件目录                                               |
| LUAJIT_LIB          | -   | luajit 的库文件                                                 |
| BUILD_WASM          | OFF | 编译为 WASM  模块                                                |

`FLOWCHART_DEBUG` 为`ON`时编译带有调试功能的版本

### 示例

windows 上编译 python 模块：

```shell
cmake -G "Visual Studio 15 Win64" .. -DBUILD_PYTHON=ON -DFLOWCHART_DEBUG=ON -DCMAKE_BUILD_TYPE=Release
```

windows 上编译 luajit  与 python 模块：

```shell
REM win_build.bat
cmake . -Bwin-build -G "Visual Studio 16 2019" -Ax64 ^
    -DBUILD_LUAJIT=ON -DBUILD_PYTHON=ON -DBUILD_TEST=ON -DFLOWCHART_DEBUG=ON ^
    -DLUAJIT_INCLUDE_PATH=./thirdparty/LuaJIT-2.1.0-beta3/src ^
    -DLUAJIT_LIB=./thirdparty/LuaJIT-2.1.0-beta3/src/lua51.lib
```

linux 上编译 lua 模块可以直接使用 `build_lua.sh`

linux 上编译 wasm 模块，需提前准备好 `emscripten`：

```shell
$ emcmake cmake .. -DBUILD_WASM=ON -DCMAKE_BUILD_TYPE=Debug
```

## 编译的常见问题

### Visual Studio项目，Debug配置下链接错误

由于CMake对于Visual Studio项目的Debug配置中，一些选项参数没有与Release进行区分导致的。

+ 预定义宏里面加上`_DEBUG `， 这是由于Python的`pyconfig.h`里面:
  
  ```c++
  #            if defined(_DEBUG)
  #                pragma comment(lib,"python36_d.lib")
  #            elif defined(Py_LIMITED_API)
  #                pragma comment(lib,"python3.lib")
  #            else
  #                pragma comment(lib,"python36.lib")
  #            endif /* _DEBUG */
  ```

+ c/c++ => 代码生成 => 运行库 修改为 `/MDd`

### Fatal Python error: *PyThreadState_Get*: no current thread

编译出来的debug版本的dll，直接使用python.exe加载会出现该错误。具体解决办法为：

+ 将 xxx.pyd 改名为 xxx_d.pyd
+ 使用python_d.exe来加载该C扩展

### linux 链接 luajit 时提示使用 -fPIC

luajit 的默认编译方式为 `mixed` 会生成一个静态链接的 `luajit`，在 `luajit` 的 `makefile` 里面修改为 `BUILDMODE= dynamic` 即可

# 使用

可以参考 `test`文件夹下`lua`和`python`文件夹中`test_`开头的lua或python文件。

所有函数都位于 `asyncflow` module下，接口参见[lua版本导出接口](./doc/lua_api.md)


# 论文介绍

对于 asyncflow 的结构可以参见 [Asyncflow: A visual programming tool for game artificial intelligence - ScienceDirect](https://www.sciencedirect.com/science/article/pii/S2468502X21000498)

引用信息见仓库里面的 [asyncflow.bib](./asyncflow.bib) 文件



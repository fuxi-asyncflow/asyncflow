version 2021-10-08

# asyncflow runtime的接入

asynclfow 的 runtime 以 c++ 模块的方式导入到 lua 或 python环境中，典型的用法如下，具体使用可参考测试用例。

```lua
local asyncflow = require("asyncflow")
-- 初始化
asyncflow.setup()
asyncflow.import_charts("../flowchart/generate/Flowchart_AI.json")
asyncflow.import_charts("../flowchart/generate/Flowchart_Subchart.json")
asyncflow.import_event("../flowchart/generate/event_info.json")

-- 为Character对象挂载 chart_name 流程图
asyncflow.register(Character)
asyncflow.attach(Character,chart_name)
asyncflow.start(Character)

-- 在游戏逻辑循环中调用 step 函数
while (true) do
  asyncflow.step(100)
end

```



# 全局接口简介

全局接口是 asyncflow 提供的最基础，也是最核心的接口。

| 接口                                | 说明                                               |
| ----------------------------------- | ------------------------------------------------------ |
| setup             | 初始化asyncflow运行环境 |
| config_log | 设置log相关参数 |
| import_charts | 加载流程图数据 |
| import_event | 加载流程图事件数据 |
| exit | 销毁asyncflow运行环境 |
| register | 注册游戏对象 |
| deregister | 移除游戏对象，与register相对应           |
| attach | 挂载流程图到游戏对象                                     |
| remove | 移除游戏对象上的流程图                                   |
| start | 启动流程图运行 |
| stop | 停止流程图运行 |
| step | 推进流程图运行 |
| event | 抛出流程图事件 |

# 接口详细说明
## 初始化函数

### setup -- 初始化asyncflow运行环境

该函数用于初始化流程图运行环境，创建并返回一个 Manager 对象，一个 Manager 对象相当于一个 asyncflow 的运行环境。通常情况下，进程中只存在一个 manager 对象时，无需保存和使用返回值。

```lua
cfg = {loop_check = true, immediate_subchart = true, default_timestep = 10, debug_port = 8080, debug_name_function ="get_name"} -- lua
mgr = asyncflow.setup(cfg)
```
参数 `cfg` 是一个字典类型，其中可以配置的参数如下：

+ `loop_check` 流程图中碰到环时的处理，为 `true` 时成环的节点会延迟一个对象的 `Tick` 再执行, 默认为true。
+ `immediate_subchart` 在一个流程图中，调用另外一个流程图时，是否延迟到下一帧再执行，默认为false。
+ `default_timestep` step的默认时间间隔，当step没有参数，或是参数无效时使用，默认为100。
+ `debug_port` 调试功能占用的开始端口号，asyncflow会从该端口号开始寻找可用的端口进行使用，默认为9000。
+ `debug_name_function` 调试时获取对象名称的接口函数名称，默认调用的语言自带的转换为字符串函数。

即使cfg中有不合法参数，函数能忽略不合法的输入，均能执行成功，返回新生成的manager对象

### config_log -- 配置log相关参数

该函数用于设置流程图运行时输出的log配置，不配置的情况下log会打印到标准输出。

`filename` log配置文件的路径，文件格式参照[spdlog_setup](https://github.com/guangie88/spdlog_setup)
`loggername` log配置文件中，logger的name

```lua
asyncflow.config_log(filename, loggername)
```

### import_charts -- 加载流程图数据
```lua
-- 全局接口调用
count = asyncflow.import_charts("Gameplay.json")
count = asyncflow.import_charts(json_str)
-- Manager 接口调用
mgr:import_charts("Gameplay.json")
mgr:import_charts(json_str)
```
加载指定的流程图文件，也就是流程图编辑工具生成的`json` 文件

可以通过输入流程图文件的字符串直接加载，或者输入文件路径进行加载，绝对路径和相对路径均可

当线上环境，需要对流程图进行 `patch`时，也使用该接口来更新流程图信息。当一个流程图被再次 `import`时（path字段名称相同的图被认为是同一张流程图），此前已经 `attach`的流程图数据并不会变化，只有在import 之后 `attach` 的流程图才会使用再次  import 时的数据，即 "旧图旧数据，新图新数据"。

函数返回导入的流程图图信息的个数，当前没有已创建的manager对象（未调用setup函数）和输入参数不为字符串类型，会抛出异常终止操作。

若函数的返回值为0，即没有流程图图信息加载成功，可能由于输入的路径不存在文件或输入的字符串不是合法的json字符串，或者图信息有缺失，详见报错信息

### import_event -- 加载 event 信息

加载流程图编辑工具生成的 `event_info.json` 文件，里面包含定义的流程图事件信息，运行时需要使用这些信息。通过输入文件路径进行加载，绝对路径和相对路径均可。

当前没有已创建的manager对象（未调用setup函数）和输入参数不为字符串类型，会抛出异常终止操作。

如果有多个 Manager 实例，它们会共用这一信息，因此只需要加载一次即可，并且以最后一次输入的事件信息为准。

```lua
count = asyncflow.import_event("event_info.json")
```
函数的返回值为加载的事件个数，由于生成的event_info.json的文件中一定存在流程图运行必需的start和tick事件，加载正确的情况下，函数的返回值大于等于2

### exit

终止 manager 的运行并进行销毁操作。调用之后，当前manager对象将完全从内存中清除。
```lua
asyncflow.exit()
```

## 管理游戏对象

### register -- 管理游戏对象 ( register )
游戏对象只有注册到 asyncflow 中之后，才能挂载流程图和运行。注册过程中，会在 asycnflow 中创建一个与之关联的 `Agent` 对象，可以看作是游戏对象在 asyncflow 环境中的代理。

```lua
agent = asyncflow.register(game_object)
agent = asyncflow.register(game_object, { tick = 1000 })
```
参数 `game_object` 为一个游戏对象，第二个参数为可选参数， 用于设置 对应`Agent`的配置信息，`Agent`  Tick 事件的触发频率，单位为毫秒，默认值为 1000

当前没有已创建的manager对象（未调用setup函数）和输入`game_object`的类型不为userdata或者table类型时，会抛出异常终止操作。

创建成功返回`game_obj`对应的可在lua中使用的agent对象，若对象已经在asyncflow中注册，创建失败返回nil。

### deregister -- 取消对游戏对象的管理
与 register 相反，用于解除 asyncflow 对于游戏对象的管理。移除之后，对象上的所有流程图都会停止运行。游戏对象对应的 `agent` 对象也将完全从内存中清除。
```lua
asyncflow.deregister(game_object)
```

当前没有已创建的manager对象（未调用setup函数）和输入`game_object`的类型不为userdata或者table类型时，会抛出异常终止操作。

若输入的`game_object`未在asyncflow中注册过则返回false，否则返回true;

## 流程图的运行

### attach - 挂载流程图

+ 通过 `attach` 接口，可以挂载流程图到游戏对象上
+ 第三个参数用于设置流程图启动时的参数列表
+ 重复多次 attach 同一个流程图，只有第一次会生效，后面挂载失败

```lua
asyncflow.attach(game_object, "AI.chart")
asyncflow.attach(game_object, "AI.chart", {arg1 = "hello", arg2 = 2})
```

当前没有已创建的manager对象（未调用setup函数）和输入game_object的类型不为userdata或者table类型时，第二个参数类型不为字符串时，会抛出异常终止操作。

根据参数列表的按照变量名称，读入其对应的值，挂载成功返回挂载的chart,可调用set_callback函数设置图运行结束的回调函数，`chart:set_callback(callback_func)`。可能由于对象未注册在asyncflow上或者当前图已经被加载在此对象上了，挂载失败从而返回nil。

### remove - 停止并移除对象上的流程图

```lua
asyncflow.remove(game_object, "AI.chart")
```

当前没有已创建的manager对象（未调用setup函数）和输入game_object的类型不为userdata或者table类型时，第二个参数类型不为字符串时，会抛出异常终止操作。

解除对象上的流程图成功返回true，若对象未在asyncflow中注册或者希望移除的图并未挂载在此对象上返回false

### start - 启动对象上的流程图

start 支持运行所有处于非运行状态的流程图，和指定流程图，所有处于非运行状态的流程图包括

1. 新attach的流程图
2. 已经运行完毕，没有节点在运行的流程图
3. 通过 stop 接口停掉的流程图

start 特定流程图时，需保证该图已经 attach 到对象上。

```lua
count = asyncflow.start(game_object) -- 启动 obj 上所有处于非运行状态的chart
count = asyncflow.start(game_object, {"AI.chart1", "AI.chart2"}) -- 启动特定的 chart，注意如果chart没有事先attach，那么并不会运行
```

asyncflow.start(game_object)返回函数调用后当前正在运行的图的总量（也是挂载在此对象上图的总量）。asyncflow.start(game_object, {"AI.chart1", "AI.chart2"})返回希望加载的图列表中，函数调用后正在运行图的总量。

### stop - 停止对象上的流程图

需要注意 `stop` 与 `remove` 的区别。`stop` 之后，可以通过 `start` 接口来重新运行该流程图，而`remove` 之后，不能直接通过 `start`来运行，需要再次 `attach`

```lua
count = asyncflow.stop(game_object) -- 停止 game_object 上所有 chart
count = asyncflow.stop(game_object, {"AI.chart1", "AI.chart2"}) -- 停止 game_object 上指定的 chart
```

asyncflow.stop(game_object)返回函数调用后当前已经停止的图的总量（也是挂载在此对象上图的总量）。

asyncflow.stop(game_object, {"AI.chart1", "AI.chart2"})返回希望加载的图列表中，函数调用后正在运行图的总量。

+ stop 之后再次start，初始变量值为attach时的值
+ remove,stop,start 均只对通过attach接口直接挂载的流程图生效，对流程图节点中通过子图的方式启动的子流程图无效

### 总结

通过以上接口，可以实现各种场景下控制流程图运行的需求：

1. 对象初始化时，加载流程图，通过start控制运行，对象销毁时，停止所有的流程图运行
   1. `register`注册对象
   2. `attach` 对应的流程图
   3. `start` 希望执行的流程图
   4. 对象准备销毁时调用`destroy` 接口
   
2. 对象的行为有多个流程图控制，可以根据时机选择其中一个流程图运行以控制对象的行为：
   1. 初始时，`attach` 所有的流程图
   2. 根据条件，`stop` 旧流程图，并 `start` 新流程图

以上接口，都可以在流程图节点中使用，也就是 `step` 函数内部调用（需要保证stop及remove的内部调用不发生在节点所在图及所在图的父节点上）。但是需要注意的是，流程图的开始运行，是`step`函数调用时才会开始运行。

asyncflow 并不提供暂停和恢复流程图运行的功能。主要原因是流程图是由事件驱动的，如流程图在暂停期间收到了等待的事件，恢复之后难以判断该事件是否有效。


## 驱动流程图运行

### step( delta_time )

所有流程图的运行过程，都发生在 `step` 函数内部。内部会将内置定时器推进 delta_time 毫秒，并处理从上次运行到现在所有产生的 event。

这一函数通常在程序的主循环中循环调用：

```python
last_time = 0
while True:
    delta_time = time.now() - last_time
    last_time = time.now()
    asyncflow.step(delta_time)    
```
需要注意的是：在 step 内部一开始，就会驱动内置定时器前进，随后才会处理 event。因此在此次 step 中所有节点的运行，都是发生在推进后的这一个时刻。如果简单的使用固定时间间隔来调用 step 函数时，容易忘记在第一次调用 step 时传入正确的参数。例如

```python
# 流程图为: start -> wait(0.1) -> print(time)
while True:
    asyncflow.step(50)	# 节点第一次运行发生在 50 ms
# 那么输出时间会是 0.15，因为 wait 是在 0.05 时刻运行的
```

### event
流程图里面的事件节点，需要等待事件产生后，才往下运行。事件的产生需要通过 `event` 函数来完成。
```python
asyncflow.event(game_object, EventId.See, args, ...)
```
第一个参数为事件相关的游戏对象，第二个参数为event的Id，后续参数为该事件的参数。


### get_current_manager
当进程中存在多个 `manager` 对象时，返回当前正在操作的 `manager` 对象。
```python
mgr = asyncflow.get_current_manager()
```

# manager类
如果游戏中需要使用多个 manager，例如不同的 manager 管理不同类型或是不同场景中的游戏对象与流程图。那么需要针对具体的 `manager` 对象来进行相应的操作，如果进程中只存在一个 `manager` 对象，使用上面的全局接口即可。

​      lua中成员函数的调用需要将当前对象作为参数传入，如register函数的调用需要写成mananger.register(manager, params ...), 使用语法糖":"可省略当前对象作为参数，写为manager:register(params...)  下列接口省略第一个参数即当前manager对象本身，agent类接口同理

| 接口                         | 参数说明                                                   | 返回值 | 接口说明                                                     |
| ---------------------------- | ---------------------------------------------------------- | ------ | ------------------------------------------------------------ |
| import_charts(path)        | path—string                                                | bool   | 载入图相关数据，若传入的path是有效的json字符串，则解析字符串，不是则载入path路径的文件。读取成功返回true失败返回false |
| import_event(path)           | path—string                                                | bool   | 载入事件信息，path表示载入路径。读取成功返回true失败返回false |
| register(obj)  | obj—待注册对象，table—{"tick"=20} | agent | 将obj对象注册到当前manager中，若无tick参数或tick不大于0，使用默认1000ms。 |
| event(obj,event_id,params...) | obj—已注册的obj对象 event_id—number params...—事件参数 | 无                    | 抛出一个obj对象的编号为event_id的事件，并传入参数params... |
| step(time)               | time—number                                             | 无     | 当前manager运行一帧，本帧的时长为time ms                  |
| deregister(obj)          | obj—已注册的obj对象                                        | bool   | 对当前manager解除注册的obj对象，并清理相关数据               |

+ 注意:

对于import_event接口，所有mananger的共享最后导入的事件信息。

#### agent类接口
agent类的接口，对挂载的流程图进行操作。agent 上与流程图相关的接口如下：


| 接口                           | 描述                                |
| ------------------------------ | ----------------------------------- |
| attach_chart(chart_name,table) | 挂载流程图，table中为参数的相关信息 |
| remove_chart(chart_name)       | 移除流程图                          |
| get_charts(is_subchart)        | 获取对象上挂载的流程图列表          |








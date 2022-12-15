-- Say("hello")
local function f_9dfc8ea58a4743db95183f7d9bfb04ec(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("AI.test_01.9dfc8ea58a4743db95183f7d9bfb04ec", f_9dfc8ea58a4743db95183f7d9bfb04ec)

-- Say(time)
local function f_cc77a904008c43719a7e505910cdc10f(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_02.cc77a904008c43719a7e505910cdc10f", f_cc77a904008c43719a7e505910cdc10f)

-- wait(1)
local function f_eb7813885a224228a7747366d8618dbf(self)
    asyncflow.wait(1)
    return true
end
asyncflow.set_node_func("AI.test_02.eb7813885a224228a7747366d8618dbf", f_eb7813885a224228a7747366d8618dbf)

-- Say(time)
local function f_98c9fbfeaef0425885d98de2c5daf952(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_02.98c9fbfeaef0425885d98de2c5daf952", f_98c9fbfeaef0425885d98de2c5daf952)

-- $str = "hello"
local function f_b5d9a1fa89fb471e8aa16acc1913e9a3(self)
    local __ret__ = asyncflow.set_var("str", "hello")
    return __ret__
end
asyncflow.set_node_func("AI.test_03.b5d9a1fa89fb471e8aa16acc1913e9a3", f_b5d9a1fa89fb471e8aa16acc1913e9a3)

-- Say($str)
local function f_4e9524324662485d8149d455b2894734(self)
    self:Say(asyncflow.get_var("str"))
    return true
end
asyncflow.set_node_func("AI.test_03.4e9524324662485d8149d455b2894734", f_4e9524324662485d8149d455b2894734)

-- Say("hello")
local function f_47ecef471c53408aa0bf0794ce312e04(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("AI.test_04.47ecef471c53408aa0bf0794ce312e04", f_47ecef471c53408aa0bf0794ce312e04)

-- Say("world")
local function f_912f034c3c29442aa9d1f70f67c727a0(self)
    self:Say("world")
    return true
end
asyncflow.set_node_func("AI.test_04.912f034c3c29442aa9d1f70f67c727a0", f_912f034c3c29442aa9d1f70f67c727a0)

-- Say(time)
local function f_5b3bb757d1724fc3a795f53db0723816(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_04.5b3bb757d1724fc3a795f53db0723816", f_5b3bb757d1724fc3a795f53db0723816)

-- OnEvent0Arg
local function f_f93329c774ce439bb8d4b81bf5d151ce(self)
    local __ret__ = asyncflow.wait_event(self, asyncflow.EventId.Event0Arg)
    return __ret__
end
asyncflow.set_node_func("AI.test_05.f93329c774ce439bb8d4b81bf5d151ce", f_f93329c774ce439bb8d4b81bf5d151ce)

-- Say("event 0 pass")
local function f_59fe1a5da28a48e0a48627d63ba50809(self)
    self:Say("event 0 pass")
    return true
end
asyncflow.set_node_func("AI.test_05.59fe1a5da28a48e0a48627d63ba50809", f_59fe1a5da28a48e0a48627d63ba50809)

-- OnEvent1Arg
local function f_9983834617ab4979acc4a56125a2d16f(self)
    local __ret__ = asyncflow.wait_event(self, asyncflow.EventId.Event1Arg)
    return __ret__
end
asyncflow.set_node_func("AI.test_05.9983834617ab4979acc4a56125a2d16f", f_9983834617ab4979acc4a56125a2d16f)

-- Say("event 1 pass" + Event1Arg.param)
local function f_a1ac7944f25842df8fedb0f6deba7a4d(self)
    self:Say("event 1 pass" .. asyncflow.get_event_param(asyncflow.EventId.Event1Arg, 0))
    return true
end
asyncflow.set_node_func("AI.test_05.a1ac7944f25842df8fedb0f6deba7a4d", f_a1ac7944f25842df8fedb0f6deba7a4d)

-- OnEvent2Arg
local function f_6d64fde9049743f393ebb811b69aeb06(self)
    local __ret__ = asyncflow.wait_event(self, asyncflow.EventId.Event2Arg)
    return __ret__
end
asyncflow.set_node_func("AI.test_05.6d64fde9049743f393ebb811b69aeb06", f_6d64fde9049743f393ebb811b69aeb06)

-- Say("event 2 pass")
local function f_92232347f4064253b2c9ad3b0e9293b1(self)
    self:Say("event 2 pass")
    return true
end
asyncflow.set_node_func("AI.test_05.92232347f4064253b2c9ad3b0e9293b1", f_92232347f4064253b2c9ad3b0e9293b1)

-- Say("1st arg" + Event2Arg.param1)
local function f_8a1c7ed3160c4d0eb5a6257f3ef34614(self)
    self:Say("1st arg" .. asyncflow.get_event_param(asyncflow.EventId.Event2Arg, 0))
    return true
end
asyncflow.set_node_func("AI.test_05.8a1c7ed3160c4d0eb5a6257f3ef34614", f_8a1c7ed3160c4d0eb5a6257f3ef34614)

-- Say("2nd arg" + Event2Arg.param2)
local function f_a0e5dc6615ea4cb3a3c2c229b598049c(self)
    self:Say("2nd arg" .. asyncflow.get_event_param(asyncflow.EventId.Event2Arg, 1))
    return true
end
asyncflow.set_node_func("AI.test_05.a0e5dc6615ea4cb3a3c2c229b598049c", f_a0e5dc6615ea4cb3a3c2c229b598049c)

-- test_06_sub()
local function f_12080d88c85f406bb2c5d63266c2228a(self)
    local __ret__ = asyncflow.call_sub("AI.test_06_sub", self)
    return __ret__ ~= 0
end
asyncflow.set_node_func("AI.test_06.12080d88c85f406bb2c5d63266c2228a", f_12080d88c85f406bb2c5d63266c2228a)

-- Say("subunit pass")
local function f_bbd7cefd50b942728807fc2831476e07(self)
    self:Say("subunit pass")
    return true
end
asyncflow.set_node_func("AI.test_06.bbd7cefd50b942728807fc2831476e07", f_bbd7cefd50b942728807fc2831476e07)

-- return(1)
local function f_52d53ccc09ac4beda69f9ef0a8663ef5(self)
    asyncflow.ret(1)
    return true
end
asyncflow.set_node_func("AI.test_06_sub.52d53ccc09ac4beda69f9ef0a8663ef5", f_52d53ccc09ac4beda69f9ef0a8663ef5)

-- test_07_sub(1,2)
local function f_9169be5d222349db89e0de785c0d6a74(self)
    local __ret__ = asyncflow.call_sub("AI.test_07_sub", self, 1, 2)
    return __ret__ ~= 0
end
asyncflow.set_node_func("AI.test_07.9169be5d222349db89e0de785c0d6a74", f_9169be5d222349db89e0de785c0d6a74)

-- $a = test_07_sub(3, 4)
local function f_edf49f0e00e149ddb9bb4f7f2f7b38b2(self)
    local __ret__ = asyncflow.call_sub("AI.test_07_sub", self, 3, 4)
    return __ret__ ~= 0
end
asyncflow.set_node_func("AI.test_07.edf49f0e00e149ddb9bb4f7f2f7b38b2", f_edf49f0e00e149ddb9bb4f7f2f7b38b2)

-- Say($a)
local function f_ae372d08c8a744e4ae0ab9ebfa73883a(self)
    self:Say(asyncflow.get_var("a"))
    return true
end
asyncflow.set_node_func("AI.test_07.ae372d08c8a744e4ae0ab9ebfa73883a", f_ae372d08c8a744e4ae0ab9ebfa73883a)

-- Say($x)
local function f_fac314b909574f9fb386a9a4cbbcf897(self)
    self:Say(asyncflow.get_var("x"))
    return true
end
asyncflow.set_node_func("AI.test_07_sub.fac314b909574f9fb386a9a4cbbcf897", f_fac314b909574f9fb386a9a4cbbcf897)

-- Say($y)
local function f_d3ca5de79331406cb6468bcf80796782(self)
    self:Say(asyncflow.get_var("y"))
    return true
end
asyncflow.set_node_func("AI.test_07_sub.d3ca5de79331406cb6468bcf80796782", f_d3ca5de79331406cb6468bcf80796782)

-- $i = $x + $y
local function f_50cd0849040c491ba1bac88436c7fbfd(self)
    local __ret__ = asyncflow.set_var("i", asyncflow.get_var("x") + asyncflow.get_var("y"))
    return __ret__ ~= 0
end
asyncflow.set_node_func("AI.test_07_sub.50cd0849040c491ba1bac88436c7fbfd", f_50cd0849040c491ba1bac88436c7fbfd)

-- Say($i)
local function f_8612fd56143d4c4191826f9c67a97d4b(self)
    self:Say(asyncflow.get_var("i"))
    return true
end
asyncflow.set_node_func("AI.test_07_sub.8612fd56143d4c4191826f9c67a97d4b", f_8612fd56143d4c4191826f9c67a97d4b)

-- return($i)
local function f_84fc5277ee074792b045e255001eb5a5(self)
    asyncflow.ret(asyncflow.get_var("i"))
    return true
end
asyncflow.set_node_func("AI.test_07_sub.84fc5277ee074792b045e255001eb5a5", f_84fc5277ee074792b045e255001eb5a5)

-- $c = AsyncAdd(1,2)
local function f_c87734cba7c048d6b8411e99677313c4(self)
    local __ret__ = asyncflow.set_var("c", self:AsyncAdd(1, 2))
    return __ret__
end
asyncflow.set_node_func("AI.test_08.c87734cba7c048d6b8411e99677313c4", f_c87734cba7c048d6b8411e99677313c4)

-- Say($c)
local function f_d0faf6e42a4348a79d306a0b079ab730(self)
    self:Say(asyncflow.get_var("c"))
    return true
end
asyncflow.set_node_func("AI.test_08.d0faf6e42a4348a79d306a0b079ab730", f_d0faf6e42a4348a79d306a0b079ab730)

-- $d = AsyncAdd(3, 4)
local function f_a3e8ca5860a243eca9d0b0a783f9ecef(self)
    local __ret__ = asyncflow.set_var("d", self:AsyncAdd(3, 4))
    return __ret__
end
asyncflow.set_node_func("AI.test_08.a3e8ca5860a243eca9d0b0a783f9ecef", f_a3e8ca5860a243eca9d0b0a783f9ecef)

-- Say($d)
local function f_86be085d3008499ea34ee8430d59cca5(self)
    self:Say(asyncflow.get_var("d"))
    return true
end
asyncflow.set_node_func("AI.test_08.86be085d3008499ea34ee8430d59cca5", f_86be085d3008499ea34ee8430d59cca5)

-- wait(1)
local function f_c8d16b1c904c4bf798a6ec8bbf703f37(self)
    asyncflow.wait(1)
    return true
end
asyncflow.set_node_func("AI.test_09.c8d16b1c904c4bf798a6ec8bbf703f37", f_c8d16b1c904c4bf798a6ec8bbf703f37)

-- Say("node 1")
local function f_cc6fe6d717f74f61a3b70652c293020a(self)
    self:Say("node 1")
    return true
end
asyncflow.set_node_func("AI.test_09.cc6fe6d717f74f61a3b70652c293020a", f_cc6fe6d717f74f61a3b70652c293020a)

-- Say("all node runned")
local function f_7825f180d385472481a7f64c1d791554(self)
    self:Say("all node runned")
    return true
end
asyncflow.set_node_func("AI.test_09.7825f180d385472481a7f64c1d791554", f_7825f180d385472481a7f64c1d791554)

-- wait(2)
local function f_e8c4dbecdd694ea980121f04bae16444(self)
    asyncflow.wait(2)
    return true
end
asyncflow.set_node_func("AI.test_09.e8c4dbecdd694ea980121f04bae16444", f_e8c4dbecdd694ea980121f04bae16444)

-- Say("node 2")
local function f_9b64a62b1cf94374a293e329fe66d9a8(self)
    self:Say("node 2")
    return true
end
asyncflow.set_node_func("AI.test_09.9b64a62b1cf94374a293e329fe66d9a8", f_9b64a62b1cf94374a293e329fe66d9a8)

-- wait(3)
local function f_cad031eba4e34290ba788e14a3b11ba3(self)
    asyncflow.wait(3)
    return true
end
asyncflow.set_node_func("AI.test_09.cad031eba4e34290ba788e14a3b11ba3", f_cad031eba4e34290ba788e14a3b11ba3)

-- Say("node 3")
local function f_d4ef42ee02024c77bd6f2ab10081690e(self)
    self:Say("node 3")
    return true
end
asyncflow.set_node_func("AI.test_09.d4ef42ee02024c77bd6f2ab10081690e", f_d4ef42ee02024c77bd6f2ab10081690e)

-- $s=CreateCharacter()
local function f_e886cb0441184affaeaa17b79e10426d(self)
    local __ret__ = asyncflow.set_var("s", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("AI.test_10.e886cb0441184affaeaa17b79e10426d", f_e886cb0441184affaeaa17b79e10426d)

-- $s.OnEvent0Arg
local function f_a7c4d9fba6d546249ff8efe98e31dc44(self)
    asyncflow.wait_event(asyncflow.get_var("s"), asyncflow.EventId.Event0Arg)
    return true
end
asyncflow.set_node_func("AI.test_10.a7c4d9fba6d546249ff8efe98e31dc44", f_a7c4d9fba6d546249ff8efe98e31dc44)

-- Say("event 0 pass")
local function f_2a7e4570f894433e9e25d97a40f5785c(self)
    self:Say("event 0 pass")
    return true
end
asyncflow.set_node_func("AI.test_10.2a7e4570f894433e9e25d97a40f5785c", f_2a7e4570f894433e9e25d97a40f5785c)

-- $s.OnEvent2Arg
local function f_0e216dd981de4983a93a08b1acb7f9b8(self)
    asyncflow.wait_event(asyncflow.get_var("s"), asyncflow.EventId.Event2Arg)
    return true
end
asyncflow.set_node_func("AI.test_10.0e216dd981de4983a93a08b1acb7f9b8", f_0e216dd981de4983a93a08b1acb7f9b8)

-- Say("event 2 pass")
local function f_d278c17dc6684644b403e02b710f3b77(self)
    self:Say("event 2 pass")
    return true
end
asyncflow.set_node_func("AI.test_10.d278c17dc6684644b403e02b710f3b77", f_d278c17dc6684644b403e02b710f3b77)

-- Say("1st arg" +Event2Arg.param1)
local function f_42aa83bff2b148cb93f40f306260de8b(self)
    self:Say("1st arg" .. asyncflow.get_event_param(asyncflow.EventId.Event2Arg, 0))
    return true
end
asyncflow.set_node_func("AI.test_10.42aa83bff2b148cb93f40f306260de8b", f_42aa83bff2b148cb93f40f306260de8b)

-- Say("2nd arg" + Event2Arg.param2)
local function f_eacbd156f3ef4739ad4f7d8666e25061(self)
    self:Say("2nd arg" .. asyncflow.get_event_param(asyncflow.EventId.Event2Arg, 1))
    return true
end
asyncflow.set_node_func("AI.test_10.eacbd156f3ef4739ad4f7d8666e25061", f_eacbd156f3ef4739ad4f7d8666e25061)

-- $s.OnEvent1Arg
local function f_b93968aec81f4d4da508c8223064ef4a(self)
    asyncflow.wait_event(asyncflow.get_var("s"), asyncflow.EventId.Event1Arg)
    return true
end
asyncflow.set_node_func("AI.test_10.b93968aec81f4d4da508c8223064ef4a", f_b93968aec81f4d4da508c8223064ef4a)

-- Say("event 1 pass" + Event1Arg.param)
local function f_86dfd1f0ac4e41639589da2ea0ef0016(self)
    self:Say("event 1 pass" .. asyncflow.get_event_param(asyncflow.EventId.Event1Arg, 0))
    return true
end
asyncflow.set_node_func("AI.test_10.86dfd1f0ac4e41639589da2ea0ef0016", f_86dfd1f0ac4e41639589da2ea0ef0016)

-- test_11_sub()
local function f_e2525217efbf4ebf99f3b86798386f34(self)
    asyncflow.call_sub("AI.test_11_sub", self)
    return true
end
asyncflow.set_node_func("AI.test_11.e2525217efbf4ebf99f3b86798386f34", f_e2525217efbf4ebf99f3b86798386f34)

-- Say("subunit pass")
local function f_26e9d8f7af1a4470a418c3f4aee4d292(self)
    self:Say("subunit pass")
    return true
end
asyncflow.set_node_func("AI.test_11.26e9d8f7af1a4470a418c3f4aee4d292", f_26e9d8f7af1a4470a418c3f4aee4d292)

-- Say("red")
local function f_e6e2831309464d679a474d01bf55842b(self)
    self:Say("red")
    return true
end
asyncflow.set_node_func("AI.test_11.e6e2831309464d679a474d01bf55842b", f_e6e2831309464d679a474d01bf55842b)

-- Say("green")
local function f_47d891df53ff42978d4c3f237d6f1678(self)
    self:Say("green")
    return true
end
asyncflow.set_node_func("AI.test_11.47d891df53ff42978d4c3f237d6f1678", f_47d891df53ff42978d4c3f237d6f1678)

-- Say("hello sub")
local function f_d7014a211f044117a75004b7c0c6e259(self)
    self:Say("hello sub")
    return true
end
asyncflow.set_node_func("AI.test_11_sub.d7014a211f044117a75004b7c0c6e259", f_d7014a211f044117a75004b7c0c6e259)

-- Say("1")
local function f_dcc94bd684af4256b35b1b6b569fd92c(self)
    self:Say("1")
    return true
end
asyncflow.set_node_func("AI.test_12.dcc94bd684af4256b35b1b6b569fd92c", f_dcc94bd684af4256b35b1b6b569fd92c)

-- Say("2")
local function f_9ebbe264cc964a4d95914440a062fa7f(self)
    self:Say("2")
    return true
end
asyncflow.set_node_func("AI.test_12.9ebbe264cc964a4d95914440a062fa7f", f_9ebbe264cc964a4d95914440a062fa7f)

-- OnEvent1Arg
local function f_b741aa2dae5f40f5834b6c2f31c2fca6(self)
    local __ret__ = asyncflow.wait_event(self, asyncflow.EventId.Event1Arg)
    return __ret__
end
asyncflow.set_node_func("AI.test_12.b741aa2dae5f40f5834b6c2f31c2fca6", f_b741aa2dae5f40f5834b6c2f31c2fca6)

-- Say("event 0 pass")
local function f_26ad3735d1b04e1ea1523c2184f7c103(self)
    self:Say("event 0 pass")
    return true
end
asyncflow.set_node_func("AI.test_12.26ad3735d1b04e1ea1523c2184f7c103", f_26ad3735d1b04e1ea1523c2184f7c103)

-- wait(1)
local function f_2a7e47308bd4416c8b4ae849709a3e61(self)
    asyncflow.wait(1)
    return true
end
asyncflow.set_node_func("AI.test_12.2a7e47308bd4416c8b4ae849709a3e61", f_2a7e47308bd4416c8b4ae849709a3e61)

-- Say("hello")
local function f_f8e95d4cf35c4c3ca373481f45562fed(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("AI.test_12.f8e95d4cf35c4c3ca373481f45562fed", f_f8e95d4cf35c4c3ca373481f45562fed)

-- Say("hello")
local function f_77c3eff07e174b4190ccc1bb215d834f(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("AI.test_13.77c3eff07e174b4190ccc1bb215d834f", f_77c3eff07e174b4190ccc1bb215d834f)

-- Say("1")
local function f_1136de93efa140d2bc91882393c2811b(self)
    self:Say("1")
    return true
end
asyncflow.set_node_func("AI.test_13.1136de93efa140d2bc91882393c2811b", f_1136de93efa140d2bc91882393c2811b)

-- Say("2")
local function f_8658e0ed635c4166b2f6da54657ef2d0(self)
    self:Say("2")
    return true
end
asyncflow.set_node_func("AI.test_13.8658e0ed635c4166b2f6da54657ef2d0", f_8658e0ed635c4166b2f6da54657ef2d0)

-- Say(1)
local function f_885dd3312aaa4823aaa2cfac700e4844(self)
    self:Say(1)
    return true
end
asyncflow.set_node_func("AI.test_14.885dd3312aaa4823aaa2cfac700e4844", f_885dd3312aaa4823aaa2cfac700e4844)

-- Say(2)
local function f_2f7be34327f24fa0b535097fe666cfbe(self)
    self:Say(2)
    return true
end
asyncflow.set_node_func("AI.test_14.2f7be34327f24fa0b535097fe666cfbe", f_2f7be34327f24fa0b535097fe666cfbe)

-- Say(3)
local function f_7fc1f00251a046af905e2ef32d9a469e(self)
    self:Say(3)
    return true
end
asyncflow.set_node_func("AI.test_14.7fc1f00251a046af905e2ef32d9a469e", f_7fc1f00251a046af905e2ef32d9a469e)

-- Say(4)
local function f_a2ff83bfb2df48fd97b3a296db445d0f(self)
    self:Say(4)
    return true
end
asyncflow.set_node_func("AI.test_14.a2ff83bfb2df48fd97b3a296db445d0f", f_a2ff83bfb2df48fd97b3a296db445d0f)

-- wait(1)
local function f_6755d438d5f844baba7de0bb822d80be(self)
    asyncflow.wait(1)
    return true
end
asyncflow.set_node_func("AI.test_15.6755d438d5f844baba7de0bb822d80be", f_6755d438d5f844baba7de0bb822d80be)

-- Say(time)
local function f_930cc0978159403aaa57db6d967df8a4(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_15.930cc0978159403aaa57db6d967df8a4", f_930cc0978159403aaa57db6d967df8a4)

-- wait(5)
local function f_ffbf71024ce145ae99a55e31d2b3c925(self)
    asyncflow.wait(5)
    return true
end
asyncflow.set_node_func("AI.test_15.ffbf71024ce145ae99a55e31d2b3c925", f_ffbf71024ce145ae99a55e31d2b3c925)

-- Say(time)
local function f_2d49ef38d1ca4024b1ee6ad1c9a954aa(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_15.2d49ef38d1ca4024b1ee6ad1c9a954aa", f_2d49ef38d1ca4024b1ee6ad1c9a954aa)

-- wait(2)
local function f_c2ef46ac033c49a4926b34b57460aea4(self)
    asyncflow.wait(2)
    return true
end
asyncflow.set_node_func("AI.test_15.c2ef46ac033c49a4926b34b57460aea4", f_c2ef46ac033c49a4926b34b57460aea4)

-- Say(time)
local function f_21e88f49ee08465185f28fbf755cbf6e(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_15.21e88f49ee08465185f28fbf755cbf6e", f_21e88f49ee08465185f28fbf755cbf6e)

-- wait(1)
local function f_47bb1b892f084f3faf2e379b737193de(self)
    asyncflow.wait(1)
    return true
end
asyncflow.set_node_func("AI.test_16.47bb1b892f084f3faf2e379b737193de", f_47bb1b892f084f3faf2e379b737193de)

-- Say(time)
local function f_83d7e4d0d23e479c861e45a18ecd2060(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_16.83d7e4d0d23e479c861e45a18ecd2060", f_83d7e4d0d23e479c861e45a18ecd2060)

-- OnEvent0Arg
local function f_a59747ae1ba64774b7b01e2a1367c21b(self)
    local __ret__ = asyncflow.wait_event(self, asyncflow.EventId.Event0Arg)
    return __ret__
end
asyncflow.set_node_func("AI.test_16.a59747ae1ba64774b7b01e2a1367c21b", f_a59747ae1ba64774b7b01e2a1367c21b)

-- Say(time)
local function f_e4f2099363374023941118f7e266ae92(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_16.e4f2099363374023941118f7e266ae92", f_e4f2099363374023941118f7e266ae92)

-- wait(2)
local function f_08af1e3195a6434498270071af0c8f0d(self)
    asyncflow.wait(2)
    return true
end
asyncflow.set_node_func("AI.test_16.08af1e3195a6434498270071af0c8f0d", f_08af1e3195a6434498270071af0c8f0d)

-- Say(time)
local function f_c21382fe4be74b5b90b98fa464955be3(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_16.c21382fe4be74b5b90b98fa464955be3", f_c21382fe4be74b5b90b98fa464955be3)

-- wait(1)
local function f_ade192269da34394a7eb87a47ea4c81e(self)
    asyncflow.wait(1)
    return true
end
asyncflow.set_node_func("AI.test_17.ade192269da34394a7eb87a47ea4c81e", f_ade192269da34394a7eb87a47ea4c81e)

-- Say(time)
local function f_3bae8e8241b548e9ad7e6d01e8707f7c(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_17.3bae8e8241b548e9ad7e6d01e8707f7c", f_3bae8e8241b548e9ad7e6d01e8707f7c)

-- test_17_sub()
local function f_e1330480adb54f65a73066597afd098e(self)
    asyncflow.call_sub("AI.test_17_sub", self)
    return true
end
asyncflow.set_node_func("AI.test_17.e1330480adb54f65a73066597afd098e", f_e1330480adb54f65a73066597afd098e)

-- Say(time)
local function f_30f45e8ceb1e46fd936f69159226b54d(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_17.30f45e8ceb1e46fd936f69159226b54d", f_30f45e8ceb1e46fd936f69159226b54d)

-- wait(2)
local function f_670513fc98454b4db29124bdbc4bf942(self)
    asyncflow.wait(2)
    return true
end
asyncflow.set_node_func("AI.test_17.670513fc98454b4db29124bdbc4bf942", f_670513fc98454b4db29124bdbc4bf942)

-- Say(time)
local function f_590f60f752f04a40a511de6bda60afdd(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_17.590f60f752f04a40a511de6bda60afdd", f_590f60f752f04a40a511de6bda60afdd)

-- Say(time)
local function f_924beff49dbb48d483747ac54782f6f0(self)
    self:Say(asyncflow.time())
    return true
end
asyncflow.set_node_func("AI.test_17_sub.924beff49dbb48d483747ac54782f6f0", f_924beff49dbb48d483747ac54782f6f0)

-- wait(3)
local function f_c5b7b977b0994bea8cfcf4bf57508de0(self)
    asyncflow.wait(3)
    return true
end
asyncflow.set_node_func("AI.test_17_sub.c5b7b977b0994bea8cfcf4bf57508de0", f_c5b7b977b0994bea8cfcf4bf57508de0)

-- $a = "hello"
local function f_2d914be5d7534adeaa52f51585d7a54e(self)
    local __ret__ = asyncflow.set_var("a", "hello")
    return __ret__
end
asyncflow.set_node_func("AI.test_18.2d914be5d7534adeaa52f51585d7a54e", f_2d914be5d7534adeaa52f51585d7a54e)

-- Say($a)
local function f_9517f2f34106421098628fff57ae5e3e(self)
    self:Say(asyncflow.get_var("a"))
    return true
end
asyncflow.set_node_func("AI.test_18.9517f2f34106421098628fff57ae5e3e", f_9517f2f34106421098628fff57ae5e3e)

-- $a="ss111"
local function f_2ba5507f48124a0ebc1ed621dc4930a7(self)
    local __ret__ = asyncflow.set_var("a", "ss111")
    return __ret__
end
asyncflow.set_node_func("AI.test_18.2ba5507f48124a0ebc1ed621dc4930a7", f_2ba5507f48124a0ebc1ed621dc4930a7)

-- OnEvent0Arg
local function f_365a6ef2dca547d6bbcb03bd7589c49b(self)
    local __ret__ = asyncflow.wait_event(self, asyncflow.EventId.Event0Arg)
    return __ret__
end
asyncflow.set_node_func("AI.test_18.365a6ef2dca547d6bbcb03bd7589c49b", f_365a6ef2dca547d6bbcb03bd7589c49b)

-- Say("event 0 pass")
local function f_ea0871440caa47b1bae4ec66b238e270(self)
    self:Say("event 0 pass")
    return true
end
asyncflow.set_node_func("AI.test_18.ea0871440caa47b1bae4ec66b238e270", f_ea0871440caa47b1bae4ec66b238e270)

-- OnEvent2Arg
local function f_6288a287371f4586a4cd7a6f274712dd(self)
    local __ret__ = asyncflow.wait_event(self, asyncflow.EventId.Event2Arg)
    return __ret__
end
asyncflow.set_node_func("AI.test_18.6288a287371f4586a4cd7a6f274712dd", f_6288a287371f4586a4cd7a6f274712dd)

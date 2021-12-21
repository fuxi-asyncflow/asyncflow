if asyncflow.node_funcs == nil then asyncflow.node_funcs = {} end
local EventId = asyncflow.EventId
function asyncflow.node_funcs.func_AI_test_01_id_9dfc8ea58a4743db95183f7d9bfb04ec(self)
    self:Say("hello")
    return true
end

function asyncflow.node_funcs.func_AI_test_02_id_cc77a904008c43719a7e505910cdc10f(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_02_id_eb7813885a224228a7747366d8618dbf(self)
    asyncflow.wait(1)
    return true
end

function asyncflow.node_funcs.func_AI_test_02_id_98c9fbfeaef0425885d98de2c5daf952(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_03_id_b5d9a1fa89fb471e8aa16acc1913e9a3(self)
    local ret = asyncflow.set_var(0, "hello")
    return true
end

function asyncflow.node_funcs.func_AI_test_03_id_4e9524324662485d8149d455b2894734(self)
    self:Say(asyncflow.get_var(0))
    return true
end

function asyncflow.node_funcs.func_AI_test_04_id_47ecef471c53408aa0bf0794ce312e04(self)
    self:Say("hello")
    return true
end

function asyncflow.node_funcs.func_AI_test_04_id_912f034c3c29442aa9d1f70f67c727a0(self)
    self:Say("world")
    return true
end

function asyncflow.node_funcs.func_AI_test_04_id_5b3bb757d1724fc3a795f53db0723816(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_05_id_f93329c774ce439bb8d4b81bf5d151ce(self)
    local ret = asyncflow.wait_event(self, EventId.Event0Arg)
    return true
end

function asyncflow.node_funcs.func_AI_test_05_id_59fe1a5da28a48e0a48627d63ba50809(self)
    self:Say("event 0 pass")
    return true
end

function asyncflow.node_funcs.func_AI_test_05_id_9983834617ab4979acc4a56125a2d16f(self)
    local ret = asyncflow.wait_event(self, EventId.Event1Arg)
    return true
end

function asyncflow.node_funcs.func_AI_test_05_id_a1ac7944f25842df8fedb0f6deba7a4d(self)
    self:Say("event 1 pass" .. asyncflow.get_event_param(EventId.Event1Arg, 0))
    return true
end

function asyncflow.node_funcs.func_AI_test_05_id_6d64fde9049743f393ebb811b69aeb06(self)
    local ret = asyncflow.wait_event(self, EventId.Event2Arg)
    return true
end

function asyncflow.node_funcs.func_AI_test_05_id_92232347f4064253b2c9ad3b0e9293b1(self)
    self:Say("event 2 pass")
    return true
end

function asyncflow.node_funcs.func_AI_test_05_id_8a1c7ed3160c4d0eb5a6257f3ef34614(self)
    self:Say("1st arg" .. asyncflow.get_event_param(EventId.Event2Arg, 0))
    return true
end

function asyncflow.node_funcs.func_AI_test_05_id_a0e5dc6615ea4cb3a3c2c229b598049c(self)
    self:Say("2nd arg" .. asyncflow.get_event_param(EventId.Event2Arg, 1))
    return true
end

function asyncflow.node_funcs.func_AI_test_06_id_12080d88c85f406bb2c5d63266c2228a(self)
    local ret = asyncflow.call_sub("AI.test_06_sub", self)
    return ret ~=0
end

function asyncflow.node_funcs.func_AI_test_06_id_bbd7cefd50b942728807fc2831476e07(self)
    self:Say("subunit pass")
    return true
end

function asyncflow.node_funcs.func_AI_test_06_sub_id_52d53ccc09ac4beda69f9ef0a8663ef5(self)
    asyncflow.ret(1)
    return true
end

function asyncflow.node_funcs.func_AI_test_07_id_9169be5d222349db89e0de785c0d6a74(self)
    local ret = asyncflow.call_sub("AI.test_07_sub", self, 1,2)
    return ret ~=0
end

function asyncflow.node_funcs.func_AI_test_07_id_edf49f0e00e149ddb9bb4f7f2f7b38b2(self)
    local ret = asyncflow.call_sub("AI.test_07_sub", self, 3,4)
    return ret ~=0
end

function asyncflow.node_funcs.func_AI_test_07_id_ae372d08c8a744e4ae0ab9ebfa73883a(self)
    self:Say(asyncflow.get_var(0))
    return true
end

function asyncflow.node_funcs.func_AI_test_07_sub_id_fac314b909574f9fb386a9a4cbbcf897(self)
    self:Say(asyncflow.get_var(0))
    return true
end

function asyncflow.node_funcs.func_AI_test_07_sub_id_d3ca5de79331406cb6468bcf80796782(self)
    self:Say(asyncflow.get_var(1))
    return true
end

function asyncflow.node_funcs.func_AI_test_07_sub_id_50cd0849040c491ba1bac88436c7fbfd(self)
    local ret = asyncflow.set_var(2, asyncflow.get_var(0) + asyncflow.get_var(1))
    return ret ~=0
end

function asyncflow.node_funcs.func_AI_test_07_sub_id_8612fd56143d4c4191826f9c67a97d4b(self)
    self:Say(asyncflow.get_var(2))
    return true
end

function asyncflow.node_funcs.func_AI_test_07_sub_id_84fc5277ee074792b045e255001eb5a5(self)
    asyncflow.ret(asyncflow.get_var(2))
    return true
end

function asyncflow.node_funcs.func_AI_test_08_id_a3e8ca5860a243eca9d0b0a783f9ecef(self)
    local ret = asyncflow.set_var(1, self:AsyncAdd(3,4))
    return ret
end

function asyncflow.node_funcs.func_AI_test_08_id_86be085d3008499ea34ee8430d59cca5(self)
    self:Say(asyncflow.get_var(1))
    return true
end

function asyncflow.node_funcs.func_AI_test_08_id_c87734cba7c048d6b8411e99677313c4(self)
    local ret = asyncflow.set_var(0, self:AsyncAdd(1,2))
    return ret
end

function asyncflow.node_funcs.func_AI_test_08_id_d0faf6e42a4348a79d306a0b079ab730(self)
    self:Say(asyncflow.get_var(0))
    return true
end

function asyncflow.node_funcs.func_AI_test_09_id_cad031eba4e34290ba788e14a3b11ba3(self)
    asyncflow.wait(3)
    return true
end

function asyncflow.node_funcs.func_AI_test_09_id_d4ef42ee02024c77bd6f2ab10081690e(self)
    self:Say("node 3")
    return true
end

function asyncflow.node_funcs.func_AI_test_09_id_7825f180d385472481a7f64c1d791554(self)
    self:Say("all node runned")
    return true
end

function asyncflow.node_funcs.func_AI_test_09_id_e8c4dbecdd694ea980121f04bae16444(self)
    asyncflow.wait(2)
    return true
end

function asyncflow.node_funcs.func_AI_test_09_id_9b64a62b1cf94374a293e329fe66d9a8(self)
    self:Say("node 2")
    return true
end

function asyncflow.node_funcs.func_AI_test_09_id_c8d16b1c904c4bf798a6ec8bbf703f37(self)
    asyncflow.wait(1)
    return true
end

function asyncflow.node_funcs.func_AI_test_09_id_cc6fe6d717f74f61a3b70652c293020a(self)
    self:Say("node 1")
    return true
end

function asyncflow.node_funcs.func_AI_test_10_id_e886cb0441184affaeaa17b79e10426d(self)
    local ret = asyncflow.set_var(0, self:CreateCharacter())
    return true
end

function asyncflow.node_funcs.func_AI_test_10_id_b93968aec81f4d4da508c8223064ef4a(self)
    local ret = asyncflow.wait_event(asyncflow.get_var(0), EventId.Event1Arg)
    return true
end

function asyncflow.node_funcs.func_AI_test_10_id_86dfd1f0ac4e41639589da2ea0ef0016(self)
    self:Say("event 1 pass" .. asyncflow.get_event_param(EventId.Event1Arg, 0))
    return true
end

function asyncflow.node_funcs.func_AI_test_10_id_0e216dd981de4983a93a08b1acb7f9b8(self)
    local ret = asyncflow.wait_event(asyncflow.get_var(0), EventId.Event2Arg)
    return true
end

function asyncflow.node_funcs.func_AI_test_10_id_d278c17dc6684644b403e02b710f3b77(self)
    self:Say("event 2 pass")
    return true
end

function asyncflow.node_funcs.func_AI_test_10_id_42aa83bff2b148cb93f40f306260de8b(self)
    self:Say("1st arg" .. asyncflow.get_event_param(EventId.Event2Arg, 0))
    return true
end

function asyncflow.node_funcs.func_AI_test_10_id_eacbd156f3ef4739ad4f7d8666e25061(self)
    self:Say("2nd arg" .. asyncflow.get_event_param(EventId.Event2Arg, 1))
    return true
end

function asyncflow.node_funcs.func_AI_test_10_id_a7c4d9fba6d546249ff8efe98e31dc44(self)
    local ret = asyncflow.wait_event(asyncflow.get_var(0), EventId.Event0Arg)
    return true
end

function asyncflow.node_funcs.func_AI_test_10_id_2a7e4570f894433e9e25d97a40f5785c(self)
    self:Say("event 0 pass")
    return true
end

function asyncflow.node_funcs.func_AI_test_11_id_e2525217efbf4ebf99f3b86798386f34(self)
    local ret = asyncflow.call_sub("AI.test_11_sub", self)
    return ret
end

function asyncflow.node_funcs.func_AI_test_11_id_26e9d8f7af1a4470a418c3f4aee4d292(self)
    self:Say("subunit pass")
    return true
end

function asyncflow.node_funcs.func_AI_test_11_id_e6e2831309464d679a474d01bf55842b(self)
    self:Say("red")
    return true
end

function asyncflow.node_funcs.func_AI_test_11_id_47d891df53ff42978d4c3f237d6f1678(self)
    self:Say("green")
    return true
end

function asyncflow.node_funcs.func_AI_test_11_sub_id_d7014a211f044117a75004b7c0c6e259(self)
    self:Say("hello sub")
    return true
end

function asyncflow.node_funcs.func_AI_test_12_id_dcc94bd684af4256b35b1b6b569fd92c(self)
    self:Say("1")
    return true
end

function asyncflow.node_funcs.func_AI_test_12_id_9ebbe264cc964a4d95914440a062fa7f(self)
    self:Say("2")
    return true
end

function asyncflow.node_funcs.func_AI_test_12_id_b741aa2dae5f40f5834b6c2f31c2fca6(self)
    local ret = asyncflow.wait_event(self, EventId.Event1Arg)
    return true
end

function asyncflow.node_funcs.func_AI_test_12_id_26ad3735d1b04e1ea1523c2184f7c103(self)
    self:Say("event 0 pass")
    return true
end

function asyncflow.node_funcs.func_AI_test_12_id_2a7e47308bd4416c8b4ae849709a3e61(self)
    asyncflow.wait(1)
    return true
end

function asyncflow.node_funcs.func_AI_test_12_id_f8e95d4cf35c4c3ca373481f45562fed(self)
    self:Say("hello")
    return true
end

function asyncflow.node_funcs.func_AI_test_13_id_77c3eff07e174b4190ccc1bb215d834f(self)
    self:Say("hello")
    return true
end

function asyncflow.node_funcs.func_AI_test_13_id_1136de93efa140d2bc91882393c2811b(self)
    self:Say("1")
    return true
end

function asyncflow.node_funcs.func_AI_test_13_id_8658e0ed635c4166b2f6da54657ef2d0(self)
    self:Say("2")
    return true
end

function asyncflow.node_funcs.func_AI_test_14_id_885dd3312aaa4823aaa2cfac700e4844(self)
    self:Say(1)
    return true
end

function asyncflow.node_funcs.func_AI_test_14_id_2f7be34327f24fa0b535097fe666cfbe(self)
    self:Say(2)
    return true
end

function asyncflow.node_funcs.func_AI_test_14_id_7fc1f00251a046af905e2ef32d9a469e(self)
    self:Say(3)
    return true
end

function asyncflow.node_funcs.func_AI_test_14_id_a2ff83bfb2df48fd97b3a296db445d0f(self)
    self:Say(4)
    return true
end

function asyncflow.node_funcs.func_AI_test_15_id_c2ef46ac033c49a4926b34b57460aea4(self)
    asyncflow.wait(2)
    return true
end

function asyncflow.node_funcs.func_AI_test_15_id_21e88f49ee08465185f28fbf755cbf6e(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_15_id_ffbf71024ce145ae99a55e31d2b3c925(self)
    asyncflow.wait(5)
    return true
end

function asyncflow.node_funcs.func_AI_test_15_id_2d49ef38d1ca4024b1ee6ad1c9a954aa(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_15_id_6755d438d5f844baba7de0bb822d80be(self)
    asyncflow.wait(1)
    return true
end

function asyncflow.node_funcs.func_AI_test_15_id_930cc0978159403aaa57db6d967df8a4(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_16_id_08af1e3195a6434498270071af0c8f0d(self)
    asyncflow.wait(2)
    return true
end

function asyncflow.node_funcs.func_AI_test_16_id_c21382fe4be74b5b90b98fa464955be3(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_16_id_a59747ae1ba64774b7b01e2a1367c21b(self)
    local ret = asyncflow.wait_event(self, EventId.Event0Arg)
    return true
end

function asyncflow.node_funcs.func_AI_test_16_id_e4f2099363374023941118f7e266ae92(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_16_id_47bb1b892f084f3faf2e379b737193de(self)
    asyncflow.wait(1)
    return true
end

function asyncflow.node_funcs.func_AI_test_16_id_83d7e4d0d23e479c861e45a18ecd2060(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_17_id_670513fc98454b4db29124bdbc4bf942(self)
    asyncflow.wait(2)
    return true
end

function asyncflow.node_funcs.func_AI_test_17_id_590f60f752f04a40a511de6bda60afdd(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_17_id_e1330480adb54f65a73066597afd098e(self)
    local ret = asyncflow.call_sub("AI.test_17_sub", self)
    return ret
end

function asyncflow.node_funcs.func_AI_test_17_id_30f45e8ceb1e46fd936f69159226b54d(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_17_id_ade192269da34394a7eb87a47ea4c81e(self)
    asyncflow.wait(1)
    return true
end

function asyncflow.node_funcs.func_AI_test_17_id_3bae8e8241b548e9ad7e6d01e8707f7c(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_17_sub_id_924beff49dbb48d483747ac54782f6f0(self)
    self:Say(asyncflow.time())
    return true
end

function asyncflow.node_funcs.func_AI_test_17_sub_id_c5b7b977b0994bea8cfcf4bf57508de0(self)
    asyncflow.wait(3)
    return true
end

function asyncflow.node_funcs.func_AI_test_18_id_2d914be5d7534adeaa52f51585d7a54e(self)
    local ret = asyncflow.set_var(0, "hello")
    return true
end

function asyncflow.node_funcs.func_AI_test_18_id_9517f2f34106421098628fff57ae5e3e(self)
    self:Say(asyncflow.get_var(0))
    return true
end

function asyncflow.node_funcs.func_AI_test_18_id_2ba5507f48124a0ebc1ed621dc4930a7(self)
    local ret = asyncflow.set_var(0, "ss111")
    return true
end

function asyncflow.node_funcs.func_AI_test_18_id_365a6ef2dca547d6bbcb03bd7589c49b(self)
    local ret = asyncflow.wait_event(self, EventId.Event0Arg)
    return true
end

function asyncflow.node_funcs.func_AI_test_18_id_ea0871440caa47b1bae4ec66b238e270(self)
    self:Say("event 0 pass")
    return true
end

function asyncflow.node_funcs.func_AI_test_18_id_6288a287371f4586a4cd7a6f274712dd(self)
    local ret = asyncflow.wait_event(self, EventId.Event2Arg)
    return true
end

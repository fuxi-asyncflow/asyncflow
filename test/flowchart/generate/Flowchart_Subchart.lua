if asyncflow.node_funcs == nil then asyncflow.node_funcs = {} end
local EventId = asyncflow.EventId
function asyncflow.node_funcs.Subchart_SubchartTest_01_id_42a43a6e54514e45afcfd15f46cbbe03(self)
    local ret = self:Say("hello")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_01_id_dca03dece2a04233a5618dc68c9cf5bd(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_01_sub", self)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_01_id_fc24d0b59b914be2a62cb8e93bb04aa9(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_01_sub_id_e73bdb60cc12406198c22d25bdaf9383(self)
    local ret = self:Say("hellosub")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_01_sub_id_266276762f0c4ae59b8baa1b498f05b7(self)
    local ret = asyncflow.wait(1)
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_01_sub_id_ea2ddbba34484e36a827e2f73d8ac897(self)
    local ret = asyncflow.ret(1)
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_02_id_9c62aa07bc894002943af2b9760f9bf3(self)
    local ret = asyncflow.set_var(0, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_02_id_fb738de1dcfd4041a45624584f8a6596(self)
    local ret = self:Say("hello")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_02_id_f67352ec4c7644ddb0cf903835f11f01(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_01_sub", asyncflow.get_var(0))
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_02_id_a31dbf21ae574b9eaed2d72273f22091(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_03_id_53d285961cbb403482b040d60b7aa6ff(self)
    local ret = asyncflow.set_var(0, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_03_id_d02f8ce037bf4cb2be0440f504764ad0(self)
    local ret = asyncflow.set_var(1, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_03_id_d802d10897ae44419a31852331f1e491(self)
    local ret = asyncflow.set_var(2, {asyncflow.get_var(0), asyncflow.get_var(1)})
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_03_id_ca65b52b0dbb436887af98e9b59eba68(self)
    local ret = asyncflow.set_var(3, 1)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_03_id_370b0b893381472e862ec67b05ed37f5(self)
    local ret = self:Say("hello")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_03_id_b49315c6f6d44c8da2a24371dc5e59aa(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_01_sub", asyncflow.get_var(2)[asyncflow.get_var(3)])
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_03_id_133c1db850f448bbb58ab69a64c08ee0(self)
    local ret = asyncflow.set_var(3, 1 + (asyncflow.get_var(3) + 1) % 2)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_03_id_77c1994817624bf1be2a13a3dd72eea4(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_04_id_94165897646e4b1ca4f34c560a37a222(self)
    local ret = self:Say("hello")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_04_id_b1b6ff9be3104b0cab7f7af889e89d53(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_04_sub", self)
    return ret
end
function asyncflow.node_funcs.Subchart_SubchartTest_04_id_3d865c0443344fc79e675f19126bba6f(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_04_sub_id_98a94c83e0424336a633ff94baa3a3e9(self)
    local ret = self:Say("hellosub")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_04_sub_id_22db35f73af84406b441e216f7dc4852(self)
    local ret = asyncflow.wait(1)
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_05_id_d6cd6a2b73e345fcb276e4e16ec9422e(self)
    local ret = asyncflow.set_var(0, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_05_id_4d0de3891cf24f6ba32e8c2ce3b2efd9(self)
    local ret = self:Say("hello")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_05_id_081c769291b44bd2bc2c49ab14ed6520(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_04_sub", asyncflow.get_var(0))
    return ret
end
function asyncflow.node_funcs.Subchart_SubchartTest_05_id_eecc6db7972445c9b24d38d3b5d6e02e(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_06_id_90ee7f98deb04f14b95c689f558eacfa(self)
    local ret = asyncflow.set_var(0, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_06_id_57b18856751b4df28098e7e0257bc109(self)
    local ret = asyncflow.set_var(1, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_06_id_0ded790ca5d94adca08635c4f0550e38(self)
    local ret = asyncflow.set_var(2, {asyncflow.get_var(0), asyncflow.get_var(1)})
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_06_id_07130982db514acc8381dc1ac12ce561(self)
    local ret = asyncflow.set_var(3, 1)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_06_id_119bffe10d054c479b41c94e26f3df06(self)
    local ret = self:Say("hello")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_06_id_767f30ee29ce4085bc9c0ee2f511f49d(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_04_sub", asyncflow.get_var(2)[asyncflow.get_var(3)])
    return ret
end
function asyncflow.node_funcs.Subchart_SubchartTest_06_id_1f7e7ecd8cd647238ee277018e7744f6(self)
    local ret = asyncflow.set_var(3, 1 + (asyncflow.get_var(3) + 1) % 2)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_06_id_4e2f23bb60814ac19ad2c0de288116d4(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_07_id_cc5b038230ff498988d5e3c97486f8d0(self)
    local ret = self:Say("hello")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_07_id_9d7479fd0ed64117ac868fa914e6bf23(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_07_sub", self)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_07_id_69f4517afe49446d81244fac4a1f69cc(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_07_sub_id_dc32fef005424047a0cd29588cf9304e(self)
    local ret = self:Say("hellosub")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_07_sub_id_3881148c21a44fc29280ff8b3330fa4f(self)
    local ret = asyncflow.ret(1)
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_08_id_ecb7cf4b2d254ae38663b6ca76241c22(self)
    local ret = asyncflow.set_var(0, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_08_id_0ce4ec77462e491eb630ae99e9650937(self)
    local ret = self:Say("hello")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_08_id_c2b07004eedd4cd5aaecc630dde26f8f(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_07_sub", asyncflow.get_var(0))
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_08_id_441812d2ea634ca1891a2343159a0e9a(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_09_id_c633b30637554a21a23d91a310e20ac6(self)
    local ret = asyncflow.set_var(0, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_09_id_5a84181fef9644aaaa5433b337941ce2(self)
    local ret = asyncflow.set_var(1, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_09_id_e10130034cd14bc39fea3047237c975c(self)
    local ret = asyncflow.set_var(2, {asyncflow.get_var(0), asyncflow.get_var(1)})
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_09_id_53dde06bda054e46a38bf7f721c5e538(self)
    local ret = asyncflow.set_var(3, 1)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_09_id_7147e19370734e80abd721248e055e1e(self)
    local ret = self:Say("hello")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_09_id_161afe7f9987433aa42e71d691785b95(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_07_sub", asyncflow.get_var(2)[asyncflow.get_var(3)])
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_09_id_b13723e7fa6944b0941fbb4de31b0ff2(self)
    local ret = asyncflow.set_var(3, 1 + (asyncflow.get_var(3) + 1) % 2)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_09_id_c0d87c34083e42bba62f6803b1da01ad(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_10_id_e1389f5d6a5b41559a579782cb791f31(self)
    local ret = asyncflow.set_var(0, self:CreateCharacter())
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_10_id_de1254a491ea42f0aa1f6935ab06102c(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_10_sub", asyncflow.get_var(0))
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_10_id_19ef73dcd7e946e7b5c2cbc34627d280(self)
    local ret = asyncflow.deregister(asyncflow.get_var(0))
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_10_id_a896783bee8d414199a15c202d7b3736(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_10_sub_id_37fb63219c8249df91a47fa358bfeeae(self)
    local ret = self:Say("joinsub")
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_10_sub_id_bec46862dfd441ff8121b81dcc28df46(self)
    local ret = asyncflow.wait(1)
    return true
end
function asyncflow.node_funcs.Subchart_SubchartTest_10_sub_id_36f67553b0044196ba245152c0987544(self)
    local ret = asyncflow.ret(1)
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_11_id_4c2b870e43ba459494314448ce047cfd(self)
    local ret = asyncflow.set_var(0, 0)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_11_id_8134b765856745239df8c754ff132868(self)
    local ret = asyncflow.get_var(0) < 3
    return ret
end
function asyncflow.node_funcs.Subchart_SubchartTest_11_id_b0909c46d2c9464a9cef61c790964e18(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_01_sub", self)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_11_id_d8e5d41748cd4f79b0a8b879baec801f(self)
    local ret = asyncflow.set_var(0, asyncflow.get_var(0) + 1)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_11_id_2ea6055927634502a84584bbd098f83b(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_12_id_f91d56f7249d437698870d204a8c819f(self)
    local ret = asyncflow.set_var(0, 0)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_12_id_295a1f172ea24251802f1d8a9ace7a9d(self)
    local ret = asyncflow.get_var(0) < 3
    return ret
end
function asyncflow.node_funcs.Subchart_SubchartTest_12_id_22db2edad8c04a7381589fc1c4f0050a(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_04_sub", self)
    return ret
end
function asyncflow.node_funcs.Subchart_SubchartTest_12_id_c0bbb9a37a464ef78138014769ec09b7(self)
    local ret = asyncflow.set_var(0, asyncflow.get_var(0) + 1)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_12_id_9c436c7877f445eeb9a446bb1401824c(self)
    local ret = self:Say("end")
    return true
end

function asyncflow.node_funcs.Subchart_SubchartTest_13_id_d02710f383004ded9eed2e1125cb75e7(self)
    local ret = asyncflow.set_var(0, 0)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_13_id_4d9b024b013d4e32920bb87454570c1e(self)
    local ret = asyncflow.get_var(0) < 3
    return ret
end
function asyncflow.node_funcs.Subchart_SubchartTest_13_id_7bbd70650b024598aeca6d34d5ad70c2(self)
    local ret = asyncflow.call_sub("Subchart.SubchartTest_07_sub", self)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_13_id_8151dfbbf8e54164a7ea6cd70125a107(self)
    local ret = asyncflow.set_var(0, asyncflow.get_var(0) + 1)
    return ret ~= 0
end
function asyncflow.node_funcs.Subchart_SubchartTest_13_id_76b0b09faf18488fb468531138d5696d(self)
    local ret = self:Say("end")
    return true
end


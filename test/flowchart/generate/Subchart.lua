-- Say("hello")
local function f_42a43a6e54514e45afcfd15f46cbbe03(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_01.42a43a6e54514e45afcfd15f46cbbe03", f_42a43a6e54514e45afcfd15f46cbbe03)

-- SubchartTest_01_sub()
local function f_dca03dece2a04233a5618dc68c9cf5bd(self)
    local __ret__ = asyncflow.call_sub("Subchart.SubchartTest_01_sub", self)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_01.dca03dece2a04233a5618dc68c9cf5bd", f_dca03dece2a04233a5618dc68c9cf5bd)

-- Say("end")
local function f_fc24d0b59b914be2a62cb8e93bb04aa9(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_01.fc24d0b59b914be2a62cb8e93bb04aa9", f_fc24d0b59b914be2a62cb8e93bb04aa9)

-- Say("hellosub")
local function f_e73bdb60cc12406198c22d25bdaf9383(self)
    self:Say("hellosub")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_01_sub.e73bdb60cc12406198c22d25bdaf9383", f_e73bdb60cc12406198c22d25bdaf9383)

-- wait(1)
local function f_266276762f0c4ae59b8baa1b498f05b7(self)
    asyncflow.wait(1)
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_01_sub.266276762f0c4ae59b8baa1b498f05b7", f_266276762f0c4ae59b8baa1b498f05b7)

-- return(1)
local function f_ea2ddbba34484e36a827e2f73d8ac897(self)
    asyncflow.ret(1)
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_01_sub.ea2ddbba34484e36a827e2f73d8ac897", f_ea2ddbba34484e36a827e2f73d8ac897)

-- $s1=CreateCharacter()
local function f_9c62aa07bc894002943af2b9760f9bf3(self)
    local __ret__ = asyncflow.set_var("s1", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_02.9c62aa07bc894002943af2b9760f9bf3", f_9c62aa07bc894002943af2b9760f9bf3)

-- Say("hello")
local function f_fb738de1dcfd4041a45624584f8a6596(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_02.fb738de1dcfd4041a45624584f8a6596", f_fb738de1dcfd4041a45624584f8a6596)

-- $s1.SubchartTest_01_sub()
local function f_f67352ec4c7644ddb0cf903835f11f01(self)
    local __ret__ = asyncflow.call_sub("Subchart.SubchartTest_01_sub", asyncflow.get_var("s1"))
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_02.f67352ec4c7644ddb0cf903835f11f01", f_f67352ec4c7644ddb0cf903835f11f01)

-- Say("end")
local function f_a31dbf21ae574b9eaed2d72273f22091(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_02.a31dbf21ae574b9eaed2d72273f22091", f_a31dbf21ae574b9eaed2d72273f22091)

-- $s1=CreateCharacter()
local function f_53d285961cbb403482b040d60b7aa6ff(self)
    local __ret__ = asyncflow.set_var("s1", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_03.53d285961cbb403482b040d60b7aa6ff", f_53d285961cbb403482b040d60b7aa6ff)

-- $s2=CreateCharacter()
local function f_d02f8ce037bf4cb2be0440f504764ad0(self)
    local __ret__ = asyncflow.set_var("s2", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_03.d02f8ce037bf4cb2be0440f504764ad0", f_d02f8ce037bf4cb2be0440f504764ad0)

-- $s={$s1,$s2}
local function f_d802d10897ae44419a31852331f1e491(self)
    local __ret__ = asyncflow.set_var("s", {asyncflow.get_var("s1"), asyncflow.get_var("s2")})
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_03.d802d10897ae44419a31852331f1e491", f_d802d10897ae44419a31852331f1e491)

-- $index=1
local function f_ca65b52b0dbb436887af98e9b59eba68(self)
    local __ret__ = asyncflow.set_var("index", 1)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_03.ca65b52b0dbb436887af98e9b59eba68", f_ca65b52b0dbb436887af98e9b59eba68)

-- Say("hello")
local function f_370b0b893381472e862ec67b05ed37f5(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_03.370b0b893381472e862ec67b05ed37f5", f_370b0b893381472e862ec67b05ed37f5)

-- $s[$index].SubchartTest_01_sub()
local function f_b49315c6f6d44c8da2a24371dc5e59aa(self)
    local __ret__ = asyncflow.call_sub("Subchart.SubchartTest_01_sub", (asyncflow.get_var("s"))[asyncflow.get_var("index")])
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_03.b49315c6f6d44c8da2a24371dc5e59aa", f_b49315c6f6d44c8da2a24371dc5e59aa)

-- $index=1+($index+1)%2
local function f_133c1db850f448bbb58ab69a64c08ee0(self)
    local __ret__ = asyncflow.set_var("index", 1 + asyncflow.get_var("index") + 1 % 2)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_03.133c1db850f448bbb58ab69a64c08ee0", f_133c1db850f448bbb58ab69a64c08ee0)

-- Say("end")
local function f_77c1994817624bf1be2a13a3dd72eea4(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_03.77c1994817624bf1be2a13a3dd72eea4", f_77c1994817624bf1be2a13a3dd72eea4)

-- Say("hello")
local function f_94165897646e4b1ca4f34c560a37a222(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_04.94165897646e4b1ca4f34c560a37a222", f_94165897646e4b1ca4f34c560a37a222)

-- SubchartTest_04_sub()
local function f_b1b6ff9be3104b0cab7f7af889e89d53(self)
    asyncflow.call_sub("Subchart.SubchartTest_04_sub", self)
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_04.b1b6ff9be3104b0cab7f7af889e89d53", f_b1b6ff9be3104b0cab7f7af889e89d53)

-- Say("end")
local function f_3d865c0443344fc79e675f19126bba6f(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_04.3d865c0443344fc79e675f19126bba6f", f_3d865c0443344fc79e675f19126bba6f)

-- Say("hellosub")
local function f_98a94c83e0424336a633ff94baa3a3e9(self)
    self:Say("hellosub")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_04_sub.98a94c83e0424336a633ff94baa3a3e9", f_98a94c83e0424336a633ff94baa3a3e9)

-- wait(1)
local function f_22db35f73af84406b441e216f7dc4852(self)
    asyncflow.wait(1)
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_04_sub.22db35f73af84406b441e216f7dc4852", f_22db35f73af84406b441e216f7dc4852)

-- $s1=CreateCharacter()
local function f_d6cd6a2b73e345fcb276e4e16ec9422e(self)
    local __ret__ = asyncflow.set_var("s1", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_05.d6cd6a2b73e345fcb276e4e16ec9422e", f_d6cd6a2b73e345fcb276e4e16ec9422e)

-- Say("hello")
local function f_4d0de3891cf24f6ba32e8c2ce3b2efd9(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_05.4d0de3891cf24f6ba32e8c2ce3b2efd9", f_4d0de3891cf24f6ba32e8c2ce3b2efd9)

-- $s1.SubchartTest_04_sub()
local function f_081c769291b44bd2bc2c49ab14ed6520(self)
    asyncflow.call_sub("Subchart.SubchartTest_04_sub", asyncflow.get_var("s1"))
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_05.081c769291b44bd2bc2c49ab14ed6520", f_081c769291b44bd2bc2c49ab14ed6520)

-- Say("end")
local function f_eecc6db7972445c9b24d38d3b5d6e02e(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_05.eecc6db7972445c9b24d38d3b5d6e02e", f_eecc6db7972445c9b24d38d3b5d6e02e)

-- $s1=CreateCharacter()
local function f_90ee7f98deb04f14b95c689f558eacfa(self)
    local __ret__ = asyncflow.set_var("s1", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_06.90ee7f98deb04f14b95c689f558eacfa", f_90ee7f98deb04f14b95c689f558eacfa)

-- $s2=CreateCharacter()
local function f_57b18856751b4df28098e7e0257bc109(self)
    local __ret__ = asyncflow.set_var("s2", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_06.57b18856751b4df28098e7e0257bc109", f_57b18856751b4df28098e7e0257bc109)

-- $s={$s1,$s2}
local function f_0ded790ca5d94adca08635c4f0550e38(self)
    local __ret__ = asyncflow.set_var("s", {asyncflow.get_var("s1"), asyncflow.get_var("s2")})
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_06.0ded790ca5d94adca08635c4f0550e38", f_0ded790ca5d94adca08635c4f0550e38)

-- $index=1
local function f_07130982db514acc8381dc1ac12ce561(self)
    local __ret__ = asyncflow.set_var("index", 1)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_06.07130982db514acc8381dc1ac12ce561", f_07130982db514acc8381dc1ac12ce561)

-- Say("hello")
local function f_119bffe10d054c479b41c94e26f3df06(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_06.119bffe10d054c479b41c94e26f3df06", f_119bffe10d054c479b41c94e26f3df06)

-- $s[$index].SubchartTest_04_sub()
local function f_767f30ee29ce4085bc9c0ee2f511f49d(self)
    asyncflow.call_sub("Subchart.SubchartTest_04_sub", (asyncflow.get_var("s"))[asyncflow.get_var("index")])
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_06.767f30ee29ce4085bc9c0ee2f511f49d", f_767f30ee29ce4085bc9c0ee2f511f49d)

-- $index=1+($index+1)%2
local function f_1f7e7ecd8cd647238ee277018e7744f6(self)
    local __ret__ = asyncflow.set_var("index", 1 + asyncflow.get_var("index") + 1 % 2)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_06.1f7e7ecd8cd647238ee277018e7744f6", f_1f7e7ecd8cd647238ee277018e7744f6)

-- Say("end")
local function f_4e2f23bb60814ac19ad2c0de288116d4(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_06.4e2f23bb60814ac19ad2c0de288116d4", f_4e2f23bb60814ac19ad2c0de288116d4)

-- Say("hello")
local function f_cc5b038230ff498988d5e3c97486f8d0(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_07.cc5b038230ff498988d5e3c97486f8d0", f_cc5b038230ff498988d5e3c97486f8d0)

-- SubchartTest_07_sub()
local function f_9d7479fd0ed64117ac868fa914e6bf23(self)
    local __ret__ = asyncflow.call_sub("Subchart.SubchartTest_07_sub", self)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_07.9d7479fd0ed64117ac868fa914e6bf23", f_9d7479fd0ed64117ac868fa914e6bf23)

-- Say("end")
local function f_69f4517afe49446d81244fac4a1f69cc(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_07.69f4517afe49446d81244fac4a1f69cc", f_69f4517afe49446d81244fac4a1f69cc)

-- Say("hellosub")
local function f_dc32fef005424047a0cd29588cf9304e(self)
    self:Say("hellosub")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_07_sub.dc32fef005424047a0cd29588cf9304e", f_dc32fef005424047a0cd29588cf9304e)

-- return(1)
local function f_3881148c21a44fc29280ff8b3330fa4f(self)
    asyncflow.ret(1)
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_07_sub.3881148c21a44fc29280ff8b3330fa4f", f_3881148c21a44fc29280ff8b3330fa4f)

-- $s1=CreateCharacter()
local function f_ecb7cf4b2d254ae38663b6ca76241c22(self)
    local __ret__ = asyncflow.set_var("s1", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_08.ecb7cf4b2d254ae38663b6ca76241c22", f_ecb7cf4b2d254ae38663b6ca76241c22)

-- Say("hello")
local function f_0ce4ec77462e491eb630ae99e9650937(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_08.0ce4ec77462e491eb630ae99e9650937", f_0ce4ec77462e491eb630ae99e9650937)

-- $s1.SubchartTest_07_sub()
local function f_c2b07004eedd4cd5aaecc630dde26f8f(self)
    local __ret__ = asyncflow.call_sub("Subchart.SubchartTest_07_sub", asyncflow.get_var("s1"))
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_08.c2b07004eedd4cd5aaecc630dde26f8f", f_c2b07004eedd4cd5aaecc630dde26f8f)

-- Say("end")
local function f_441812d2ea634ca1891a2343159a0e9a(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_08.441812d2ea634ca1891a2343159a0e9a", f_441812d2ea634ca1891a2343159a0e9a)

-- $s1=CreateCharacter()
local function f_c633b30637554a21a23d91a310e20ac6(self)
    local __ret__ = asyncflow.set_var("s1", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_09.c633b30637554a21a23d91a310e20ac6", f_c633b30637554a21a23d91a310e20ac6)

-- $s2=CreateCharacter()
local function f_5a84181fef9644aaaa5433b337941ce2(self)
    local __ret__ = asyncflow.set_var("s2", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_09.5a84181fef9644aaaa5433b337941ce2", f_5a84181fef9644aaaa5433b337941ce2)

-- $s={$s1,$s2}
local function f_e10130034cd14bc39fea3047237c975c(self)
    local __ret__ = asyncflow.set_var("s", {asyncflow.get_var("s1"), asyncflow.get_var("s2")})
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_09.e10130034cd14bc39fea3047237c975c", f_e10130034cd14bc39fea3047237c975c)

-- $index=1
local function f_53dde06bda054e46a38bf7f721c5e538(self)
    local __ret__ = asyncflow.set_var("index", 1)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_09.53dde06bda054e46a38bf7f721c5e538", f_53dde06bda054e46a38bf7f721c5e538)

-- Say("hello")
local function f_7147e19370734e80abd721248e055e1e(self)
    self:Say("hello")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_09.7147e19370734e80abd721248e055e1e", f_7147e19370734e80abd721248e055e1e)

-- $s[$index].SubchartTest_07_sub()
local function f_161afe7f9987433aa42e71d691785b95(self)
    local __ret__ = asyncflow.call_sub("Subchart.SubchartTest_07_sub", (asyncflow.get_var("s"))[asyncflow.get_var("index")])
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_09.161afe7f9987433aa42e71d691785b95", f_161afe7f9987433aa42e71d691785b95)

-- $index=1+($index+1)%2
local function f_b13723e7fa6944b0941fbb4de31b0ff2(self)
    local __ret__ = asyncflow.set_var("index", 1 + asyncflow.get_var("index") + 1 % 2)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_09.b13723e7fa6944b0941fbb4de31b0ff2", f_b13723e7fa6944b0941fbb4de31b0ff2)

-- Say("end")
local function f_c0d87c34083e42bba62f6803b1da01ad(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_09.c0d87c34083e42bba62f6803b1da01ad", f_c0d87c34083e42bba62f6803b1da01ad)

-- $s1=CreateCharacter()
local function f_e1389f5d6a5b41559a579782cb791f31(self)
    local __ret__ = asyncflow.set_var("s1", self:CreateCharacter())
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_10.e1389f5d6a5b41559a579782cb791f31", f_e1389f5d6a5b41559a579782cb791f31)

-- $s1.SubchartTest_10_sub()
local function f_de1254a491ea42f0aa1f6935ab06102c(self)
    local __ret__ = asyncflow.call_sub("Subchart.SubchartTest_10_sub", asyncflow.get_var("s1"))
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_10.de1254a491ea42f0aa1f6935ab06102c", f_de1254a491ea42f0aa1f6935ab06102c)

-- deregister($s1)
local function f_19ef73dcd7e946e7b5c2cbc34627d280(self)
    deregister(asyncflow.get_var("s1"))
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_10.19ef73dcd7e946e7b5c2cbc34627d280", f_19ef73dcd7e946e7b5c2cbc34627d280)

-- Say("end")
local function f_a896783bee8d414199a15c202d7b3736(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_10.a896783bee8d414199a15c202d7b3736", f_a896783bee8d414199a15c202d7b3736)

-- Say("joinsub")
local function f_37fb63219c8249df91a47fa358bfeeae(self)
    self:Say("joinsub")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_10_sub.37fb63219c8249df91a47fa358bfeeae", f_37fb63219c8249df91a47fa358bfeeae)

-- wait(1)
local function f_bec46862dfd441ff8121b81dcc28df46(self)
    asyncflow.wait(1)
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_10_sub.bec46862dfd441ff8121b81dcc28df46", f_bec46862dfd441ff8121b81dcc28df46)

-- return(1)
local function f_36f67553b0044196ba245152c0987544(self)
    asyncflow.ret(1)
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_10_sub.36f67553b0044196ba245152c0987544", f_36f67553b0044196ba245152c0987544)

-- $s1=0
local function f_4c2b870e43ba459494314448ce047cfd(self)
    local __ret__ = asyncflow.set_var("s1", 0)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_11.4c2b870e43ba459494314448ce047cfd", f_4c2b870e43ba459494314448ce047cfd)

-- $s1<3
local function f_8134b765856745239df8c754ff132868(self)
    local __ret__ = asyncflow.get_var("s1") < 3
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_11.8134b765856745239df8c754ff132868", f_8134b765856745239df8c754ff132868)

-- Say("end")
local function f_2ea6055927634502a84584bbd098f83b(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_11.2ea6055927634502a84584bbd098f83b", f_2ea6055927634502a84584bbd098f83b)

-- SubchartTest_01_sub()
local function f_b0909c46d2c9464a9cef61c790964e18(self)
    local __ret__ = asyncflow.call_sub("Subchart.SubchartTest_01_sub", self)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_11.b0909c46d2c9464a9cef61c790964e18", f_b0909c46d2c9464a9cef61c790964e18)

-- $s1=$s1+1
local function f_d8e5d41748cd4f79b0a8b879baec801f(self)
    local __ret__ = asyncflow.set_var("s1", asyncflow.get_var("s1") + 1)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_11.d8e5d41748cd4f79b0a8b879baec801f", f_d8e5d41748cd4f79b0a8b879baec801f)

-- $s1=0
local function f_f91d56f7249d437698870d204a8c819f(self)
    local __ret__ = asyncflow.set_var("s1", 0)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_12.f91d56f7249d437698870d204a8c819f", f_f91d56f7249d437698870d204a8c819f)

-- $s1<3
local function f_295a1f172ea24251802f1d8a9ace7a9d(self)
    local __ret__ = asyncflow.get_var("s1") < 3
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_12.295a1f172ea24251802f1d8a9ace7a9d", f_295a1f172ea24251802f1d8a9ace7a9d)

-- Say("end")
local function f_9c436c7877f445eeb9a446bb1401824c(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_12.9c436c7877f445eeb9a446bb1401824c", f_9c436c7877f445eeb9a446bb1401824c)

-- SubchartTest_04_sub()
local function f_22db2edad8c04a7381589fc1c4f0050a(self)
    asyncflow.call_sub("Subchart.SubchartTest_04_sub", self)
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_12.22db2edad8c04a7381589fc1c4f0050a", f_22db2edad8c04a7381589fc1c4f0050a)

-- $s1=$s1+1
local function f_c0bbb9a37a464ef78138014769ec09b7(self)
    local __ret__ = asyncflow.set_var("s1", asyncflow.get_var("s1") + 1)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_12.c0bbb9a37a464ef78138014769ec09b7", f_c0bbb9a37a464ef78138014769ec09b7)

-- $s1=0
local function f_d02710f383004ded9eed2e1125cb75e7(self)
    local __ret__ = asyncflow.set_var("s1", 0)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_13.d02710f383004ded9eed2e1125cb75e7", f_d02710f383004ded9eed2e1125cb75e7)

-- $s1<3
local function f_4d9b024b013d4e32920bb87454570c1e(self)
    local __ret__ = asyncflow.get_var("s1") < 3
    return __ret__
end
asyncflow.set_node_func("Subchart.SubchartTest_13.4d9b024b013d4e32920bb87454570c1e", f_4d9b024b013d4e32920bb87454570c1e)

-- Say("end")
local function f_76b0b09faf18488fb468531138d5696d(self)
    self:Say("end")
    return true
end
asyncflow.set_node_func("Subchart.SubchartTest_13.76b0b09faf18488fb468531138d5696d", f_76b0b09faf18488fb468531138d5696d)

-- SubchartTest_07_sub()
local function f_7bbd70650b024598aeca6d34d5ad70c2(self)
    local __ret__ = asyncflow.call_sub("Subchart.SubchartTest_07_sub", self)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_13.7bbd70650b024598aeca6d34d5ad70c2", f_7bbd70650b024598aeca6d34d5ad70c2)

-- $s1=$s1+1
local function f_8151dfbbf8e54164a7ea6cd70125a107(self)
    local __ret__ = asyncflow.set_var("s1", asyncflow.get_var("s1") + 1)
    return __ret__ ~= 0
end
asyncflow.set_node_func("Subchart.SubchartTest_13.8151dfbbf8e54164a7ea6cd70125a107", f_8151dfbbf8e54164a7ea6cd70125a107)

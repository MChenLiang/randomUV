eg:
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" -DMAYA_VERSION=2018 ../

# randomUV
大楼中住户，随着时间不断变换的光影效果。

# 制作思路
通过时间点移动玻璃的位置实现。

# 后期制作方案
在速率确定之后，导出abc。 后续使用abc进行渲染。

# 节点属性介绍
inMesh      ：输入模型
time        ：当前时间。time1节点outTime
stTime      ：从第几帧开始结算
frequency   ：每多少帧变换一次。这是个可key帧属性，可以进行key帧来调节变换速率
outMesh     ：输出模型形态（这是已经改变过UV的形态）
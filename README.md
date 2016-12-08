TH15ModLoader
=========

目标是用C++魔改东方，不用管ECL和其他限制

**只适用于东方绀珠传1.00b**

使用方法：编译后把d3d9.dll放到你的游戏目录，在游戏目录下新建`mods`目录，把MOD的dll放到mods目录

附加功能：优先读取data目录下的资源，魔改资源就不用打包了，新建`data`目录把要替换的资源放进去


搭建开发环境
---------

### MOD

参考ModSample

1. 用VS2013编译TML，把dll放到你的游戏目录
2. 新建dll项目,导入TML的lib库文件，include相应的头文件
3. （可选）把生成路径设置到mods目录，把调试的命令设置为启动th15.exe
4. 在DllMain里注册事件监听器，实现各种功能

### TML

1. 本项目使用VS2013编写，更高版本的VS应该也可以
2. 把`CommonProperty.props.example`复制一份并重命名为`CommonProperty.props`
3. 用文本编辑器（或在VS2013里）打开CommonProperty.props，把里面的TH15目录设置为你的东方绀珠传目录
4. 设置好后TML编译后会自动复制到游戏目录下
5. 可以直接在VS里调试

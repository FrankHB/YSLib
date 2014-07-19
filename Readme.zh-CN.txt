 YSLib 项目自述文件

> 项目组成概要
====
 YSLib 项目包括 YBase 和 YFramework 两个库，以及测试项目 YSTest 和其它一些小工具。
 YBase 提供一些 C++ 语言特性封装和公用设施的基本支持。 YFramework 是跨平台的框架，可用于开发本机 GUI 程序。
当前支持的平台有 DS 和 MinGW32 。

> 根目录结构和文件说明
====
以下只包括一级目录和少数特定的二级目录说明。
/.git
 Git 目录（可通过 hg-git 同步）
/.hg
 Mercurial 目录
/3rdparty
第三方依赖项
/Data
最终用户环境中部署的数据目录
/build
默认生成文件目录
/doc
开发文档
/doc/vsd
架构示意 Visio 文档
/Tools
开发使用的（辅助）工具
/YBase
顶级子项目 YBase
/YFramework
顶级子项目 YFramework
/YSTest/Android
示例项目 YReader 的 Android 平台项目（未完成正式支持）
/YSTest/DS_ARM7
示例项目 YReader 的 ARM7 项目（仅用于 DS 平台，用于生成 ARM7 ELF 二进制映像）
/YSTest/DS_ARM9
示例项目 YReader 的 ARM9 项目（包括 DS 平台 ARM9 部分，于生成 ARM9 ELF 二进制映像；源代码和其它平台共享）
/YSTest/DS
示例项目 YReader 的 DS 平台可执行文件项目（仅用于 DS 平台，用于生成 NDS 文件）
/YSTest/MinGW32
示例项目 YReader 的 MinGW32 平台可执行文件项目（仅用于 MinGW32 平台，用于生成 EXE 文件）
/.hgignore
 Mercurial 忽略文件
/.hgtags
 Mercurial 标签文件
/CC BY-SA 3.0 legalcode.txt
文档许可证： Creative Commons Legal Code Attribution-ShareAlike 3.0 Unported
/Doxyfile
 Doxygen 配置文件
/FTL.TXT
许可证： The FreeType Project LICENSE
/gpl-2.0.txt
许可证： GNU GENERAL PUBLIC LICENSE Version 2
/gpl-3.0.txt
许可证： GNU GENERAL PUBLIC LICENSE Version 3
/LICENSE.txt
整体许可证
/LICENSE_HISTORY.txt
整体许可证：历史版本
/license-fi.txt
许可证： FreeImage Public License - Version 1.0
/Readme.zh-CN.txt
本自述文件
/YSTest.sln
 Visual Studio 解决方案文件
/YSTest.workspace
 Code::Blocks 工作空间文件

> 构建说明
====
构建平台/宿主平台： Windows 或类 UNIX 系统。
目标平台： DS 和 MinGW32 。非正式支持 Android 。
详细参见 doc/Designation.txt @5.1 。
按 doc/Dependencies.txt @1 下载搭建目标平台对应的开发环境。
使用 Microsoft Visual Studio 2012 或以上版本打开 .sln （仅 Windows ）；
使用 Code::Blocks 12.11 或以上版本打开 .workspace 。
注意：
打开解决方案/工作空间后直接生成会失败，因为 .sln 和 .workspace 中并非所有项目都保证能生成，项目名没有平台后缀的不用于构建，仅作为项目原型便于开发；
需要生成特定平台后缀的项目，如 Win32 上的可执行文件对应 YSTest_MinGW32 ；
当前 Win32 依赖 Code::Blocs 内建构建工具支持，其它平台使用 makefile ，因此 Visual Studio 解决方案不支持 Win32 平台配置生成。

 DS
==
使用 devkitPro 至少需要以下环境变量（具体路径仅为示例，下同）：
DEVKITARM = /F/devkitPro/devkitARM
DEVKITPRO = /F/devkitPro
使用 Microsoft Visual Studio 打开文件，还需要以下环境变量以保证路径解析正确：
DKP_HOME = F:\devkitPro
需要保证有可用的 UNIX shell ； Windows 下可能需要另外加入 devkitPro 安装目录下 msys/bin 至 %PATH% 。可使用 MSys2 替代。

 Android
==
未正式支持。
开发环境配置详见 doc/Dependencies.txt @1.6 。

运行时依赖
==
 MinGW32 平台需要特定版本的 MinGW GCC 运行库，未在发布版本中打包。
需确认 doc/Dependencies.txt @1.5.2 描述的对应版本的构建依赖。若缺少 DLL ，可在 MinGW32 GCC 发行版其中的 bin 目录下找到。
特别注意，应当使用对应的正确的线程模型（使用 POSIX 而不是 Win32 ，后者没有实现标准库的线程支持）以及异常模型（ Dwarf2 和 SjLj 之一，特定构建版本相关）。
配置文件 yconf.txt 在当前工作目录下生成。使用 CHRLib 转换 GBK 编码的程序若找不到 cp113.bin 可能会异常退出，需要在 yconf.txt 中指定 DataDirectory 路径，或复制 /Data 下的文件到程序所在的目录（因为默认 DataDirectory 为当前目录）。

其它
==
详细的其它选项参照 https://bitbucket.org/FrankHB/yslib/wiki/Home 。

> YSLib PreAlpha 更新说明
====
2014-05-30(PreAlpha 5)
更改输出目录结构：目标文件和源代码分离。
添加 MinGW.org 支持（不用于发布）。
添加非正式的 Android 支持（ YBase 外未完成）。
更新 Doxygen 文档忽略列表，启用短文件名设置以避免含有制表符的文件名导致生成失败。
[YBase]
添加 Microsoft Visual Studio C++ 2013 支持。
[YFramework]
添加 MIME 类型和扩展名映射。
修整光栅化接口和实现，修复未显示右下边界线段和矩形右下角的点。
限制部件渲染区域保证绘制区域被渲染一次而正确显示。
调整消息宏命名风格。
添加基于界面无效化的简单逐帧动画支持。
添加多页面图像读取支持（基于 FreeImage 独立实现）。
添加 MRU 缓存并应用于字形缓存的实现。
在包含中使用模块标识宏代替文件名的直接使用。
添加 MinGW.org 运行时支持。
按钮和单选框增加悬停控件状态并修改对应状态的外观。
单选框禁用时渲染为灰色外观。
添加视觉样式支持。
添加图像缓冲区转换和保存为文件。
未启用或隐藏的部件不响应键盘焦点。
添加级联焦点缓冲（通过多级容器和部件的焦点确定键盘焦点）。
添加本机按键编码分类和相关操作；支持更多虚拟按键。
添加 TextBox 和 TabControl 控件。
增强日志输出接口。
[MinGW32]
修复终止时栈溢出。
本机窗口增加消息映射。
[YReader]
更改部分按键。
添加界面风格设置。
更改项目目录结构。
添加界面动画、文本框和标签页测试。
阅读器设置界面使用标签页。
====
2013-10-12(build 449)
 build 449 起使用 2-Clause BSD 许可证风格的条款进行基本授权。
[Config]
外部依赖库移至 "/3rdparty" ，其中 FreeType 需要在头文件中显式依赖。（需要对显式依赖项指定搜索包含路径 "/3rdparty/include" 。）
[YBase]
增加容器适配器和序列容器适配器模板。
增加路径类模板 ystdex::path 。
[YFramework]
用户 Shell 可以设置延时；
使用 MinGW 的 ::dirent 和 ::opendir 等。
 BufferedRenderer 支持更换缓冲区。
一些命名调整，便于配合标准库使用（ Swap → swap ）以及和 YBase 保持一致性（ Clone → clone ）。
调整 YCLib 文件系统接口。
重新设计 Path 类（使用ystdex::path）。
清理 YSLib 文件系统接口。
[MinGW] COM通用代码（需要自行增加链接库）。
修复非 255 阶灰度字符位图（包括矢量字体的内嵌位图）渲染错误。
增加图像读取支持（使用 FreeImage 实现）。
不使用 FreeType Cache Subsystem 重新实现字体缓存（减少大小并提高加载效率）。
支持渲染粗体和斜体字形位图。
 TouchEventArgs 重命名为 CursorEventArgs 。
 TouchMove 事件合并至 TouchHeld 事件。原逻辑固定。
支持新的 UI 事件 CursorOver 和 CursorWheel （在 DS 无法自动产生但可以由用户程序触发）。
[MinGW] 修复 GUI 响应坐标转换。
按键绑定自动生成复合事件，不需要单独绑定 KeyPress 到 Click 。
滚动条和列表框控件支持滚轮事件。
增加 DS 屏幕切换功能。
[MinGW] 允许设置顶层窗口透明性或使用 Alpha 位图。
增加简单的动态界面加载。
增加像素格式模板。
扩展 Alpha 组合算法为模板。
增加用于部件调整大小的边框（支持宿主窗口）。
修复圆光栅化算法实现。
增加填充圆绘制。
事件处理器支持省略末尾部分参数的可调用类型。
 NPLA1 配置增加读写多个未命名节点项。
[YReader]
更改界面：缩减背景，增大文件选择区域。
====
2013-04-24(PreAlpha 4)
更明确的许可证说明；
显式导出符号的 DLL （便于直接比较事件处理器函数指针等）。
 YBase 中增加以下内容：
变参模版和一些元编程接口；
 libdefect，当前仅修正 (MinGW32) libstdc++ 4.8 以下字符串函数（ std::stoi 等）缺失；
 YTest，当前仅测试计时；
 C/C++ 标准库 I/O 参数转换；
一些实现支持的特性（ C++11 关键字等）判断和替代宏；
 constexpr 字符串操作；
动态泛型值 any 类和动态泛型输入迭代器 any_input_iterator ；
 nonmovable 类。
 YFramework 中增加以下内容：
按钮色调；
值节点ValueNode类；
部分 YCLib 接口；
 NPL（用于配置的语言的简单实现）；
默认框架配置文件；
部件遍历迭代器；
 GUI 子部件接口；
菜单部件映射；
 MinGW32 简单宿主窗口控制（传递输入、改变宿主窗口大小、宿主窗口客户区为目标的部件渲染器）；
支持指针设备传递按键状态（区分是否有键同时按下、右键等）；
 GUI 应用程序和 Shell 基类。
 YFramework 中移除以下内容：
部件透明性（直接修改背景代替）。
 YFramework 中修正以下内容：
文本渲染边距计算；
 MinGW32 下的一些线程安全问题；
若干资源泄漏。
 YFramework 中修改以下内容：
改进值对象ValueObject类的可扩展性；
单选框外观；
消息实现、输入分发、主消息循环性能优化；
测试界面（增加关于窗口）；
文本渲染剪切接口和算法优化；
使用不可转移和复制的 GUI 状态；
用户界面命名空间： YSLib::Components → YSLib::UI ；
 Core 不依赖 Desktop 类。
 YReader 改进：
增加保存设置配置；
修正那个设置颜色改变后取消无效；
修改设置中上屏幕直接显示背景色；
修正设置下屏幕背景色后按钮角落遗漏重绘；
 MinGW32 下文本阅读界面增加右键显示上下文菜单；
增加书签界面；
增加书签保存和读取。
====
2012-07-10
更改了 CHRLib 载入非 Unicode 编码（目前只有 GBK ）的方式，由纯静态链接改为数据从路径 /Data 中的编码表文件读入。当前 GBK 对应的编码表文件为 cp113.bin 。
需要在初始化时保证载入正确，否则可能引起断言失败（ debug 配置下）或未定义行为（ release 配置下）。
====
2012-04-12(PreAlpha 3)
增加 MinGW 动态链接库。
增加配置文件生成，文件名为 config.txt ，编码为 UTF-8 （默认生成的有 BOM ，不过读取的不限，反正没 BOM 全 ASCII 按 ANSI 读也一样，有非 ASCII 字符一般能自动猜出来……）。
当前配置文件内容只有三行路径，依次为程序工作目录（初始化完毕后getcwd()取得的目录），矢量字体路径（会优先读取此路径的字体文件），矢量字体目录（会读取此目录下所有字体文件）。路径长度不超过 80 字节（多了会被截断）。
默认配置内容硬编码于 YSLib/Helper/Initialization.cpp 。
====
2012-04-09
完善框架实现：
基础架构和核心抽象实现调整；
增加各种语言层次上的优化；
增加高精度计时器；
消息类型调整和消息队列实现；
增加事件类型优先级支持；
微调块传输贴图算法；
部分完成 MVC 分离；
增加允许在运行时调整渲染策略的部件渲染支持；
增加基于无效矩形算法实现的部分渲染……
完善示例程序：
 release 版本对于某些字体来说效率上可以基本适应实际需要，但仍然无法使用带有位图的字体（显示错误，疑似 freetype 的 bug ）。
文本阅读器修正若干文件读取和布局渲染 bug （重新实现缓冲算法）；增加菜单和设置界面；增加进度显示和随机定位功能；增加背景色和文本色调整、字体调整、字体大小调整；增加阅读历史记录（前进/后退）；增加自动（可选以文本或像素行作为单位）滚屏；增加编码自动识别。
增加十六进制浏览功能。
主界面增加可选的 FPS 显示（伪；实际上是消息循环速率）。
增加 IDE 支持：
可使用 Code::Blocks 。
增加生成平台：
 Build 299 起支持 MinGW(g++4.7.0) 编译生成 Win32 本机程序。 MinGW 下载： http://code.google.com/p/mingw-builds 。
使用 Code::Blocks 打开工作空间文件直接生成（暂不支持 Visual Studio 生成）。
其它注意事项：
已确认 DeSmuMe SVN r4030 起有 bug 打开 .nds 后无法正确读取文件。
配置文件默认路径仍然是硬编码的（见YSLib/Helper/Initialization），根目录为 "H:\\NDS\\EFSRoot" ，所以运行的时候可能有些麻烦……
本机程序←→×之类的无法显示（大概也是 freetype 问题），于是有些按钮没文本看起来是空的……= =
====
2011-04-11(PreAlpha 2)
 PreAlpha 2(Build 200) 发布。
维护地址： http://code.google.com/p/yslib  。源码可以在那边下载。
界面完善了点。其它么……没什么对最终用户而言特别突出的特性。当作阅读器而言仍然很不完善，书签啥的都没有。唯一一个显著的进步是效率（界面响应在 release 版本下比较流畅）。此外就是细节上（但实现起来比较麻烦）的东西了，像是连续响应输入之类。
代价……重构掉的代码少说也是之前的 5 倍。
增加的特性除了以上内容外，主要是只对于开发而言有用的：
自定义宏（缩减代码长度用，否则光是写getter就会疯掉）；
多播委托模板；
事件映射表（只是用来压缩接口个数……）；
字符光栅化模板和渲染器（我真的只是重复发明轮子没刻意山寨M$- -，即使类名类似也不要脑补有相同的功能）；
依赖事件（同上）；
事件路由（同上）。
注释文档可以用 Doxygen 编译（我用的好像是 1.7.2 ）。
还是集成测试代替单元测试，所以版本号不表示实际编译次数。 Build 是全编译版本号，每次预定平均增量编译64次。
测试基本完全语句覆盖，不过应该还有很多 bug 。
====
2011-04-09
优化选项相关 bug 修复。
发现新的 DeSmuMe 的 release 应该两个月前可以用了，不过我仍然用更新的SVN版……
====
2011-03-20
发现优化 -O0 以上除了 -Os 外出问题（莫名其妙就控件状态就乱掉了），照着 gnu.gcc.org 上的Manual上的优化选项一个个试了，居然全部没问题……
更orz的是 -Os 加上对照 -Os 禁用的选项后文件体积还缩小了 1K 。
暂时先这样，继续原因不明好了……
====
2011-01-21
补遗：使用新版本 DeSmuME 可以运行。因为我不时更新SVN版，所以没很长时间没注意到 devkitPro 官网上说的最近的libnds无法在最新的 DeSmuMe 的 release 版（0.9.6；这个版本还有关于 FAT 访问的 bug ）中运行……
顺便删了 makefile 里面的 DLDI patch 命令行（反正模拟器会自动打补丁）。
====
2011-01-19
为了继续减少重复编译的工作量，继续把 ARM9 和 ARM7 目标文件也拆分出来改成项目了。
结果发现因为懒得写makefile项目之间的依赖性（似乎也确实不怎么容易 include ，路径变量什么的都是一堆export= =），结果反而导致在更新被依赖的库的时候导致生成的.nds不是最新版本，因为 ARM9/ARM7 的目标文件都是最新的……
需要手动右键->仅用于项目->仅清除...把 ARM9 的目标文件清理掉（其实也可以写个批处理什么的，不过这里的文件不多，重复编译花不了多少时间，于是又懒得搞了(_ _)）才可以保证生成最新的 .nds 。
另外 VS 偶尔会脑残地把配置 debug 当成 Debug （注意这里区分大小写）导致编译失败（不一定失败，只是makefile里面定义了 NDEBUG 宏，即使编译成功也会很麻烦，像莫名其妙多出来的 memory leaks 检测结果等等）……关掉VS重新打开解决方案应该就行了。
当然自己手动分别 make 应该是最保险的，只是我也懒的搞……= =...最简单的可以直接重新生成解决方案然后就可以一边去打灰机钓妹子（以下略）之类，等个大约5分钟就可以编译好了（我的笔记本上）。
====
2011-01-16
为了减少重复编译的工作量，把几个自制的库改成 VS2010 项目了。
清理项目删除不掉VS生成的 .log ……嘛，无视。
====
2010-10-27
改了一下配置，不过因为我不会改内置宏，也不会写 makefile ，项目配置基本成了摆设。于是 Debug/Release 也就是装潢门面， .nds 得到 Release 目录找， Release 连个 NDEBUG 都没有，还要手动定义……
====
2010-10-09
补 Loki 的 MIT License ，orz...
====
2010-09-29
迁移至 Visual Studio 2010 。
自动转换解决方案和工程文件（保留了旧版本）。
环境变量 NDSEmulator 去除引号，例如 F:\devkitPro\PALib\emulators\DeSmuME\DeSmuME.exe 。
现在的调试命令行是类似这样的：$(NDSEmulator) $(ProjectDir)$(NMakeOutput) --cflash-path=H:\NDS\efsroot。
之后就可以正常编译了。
====
2010-05-09(PreAlpha 1 @ yayabo.cn)
意义不明的框架库和附属品，半成品，对于开发人员以外完全没有价值。包含了框架库的测试程序，不过似乎完全无法体现设计目标= =...
嗯，基本上就是能用 TTF 字体，然后一些显示简单界面焦点响应什么的一个玩意……理论上来说做好了能完秒 MoonShell ，不过实际又是另外一回事了……

工程名字什么的……是随机取的。如有雷同纯属巧合。
库文件组织，代码风格，为什么用 VS 等请勿吐槽。
本体基本上是平台无关的，不过程序初始化部分还是保留了和一些 DS 相关的东西。
源码很大部分抄自 VNDS ，不过除了一些没删掉的注释和空行以外，基本上每行都修改过。
测试程序中，除了本体以外的是支持用的库。Freetype 改用了新版本结果发现编译出来的文件很大只能用旧的.a文件， AGG 移植好了发现不实用（姑且保留）、 Loki 太深奥所以基本上没用到，感觉灰常失败o.o。。。
其它部分都是自己写的（当然，参考了下载到的一些资源）。
文档基本属于摆设。

efsroot是放外部文件的（以前用到 EFS ，名字也懒得改了）。
efsroot\Font\下需要自己往里面放 TTF 文件（可以多于一个）。
剩下的除了文档以外是一个完整的 VS2008 工程。当然，可以手动 make 。

用到了 DLDIrc ，不需要的话在 Makefile 里面找下面的路径改掉：
@D:/Progra~2/DLDIrc/dlditool.exe "D:\Program Files (x86)\DLDIrc\drivers\mpcf.dldi" $(OUTPUT).nds。
另外，Makefile 是基于 devkitPro 的，没装的话当然编译不了。 devkitPro 我用的是最新的（对应devkitARM r30） ，旧的 Makefile 应该有点不一样，需要手动修改（具体哪些地方我忘了- -...）。不过规模不大，新写一个应该也不会太困难。
话说工程是用 VS2008 配合 PALibAppWizard 创建的，后来移除了 PALib 的部分，不知道还需不需要……
我用到的一些环境变量：
DEVKITARM = /F/devkitPro/devkitARM
DEVKITPPC = /F/devkitPro/devkitPPC
DEVKITPRO = /F/devkitPro
DEVKITPSP = /F/devkitPro/devkitPSP
DKP_HOME = F:\devkitPro
NDSEmulator = "F:\devkitPro\PALib\emulators\DeSmuME\DeSmuME.exe"

用 DeSmuME 调试命令行看起来是这样：
$(NDSEmulator) $(TargetPath) --cflash-path=H:\NDS\efsroot

使用了一些外部的库，虽然都可以不用GNU GPL的，不过保留在注释的一部分 VNDS 代码没删干净，不得不用GPL发布。

感想：伟大的雷叔在程序群里曾经教导过说，在 DS 上面写界面库是愚蠢的行为。果然如此。所以我决定把愚蠢贯彻到底（虽然有点微妙的变化= =），这样好歹对自己随口挖的坑有个交待……


# ccscripter_cpp
[Very WIP, DO NOT USE ME] my ccscripter cpp/qt port by AI (Codebuddy CN, may be minimax or deepseek-v3) 

## TODO
* 最开始我是想手工移植的, 先把插桩函数的定义声明全部搭起来, 里面的实现代码全部留空, 然后再慢慢填实现代码, 参考: CCScripter_cpp_v9.7z
* 但我后来没有这样做, 我另外用ai转换objc代码到c++(实际是qt), 转换的m代码可以参考: testobjc_codebuddy.7z
* 如果可以, 我觉得可以重新用AI转换objc代码到qt, 基于testobjc_codebuddy.7z里面的m文件
* 或者参考原版代码: CCScripter-0.8.24.tar.gz
* 另外, 我还尝试用xcode编译它的objc代码(去掉里面的析构代码, 改成用ARC自动引用计数方式编译, 但无法运行), 但没有放在这个仓库里面(不推荐用这个代码转换语言): Tukuyomi_v3_build_success.zip
* 这个仓库的代码的代码基是这个: testobjc_compare_v5.7z

## weibo record
```
上星期整理资料发现远古代码CCScripter（类似onscripter但只支持运行于旧的osx），
里面的解释器竟然是用yacc做的，
和onscripter截然不同，不过我以前也有类似的想法（不知道有没有人也这样想，我去查查），
只不过我是用antlr工具测试的，
估计没做出来，我可能考虑把这份代码开源算了，说不定这个方向其实也是行得通的

今天开始研究CCScripter，打算移植到C++（原版是objc），虽然目前只研究了很少一部分代码。
因为这份代码是基于bison和flex的，
所以我不是从图形界面开始研究，而是只研究yacc那部分代码，可以简单地编译一下，但链接是不行的。
原版似乎也没有做纯命令行的解释器，我可能要自己改成纯命令行解释器，
这样可以不用移植图形界面也能预览到效果——当然还有一种做法是不移植，
直接拿去macos下编译，可以有时间试试，但不确定能否编译成功（可能可以编译，
但也有可能编译不了，因为代码是很久以前的objc）

尝试用intel版的xcode打开运行CCScripter的工程文件，提示说不可以，不能运行power pc架构，
所以我放弃了。虽然xcode也支持cocoa框架，不过我感觉没那么容易就能迁移过去，
肯定会有很多问题，所以我就没试了，等以后我了解清楚再试（也许永远都不会试，
而是移植到c++和linux就算了）。当然这不是唯一的情况，如果打开onscripter的osx工程或者kurokoge，
通常都是打不开或者编译失败的，如果做不到我也不会硬着来，反正也都是很久以前的代码了——
反正都不是我写的，我当它们不存在

ccscripter研究。现在我可以把代码塞进新的xcode工程中用xcode编译（改了很久，改成新的objc写法，
ARC自动引用计数），然而并没用，还是跑不起来，继续研究——至于移植到C++，还没头绪 

ccscripter研究。好像是这样看的，有一个文件叫info.plist（或者看project.pbxproj），
里面有一个数值NSMainNibFile是MainMenu，然后找到Japanese.lproj/MainMenu.nib/classes.nib，
里面绑定了TYMainController和TYVisualNovelView这两个入口类 ​​​

ccscripter研究。想试试把xcode nib跑通，没成功，放弃，可能等以后再试，或者等移植完C++后。发现几个xcode技巧
（1）添加代码文件夹时下方选项选择创建节点可以在xcode里面创建多个文件夹，然后右键排序一下即可
（2）如果自己写的控制器类不是继承ViewController，也可以绑定到nib，方法是右上角加号添加Object，
然后绑定到类即可，实际上AppDelegate也是一个类似的Object，它也不是继承自ViewController，
但它也可以起到控制器的作用（3）如果需要绑定控制器类的outlet，需要先绑定控制器类到nib，
然后在nib编辑界面的右边属性栏中把outlet拉到想要绑定的界面元素上——也就是说只能在界面编辑器中
才能修改outlet（IBOutlet成员对象）的绑定对象

用qemu模拟powerpc版mac os x tiger运行ccscripter的效果。我不能编译，因为我没装xcode；
也不是我编译的，是原作者编译打包到源代码压缩包里面；模拟器也不是我装的是，是来源于其他人提供的，
我通过iso文件把文件传进去虚拟机运行。没有其他特别的功能，可以当作是类似onscripter
的程序但实现方法不同，而且已经不维护了

我打算用openclaw来转换objc到c++，用于ccscripter的移植（我之前做了一部分工作，但进度很慢，
而且我的进度可以用openclaw的工作代替，我之前做的可以白做）。除此以外，
我还测试过把Lua源代码的c语言头文件和源文件可以转换成c井，只是很小的代码量。
我打算尝试大代码量的转换，用于ccscripter的移植，但是否能成功还不清楚

我换了另外一个ai编程工具，不是claude code，用codebuddy去转换ccscripter的objc代码，
转成qt c++，好像这个工具会容易用一点（虽然中途停止了，但可以跟ai说继续执行），
等我有时间试试能不能编译成功 ​​​

ccscripter研究。其实我还说不定这个研究会继续下去。上次我用ai编程工具尝试把objc转成qt c++，
转换是成功了，不过我对比过原来的代码，其实还是有少数几个文件没转换成功，
而很多文件是缺少一些函数的（如果文件较少的话则不会缺少）。
我正在对比内容和转成UTF8BOM。简单说，就是还不能编译，我在想办法手工弄成一比一的代码，
所以距离能编译还有很长的路要走——即使能编译了，也很难跑起来——其实我觉得我研究好qt5的玩法，
可能比这个研究本身更好玩更有用

ccscripter研究。我现在真的想用qt5写galgame框架，基于ccscripter和ai编程工具
（qt4就算了，qt4可能更适合于嵌入式Linux）。主要我用过qt5做过公司项目，
现在回想起来qt5确实有很多类和objc是很契合的（也有可能qt的架构设计参考了objc的窗口类，
当然只是我猜测的）。不过现在做不出，如果按照现在的进度，估计最快也要下一年，
现在是连编译都没编译成功

ccscripter研究。今天正式开始objc和c++的代码对齐工作，目标是要可以用cmake和qt5
把这个工程完整编译出来，不需要运行成功。其实还有相当多的代码没有转换成c++，
所以接下来我会对单独的文件局部用ai工具转换成c++，但大部分代码都需要我手工对齐，
因为ai会跳过一些代码不转换或者改一些变量名，我喜欢自己逐行手工核对是否转换c++成功，
顺便会调整一些代码次序，例如把构造函数内联到头文件中

tukineko pygame研究。ai生成的代码，多出来的部分似乎也不是凭空出现的，
有一些类名好像是来源于github的代码，甚至我怀疑有一些是根据输入内容生成的代码，
如果ai能文档化这些内容就好，可惜它也不会文档化，或者它只是在生成代码的过程中说一下，
但没有保存到文档中​​​​

尝试降低难度，上次把ccscripter的objc转换成qt5，今天把tukineko的c井转换成qt5，
结果——还是不行，编译不通过，因为对qt5不熟，所以暂时还是不改了，等以后再玩
（或者等以后在Linux下转换，看能不能用ai自动修复编译错误）

tukineko pygame移植。改不下去（懒），我打算这几天把代码开源到gh上继续改
（反正没有游戏文件你们也跑不了），因为这代码稍微能运行一下（但bug无限多）。
除此以外我还有ccscripter qt版和tukineko qt版，不过这俩的进度更慢，
前者最快要下一年，后者最快要年底，所以这俩应该要等年底才会开源。
当然我的长远计划是另外做tukineko的新改良版，用于替代nscripter和onscripter，
但至少要先跑通tukineko的原版功能
```

## Qt5 for Windows 11
* qt-opensource-windows-x86-mingw492-5.6.1-1.exe
* (not need qt-creator-opensource-windows-x86-4.0.0.exe)
* **NOTE:** Qt 5.6.1 is too old,
* May cause incompatibility between the code and the latest Qt5,   
* but I like to use this version, Be Careful !!!  

## Qt5 for Xubuntu 20.04
* sudo apt install qt5-default
​​
## Qt5 for Xubuntu 25.04
* sudo apt install qtbase5-dev
* ====
* https://www.thundercomm.com/rubik-pi-3/cn/docs/rubik-pi-3-user-manual/1.1.2/qt5-user-guide
* sudo apt install build-essential  
* sudo apt install qtcreator  
* sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools qtbase5-examples qtbase5-doc-html  
* (??? not need qt5-doc ????)

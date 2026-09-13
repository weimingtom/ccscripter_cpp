# ccscripter_cpp
[Very WIP, DO NOT USE ME] my ccscripter cpp/qt port by AI (Codebuddy CN, may be minimax or deepseek-v3) 

## TODO
* 最开始我是想手工移植的, 先把插桩函数的定义声明全部搭起来, 里面的实现代码全部留空, 然后再慢慢填实现代码, 参考: CCScripter_cpp_v9.7z
* 但我后来没有这样做, 我另外用ai转换objc代码到c++(实际是qt), 转换的m代码可以参考: testobjc_codebuddy.7z
* 如果可以, 我觉得可以重新用AI转换objc代码到qt, 基于testobjc_codebuddy.7z里面的m文件
* 或者参考原版代码: CCScripter-0.8.24.tar.gz
* 另外, 我还尝试用xcode编译它的objc代码(去掉里面的析构代码, 改成用ARC自动引用计数方式编译, 但无法运行), 但没有放在这个仓库里面(不推荐用这个代码转换语言): Tukuyomi_v3_build_success.zip
* 这个仓库的代码的代码基是这个: testobjc_compare_v5.7z

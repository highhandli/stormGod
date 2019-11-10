这个目录是代码服务端的实现部分，根据 os.h 的宏来定义代码所处的操作系统，现在支持ubuntu 64 和 win7 64，代码默认是 win7 64。
定义了宏SYSTEM_WINDOWS_64表示需要在win7 64 编译，需要用到 tdm 中的gcc。
定义了宏SYSTEM_UBUNTU表示需要在ubuntu 64 编译，需要用到gcc
这两个宏只能定义其中一个，如果两个都定义了，默认是win7 64

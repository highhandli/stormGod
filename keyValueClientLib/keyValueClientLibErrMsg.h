#ifdef SUCCESS
#undef
#endif
#define SUCCESS 0

#define MALLOC_ERRMSG_1            "申请数量超出最大值"
#define MALLOC_ERRMSG_2            "申请内存失败"

#define MALLOC_ERRCODE_1           -101
#define MALLOC_ERRCODE_2           -102


#define CLIENT_SHORT_ERRMSG_1      "短连接，windows初始化tcp动态库失败"
#define CLIENT_SHORT_ERRMSG_2      "短连接，IP地址不符合规范"
#define CLIENT_SHORT_ERRMSG_3      "短连接，端口不符合规范"
#define CLIENT_SHORT_ERRMSG_4      "短连接，内存地址不是通过提供的函数申请"
#define CLIENT_SHORT_ERRMSG_5      "短连接，缺少操作类型"
#define CLIENT_SHORT_ERRMSG_6      "短连接，缺少要处理的数据"
#define CLIENT_SHORT_ERRMSG_7      "短连接，操作类型不存在"
#define CLIENT_SHORT_ERRMSG_8      "短连接，缺少要处理的数据"
#define CLIENT_SHORT_ERRMSG_9      "短连接，创建SOCKET或者连接到服务端失败"
#define CLIENT_SHORT_ERRMSG_10     "短连接，操作类型不存在"

#define CLIENT_SHORT_ERRCODE_1     -201
#define CLIENT_SHORT_ERRCODE_2     -202
#define CLIENT_SHORT_ERRCODE_3     -203
#define CLIENT_SHORT_ERRCODE_4     -204
#define CLIENT_SHORT_ERRCODE_5     -205
#define CLIENT_SHORT_ERRCODE_6     -206
#define CLIENT_SHORT_ERRCODE_7     -207
#define CLIENT_SHORT_ERRCODE_8     -208
#define CLIENT_SHORT_ERRCODE_9     -209
#define CLIENT_SHORT_ERRCODE_10    -210


#define UPDATE_SINGLE_ERRMSG_1     "更新操作，缺少key和value"
#define UPDATE_SINGLE_ERRMSG_2     "更新操作，缺少key"
#define UPDATE_SINGLE_ERRMSG_3     "更新操作，key过长"
#define UPDATE_SINGLE_ERRMSG_4     "更新操作，缺少value"
#define UPDATE_SINGLE_ERRMSG_5     "更新操作，value过长"
#define UPDATE_SINGLE_ERRMSG_6     "更新操作，发送数据到服务端失败"
#define UPDATE_SINGLE_ERRMSG_7     "更新操作，发送数据到服务端失败"
#define UPDATE_SINGLE_ERRMSG_8     "更新操作，等待接收数据失败"
#define UPDATE_SINGLE_ERRMSG_9     "更新操作，接收数据失败"
#define UPDATE_SINGLE_ERRMSG_10    "更新操作，申请内存失败"

#define UPDATE_SINGLE_ERRCODE_1    -301
#define UPDATE_SINGLE_ERRCODE_2    -302
#define UPDATE_SINGLE_ERRCODE_3    -303
#define UPDATE_SINGLE_ERRCODE_4    -304
#define UPDATE_SINGLE_ERRCODE_5    -305
#define UPDATE_SINGLE_ERRCODE_6    -306
#define UPDATE_SINGLE_ERRCODE_7    -307
#define UPDATE_SINGLE_ERRCODE_8    -308
#define UPDATE_SINGLE_ERRCODE_9    -309
#define UPDATE_SINGLE_ERRCODE_10   -310


#define DELETE_SINGLE_ERRMSG_1     "删除操作，缺少key"
#define DELETE_SINGLE_ERRMSG_2     "删除操作，key过长"
#define DELETE_SINGLE_ERRMSG_3     "删除操作，发送数据到服务端失败"
#define DELETE_SINGLE_ERRMSG_4     "删除操作，发送数据到服务端失败"
#define DELETE_SINGLE_ERRMSG_5     "删除操作，等待接收数据失败"
#define DELETE_SINGLE_ERRMSG_6     "删除操作，接收数据失败"
#define DELETE_SINGLE_ERRMSG_7     "删除操作，申请内存失败"

#define DELETE_SINGLE_ERRCODE_1    -401
#define DELETE_SINGLE_ERRCODE_2    -402
#define DELETE_SINGLE_ERRCODE_3    -403
#define DELETE_SINGLE_ERRCODE_4    -404
#define DELETE_SINGLE_ERRCODE_5    -405
#define DELETE_SINGLE_ERRCODE_6    -406
#define DELETE_SINGLE_ERRCODE_7    -407

#define SELECT_SINGLE_ERRMSG_1     "查询操作，缺少key"
#define SELECT_SINGLE_ERRMSG_2     "查询操作，key过长"
#define SELECT_SINGLE_ERRMSG_3     "查询操作，发送数据到服务端失败"
#define SELECT_SINGLE_ERRMSG_4     "查询操作，发送数据到服务端失败"
#define SELECT_SINGLE_ERRMSG_5     "查询操作，等待接收数据失败"
#define SELECT_SINGLE_ERRMSG_6     "查询操作，接收数据失败"
#define SELECT_SINGLE_ERRMSG_7     "查询操作，接收数据失败"
#define SELECT_SINGLE_ERRMSG_8     "查询操作，申请内存失败"

#define SELECT_SINGLE_ERRCODE_1    -501
#define SELECT_SINGLE_ERRCODE_2    -502
#define SELECT_SINGLE_ERRCODE_3    -503
#define SELECT_SINGLE_ERRCODE_4    -504
#define SELECT_SINGLE_ERRCODE_5    -505
#define SELECT_SINGLE_ERRCODE_6    -506
#define SELECT_SINGLE_ERRCODE_7    -507
#define SELECT_SINGLE_ERRCODE_8    -508


#define UPDATE_BAT_ERRMSG_1        "批量更新操作，缺少key和value"
#define UPDATE_BAT_ERRMSG_2        "批量更新操作，申请内存失败"
#define UPDATE_BAT_ERRMSG_3        "批量更新操作，缺少key"
#define UPDATE_BAT_ERRMSG_4        "批量更新操作，key过长"
#define UPDATE_BAT_ERRMSG_5        "批量更新操作，缺少value"
#define UPDATE_BAT_ERRMSG_6        "批量更新操作，value过长"
#define UPDATE_BAT_ERRMSG_7        "批量更新操作，申请内存失败"
#define UPDATE_BAT_ERRMSG_8        "批量更新操作，申请内存失败"
#define UPDATE_BAT_ERRMSG_9        "批量更新操作，发送数据到服务端失败"
#define UPDATE_BAT_ERRMSG_10       "批量更新操作，发送数据到服务端失败"
#define UPDATE_BAT_ERRMSG_11       "批量更新操作，等待接收数据失败"
#define UPDATE_BAT_ERRMSG_12       "批量更新操作，接收数据失败"
#define UPDATE_BAT_ERRMSG_13       "批量更新操作，申请内存失败"
#define UPDATE_BAT_ERRMSG_14       "批量更新操作，接收数据失败"

#define UPDATE_BAT_ERRCODE_1       -601
#define UPDATE_BAT_ERRCODE_2       -602
#define UPDATE_BAT_ERRCODE_3       -603
#define UPDATE_BAT_ERRCODE_4       -604
#define UPDATE_BAT_ERRCODE_5       -605
#define UPDATE_BAT_ERRCODE_6       -606
#define UPDATE_BAT_ERRCODE_7       -607
#define UPDATE_BAT_ERRCODE_8       -608
#define UPDATE_BAT_ERRCODE_9       -609
#define UPDATE_BAT_ERRCODE_10      -610
#define UPDATE_BAT_ERRCODE_11      -611
#define UPDATE_BAT_ERRCODE_12      -612
#define UPDATE_BAT_ERRCODE_13      -613
#define UPDATE_BAT_ERRCODE_14      -614


#define DELETE_BAT_ERRMSG_1        "批量删除操作，缺少key"
#define DELETE_BAT_ERRMSG_2        "批量删除操作，申请内存失败"
#define DELETE_BAT_ERRMSG_3        "批量删除操作，缺少key"
#define DELETE_BAT_ERRMSG_4        "批量删除操作，key过长"
#define DELETE_BAT_ERRMSG_5        "批量删除操作，申请内存失败"
#define DELETE_BAT_ERRMSG_6        "批量删除操作，发送数据到服务端失败"
#define DELETE_BAT_ERRMSG_7        "批量删除操作，发送数据到服务端失败"
#define DELETE_BAT_ERRMSG_8        "批量删除操作，等待接收数据失败"
#define DELETE_BAT_ERRMSG_9        "批量删除操作，接收数据失败"
#define DELETE_BAT_ERRMSG_10       "批量删除操作，申请内存失败"
#define DELETE_BAT_ERRMSG_11       "批量删除操作，接收数据失败"

#define DELETE_BAT_ERRCODE_1       -701
#define DELETE_BAT_ERRCODE_2       -702
#define DELETE_BAT_ERRCODE_3       -703
#define DELETE_BAT_ERRCODE_4       -704
#define DELETE_BAT_ERRCODE_5       -705
#define DELETE_BAT_ERRCODE_6       -706
#define DELETE_BAT_ERRCODE_7       -707
#define DELETE_BAT_ERRCODE_8       -708
#define DELETE_BAT_ERRCODE_9       -709
#define DELETE_BAT_ERRCODE_10      -710
#define DELETE_BAT_ERRCODE_11      -711


#define SELECT_BAT_ERRMSG_1        "批量查询操作，缺少key"
#define SELECT_BAT_ERRMSG_2        "批量查询操作，申请内存失败"
#define SELECT_BAT_ERRMSG_3        "批量查询操作，缺少key"
#define SELECT_BAT_ERRMSG_4        "批量查询操作，key过长"
#define SELECT_BAT_ERRMSG_5        "批量查询操作，申请内存失败"
#define SELECT_BAT_ERRMSG_6        "批量查询操作，发送数据到服务端失败"
#define SELECT_BAT_ERRMSG_7        "批量查询操作，发送数据到服务端失败"
#define SELECT_BAT_ERRMSG_8        "批量查询操作，等待接收数据失败"
#define SELECT_BAT_ERRMSG_9        "批量查询操作，接收数据失败"
#define SELECT_BAT_ERRMSG_10       "批量查询操作，申请内存失败"
#define SELECT_BAT_ERRMSG_11       "批量查询操作，接收数据失败"

#define SELECT_BAT_ERRCODE_1       -801
#define SELECT_BAT_ERRCODE_2       -802
#define SELECT_BAT_ERRCODE_3       -803
#define SELECT_BAT_ERRCODE_4       -804
#define SELECT_BAT_ERRCODE_5       -805
#define SELECT_BAT_ERRCODE_6       -806
#define SELECT_BAT_ERRCODE_7       -807
#define SELECT_BAT_ERRCODE_8       -808
#define SELECT_BAT_ERRCODE_9       -809
#define SELECT_BAT_ERRCODE_10      -810
#define SELECT_BAT_ERRCODE_11      -811


#define MIX_BAT_ERRMSG_1           "批量混合操作，缺少操作符"
#define MIX_BAT_ERRMSG_2           "批量混合操作，申请内存失败"
#define MIX_BAT_ERRMSG_3           "批量混合操作，缺少操作符"
#define MIX_BAT_ERRMSG_4           "批量混合操作，缺少操作符"
#define MIX_BAT_ERRMSG_5           "批量混合操作，更新，缺少key"
#define MIX_BAT_ERRMSG_6           "批量混合操作，更新，key过长"
#define MIX_BAT_ERRMSG_7           "批量混合操作，更新，缺少value"
#define MIX_BAT_ERRMSG_8           "批量混合操作，更新，value过长"
#define MIX_BAT_ERRMSG_9           "批量混合操作，更新，申请内存失败"
#define MIX_BAT_ERRMSG_10          "批量混合操作，删除，缺少key"
#define MIX_BAT_ERRMSG_11          "批量混合操作，删除，key过长"
#define MIX_BAT_ERRMSG_12          "批量混合操作，删除，申请内存失败"
#define MIX_BAT_ERRMSG_13          "批量混合操作，查询，缺少key"
#define MIX_BAT_ERRMSG_14          "批量混合操作，查询，key过长"
#define MIX_BAT_ERRMSG_15          "批量混合操作，查询，申请内存失败"
#define MIX_BAT_ERRMSG_16          "批量混合操作，操作符不存在"
#define MIX_BAT_ERRMSG_17          "批量混合操作，申请内存失败"
#define MIX_BAT_ERRMSG_18          "批量混合操作，发送数据到服务端失败"
#define MIX_BAT_ERRMSG_19          "批量混合操作，发送数据到服务端失败"
#define MIX_BAT_ERRMSG_20          "批量混合操作，等待接收数据失败"
#define MIX_BAT_ERRMSG_21          "批量混合操作，接收数据失败"
#define MIX_BAT_ERRMSG_22          "批量混合操作，申请内存失败"
#define MIX_BAT_ERRMSG_23          "批量混合操作，接收数据失败"

#define MIX_BAT_ERRCODE_1          -901
#define MIX_BAT_ERRCODE_2          -902
#define MIX_BAT_ERRCODE_3          -903
#define MIX_BAT_ERRCODE_4          -904
#define MIX_BAT_ERRCODE_5          -905
#define MIX_BAT_ERRCODE_6          -906
#define MIX_BAT_ERRCODE_7          -907
#define MIX_BAT_ERRCODE_8          -908
#define MIX_BAT_ERRCODE_9          -909
#define MIX_BAT_ERRCODE_10         -910
#define MIX_BAT_ERRCODE_11         -911
#define MIX_BAT_ERRCODE_12         -912
#define MIX_BAT_ERRCODE_13         -913
#define MIX_BAT_ERRCODE_14         -914
#define MIX_BAT_ERRCODE_15         -915
#define MIX_BAT_ERRCODE_16         -916
#define MIX_BAT_ERRCODE_17         -917
#define MIX_BAT_ERRCODE_18         -918
#define MIX_BAT_ERRCODE_19         -919
#define MIX_BAT_ERRCODE_20         -920
#define MIX_BAT_ERRCODE_21         -921
#define MIX_BAT_ERRCODE_22         -922
#define MIX_BAT_ERRCODE_23         -923

#define CLIENT_LONG_INTI_ERRMSG_1  "长连接初始化，IP地址不符合规范"
#define CLIENT_LONG_INTI_ERRMSG_2  "长连接初始化，端口不符合规范"
#define CLIENT_LONG_INTI_ERRMSG_3  "长连接初始化，长连接已初始化过"
#define CLIENT_LONG_INTI_ERRMSG_4  "长连接初始化，windows初始化tcp动态库失败"
#define CLIENT_LONG_INTI_ERRMSG_5  "长连接初始化，创建SOCKET或者连接到服务端失败"
#define CLIENT_LONG_INTI_ERRMSG_6  "长连接初始化，开启线程失败"


#define CLIENT_LONG_INTI_ERRCODE_1 -1001
#define CLIENT_LONG_INTI_ERRCODE_2 -1002
#define CLIENT_LONG_INTI_ERRCODE_3 -1003
#define CLIENT_LONG_INTI_ERRCODE_4 -1004
#define CLIENT_LONG_INTI_ERRCODE_5 -1005
#define CLIENT_LONG_INTI_ERRCODE_6 -1006

#define CLIENT_LONG_ERRMSG_1     "长连接，内存地址不是通过提供的函数申请"
#define CLIENT_LONG_ERRMSG_2     "长连接，线程ID不存在"
#define CLIENT_LONG_ERRMSG_3     "长连接，缺少操作类型"
#define CLIENT_LONG_ERRMSG_4     "长连接，缺少要处理的数据"
#define CLIENT_LONG_ERRMSG_5     "长连接，操作类型不存在"
#define CLIENT_LONG_ERRMSG_6     "长连接，缺少要处理的数据"
#define CLIENT_LONG_ERRMSG_7     "长连接，等待发送数据失败"
#define CLIENT_LONG_ERRMSG_8     "长连接，操作类型不存在"

#define CLIENT_LONG_ERRCODE_1    -1101
#define CLIENT_LONG_ERRCODE_2    -1102
#define CLIENT_LONG_ERRCODE_3    -1103
#define CLIENT_LONG_ERRCODE_4    -1104
#define CLIENT_LONG_ERRCODE_5    -1105
#define CLIENT_LONG_ERRCODE_6    -1106
#define CLIENT_LONG_ERRCODE_7    -1107
#define CLIENT_LONG_ERRCODE_8    -1108

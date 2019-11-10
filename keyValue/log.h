#define __LOG_H

#define DEBUG

#define LVSYS  9,__FILE__,__LINE__,__FUNCTION__,"server9.debug"    /* 系统信息日志级别   */
#define LVERR  8,__FILE__,__LINE__,__FUNCTION__,"server8.debug"    /* 错误信息日志级别   */
#define LVWAR  7,__FILE__,__LINE__,__FUNCTION__,"server7.debug"    /* 警告信息日志级别   */
#define LVNOR  6,__FILE__,__LINE__,__FUNCTION__,"server6.debug"    /* 正常信息日志级别   */
#define LVBUG  5,__FILE__,__LINE__,__FUNCTION__,"server5.debug"    /* 除错信息日志级别   */
#define LVDET  4,__FILE__,__LINE__,__FUNCTION__,"server4.debug"    /* 详细信息日志级别   */

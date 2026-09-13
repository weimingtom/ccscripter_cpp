/*
 *  StopNSLog.h
 *  Tukuyomi
 *
 *  Created by toveta on Thu Oct 04 2001.
 *
 */

#ifndef StopNSLog_h
#define StopNSLog_h

static inline void Stop_the_NSLog(const char* format, ...)
{
    //(empty)
}

//#define UNFATAL_LOG_NO_OUTPUT

// 「致命的でないログの出力を抑止する」オプションが有効な場合、NSLog2を無効化する。
#if defined(UNFATAL_LOG_NO_OUTPUT)
    #define NSLog2(...) 1 ? ((void)0) : Stop_the_NSLog(...)
#else
    #define NSLog2 qDebug
#endif

#endif // StopNSLog_h

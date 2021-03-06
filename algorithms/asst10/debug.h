#ifndef _DEBUG_H_
#define _DEBUG_H_
/*
  Debugging constructs more useful than the standard assert macro.
*/
#if !(defined NDEBUG)
#define DEBUG_PRINTF(fmt,...)                   \
  fprintf(stderr,fmt,##__VA_ARGS__)
#define DEBUG_PRINTF_ONCE(fmt,...)              \
  ({static int printed;                         \
    if(!printed){                               \
      printed = 1;                              \
      fprintf(stderr,fmt,##__VA_ARGS__);        \
    };})
#define HERE_FMT(fmt,...)                               \
  fprintf(stderr,"file: %s, line: %d, function: %s\n" fmt, \
          __FILE__,__LINE__,__func__,##__VA_ARGS__)
#define HERE() HERE_FMT("")
#define HERE_STR(str) HERE_FMT("%s",str)

#define HERE_FMT_ONCE(fmt,...)                                  \
  ({static int printed;                                         \
    if(!printed){                                               \
      printed = 1;                                              \
      fprintf(stderr,"file: %s, line: %d, function: %s\n" fmt,  \
              __FILE__,__LINE__,__func__,##__VA_ARGS__);        \
    };})
#define HERE_ONCE() HERE_FMT_ONCE("")
#define HERE_STR_ONCE(str) HERE_FMT_ONCE("%s",str)

#define ORDINAL_SUFFIX(num)                     \
  ({char *suffix = "th";                        \
    if(num == 1){suffix = "st";}                \
    if(num == 2){suffix = "nd";}                \
    if(num == 3){suffix = "rd";};               \
    suffix;})

#define HERE_COUNTER()                                                  \
  ({static int counter = 1;                                             \
    fprintf(stderr,"%d%s time at file: %s, line: %d\n",                 \
            counter, ORDINAL_SUFFIX(counter), __FILE__, __LINE__);      \
    counter++;})
//I got these from the linux kernel
#define WARN(cond,fmt,...)                      \
  ({if(cond){                                   \
      HERE_FMT(fmt,##__VA_ARGS__);              \
    };})
#define WARN_ON(cond)                           \
  ({if(cond){                                   \
      HERE();                                   \
    };})
#define WARN_ONCE(cond, fmt, ...)               \
  ({static int warned;                          \
    if(cond){                                   \
      if(!warned){                              \
        HERE_FMT(fmt, ##__VA_ARGS__);           \
        warned = 1;                             \
      }                                         \
    };})
#define WARN_ON_ONCE(cond)                      \
  ({static int warned;                          \
    if(cond){                                   \
      if(!warned){                              \
        HERE();                                 \
        warned = 1;                             \
      }                                         \
    };})
//Extensions of the above
#define ERROR(cond,fmt,...)                     \
  ({if(cond){                                   \
      HERE_FMT(fmt,##__VA_ARGS__);              \
      exit(EXIT_FAILURE);                       \
    };})
#define ERROR_ON(cond)                          \
  ({if(cond){                                   \
      HERE();                                   \
      exit(EXIT_FAILURE);                       \
    };})
#define TRAP(cond, fmt, ...)                    \
  ({if(cond){                                   \
      HERE_FMT(fmt,##__VA_ARGS__);              \
      raise(SIGTRAP);                           \
    };})
#define TRAP_ON(cond)                           \
  ({if(cond){                                   \
      HERE();                                   \
      raise(SIGTRAP);                           \
    };})
#define BREAKPOINT() raise(SIGTRAP)
//we don't need to include this if debugging is disabled
/*#include <execinfo.h>
static void print_backtrace(int __attribute__((unused)) signo){
  #define BACKTRACE_BUF_SIZE 128
  void *buffer[BACKTRACE_BUF_SIZE];
  int stack_entries = backtrace(buffer, BACKTRACE_BUF_SIZE);
  //just write the backtrace straight to stderr
  backtrace_symbols_fd(buffer, stack_entries, STDERR_FILENO);
}
static __attribute__((unused)) void enable_backtraces(void){
  struct sigaction act;
  act.sa_handler = print_backtrace;
  act.sa_flags = SA_RESETHAND;
  sigaction(SIGSEGV, &act, NULL);
  sigaction(SIGABRT, &act, NULL);
  sigaction(SIGINT, &act, NULL);
  }*/
#else
#define DEBUG_PRINTF(fmt,...)
#define HERE()
#define HERE_FMT(fmt,...)
#define HERE_STR(str)
#define HERE_COUNTER()
#define WARN_ON(...)
#define WARN_ON_ONCE(...)
#define WARN(...)
#define WARN_ONCE(...)
#define ERROR(...)
#define ERROR_ON(...)
#define TRAP(...)
#define TRAP_ON(...)
#define BREAKPOINT()

#endif
#endif //defined _DEBUG_H_

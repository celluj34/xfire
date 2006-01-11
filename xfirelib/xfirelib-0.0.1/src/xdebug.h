

#ifndef __XDEBUG_H
#define __XDEBUG_H

#define RESET 0

#define BLACK  0
#define RED    1
#define GREEN  2
#define YELLOW 3
#define WHITE  7
#include <stdio.h>
  #define XDEBUG(args) { \
    printf( "[0;33;40m" ); \
    printf( " XFireLibDEBUG(%25s,%4d): ", __FILE__, __LINE__ ); \
    printf args ; \
    printf( "[0;37;40m" ); \
  }
  #define XINFO(args)  { \
    printf( "[1;32;40m" ); \
    printf( " XFireLibINFO (%25s,%4d): ", __FILE__, __LINE__ ); \
    printf args ; \
    printf( "[0;37;40m" ); \
  }
  #define XERROR(args) { \
    printf( "[1;31;40m" ); \
    printf( " XFireLibERROR(%25s,%4d): ", __FILE__, __LINE__ ); \
    printf args ; \
    printf( "[0;37;40m" ); \
  }
#ifndef XENABLEDEBUG
  #undef XDEBUG
  #define XDEBUG(args)
  #ifndef XINFO
    #define XINFO(args)
  #endif
#endif


#endif


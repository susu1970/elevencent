#ifndef GLOBAL_H_
#define GLOBAL_H_
#include<QScreen>
#include<QGuiApplication>
namespace qt_elevencent{
#define SCREEN_RECT QGuiApplication::primaryScreen()->geometry()
#define SCREEN_W SCREEN_RECT.width()
#define SCREEN_H SCREEN_RECT.height()
#define MAX(a,b) ((a)>=(b)?(a):(b))
#define MIN(a,b) ((a)<=(b)?(a):(b))

  
}

#endif

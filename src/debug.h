/*
* Debug.h
* Permet de gérer les messages de debug
*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG_ON

#ifdef DEBUG_ON
   #define DEBUG_INIT    Serial.begin(115200);
   #define DEBUG(...)    Serial.print(__VA_ARGS__)
   #define DEBUGLN(...)  Serial.println(__VA_ARGS__)
#else
   #define DEBUG(...)
   #define DEBUGLN(...)
   #define DEBUG_INIT    
#endif

#endif

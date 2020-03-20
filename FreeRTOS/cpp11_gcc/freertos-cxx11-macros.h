#pragma once

#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 8
#define configUSE_RECURSIVE_MUTEXES 1 // [SCO]

#define pdMS_TO_TICKS( xTimeInMs ) \
     ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * \
     ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )

#ifndef pdTICKS_TO_MS
#define pdTICKS_TO_MS(ticks) \
  ((((long long)(ticks)) * (configTICK_RATE_HZ)) / 1000)
#endif

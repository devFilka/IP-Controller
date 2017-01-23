#ifndef TIME_H
#define TIME_H

/* Arduino header */
#include "Arduino.h"

/* Time constants */
#define MAX_MILLIS 4294967295

/* Time functions */
bool is_time_end(uint32_t t, uint32_t begin, uint32_t end);

#endif

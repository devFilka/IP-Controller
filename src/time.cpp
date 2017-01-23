#include "time.h"

bool is_time_end(uint32_t t, uint32_t begin, uint32_t end) {
  if( end >= begin ){
    if( (end-begin) >= t ){
      return true;
    }
    else {
      return false;
    }
  }
  else {
    if( (MAX_MILLIS-begin+end) >= t ){
      return true;
    }
    else {
      return false;
    }
  }
}

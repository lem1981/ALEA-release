/*
* FIXME:
* For Odroid-XU3 it should be /dev/i2c-3
* For Odroid-XU  it should be /dev/i2c-4
*/
#define INA231_DEV_NAME "/dev/i2c-3" 
#define INA231_DEV_NAME_SIZE 20

#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>

#define LSB_UA(x) x*1e6/32768

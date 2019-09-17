#include "alea.h"
#include "alea_common.h"
#include "i2c-dev.h"
#include "ina231.h"

static int       arm_power_cont;
static int       ina231_dev = 0;

void init_ina231()
{
   char filename[INA231_DEV_NAME_SIZE];

   snprintf(filename, INA231_DEV_NAME_SIZE-1, INA231_DEV_NAME);

   ina231_dev = open(filename, O_RDWR);
   if (ina231_dev < 0)
   {
      ERROR("Can't open INA231 \n");
   }

   if ( ioctl(ina231_dev, I2C_SLAVE_FORCE, 0x0040) < 0)
   {
      ERROR("Can't init INA 231");
   }

   if ( i2c_smbus_write_word_data( ina231_dev, 0x00, 0x0740) != 0)
   {
      ERROR("Can't write init info inti INA231");
   }
}/* init_ina231  */

void start_counter_ina231()
{
   /* write_val(file, &a15, INA231_REG_MASK, 0x4) */
   if ( ioctl(ina231_dev, I2C_SLAVE_FORCE, 0x0040) < 0)
   {
      ERROR("Can't init INA 231");
   }

   if ( i2c_smbus_write_word_data( ina231_dev, 0x06, 0x4) != 0)
   {
      ERROR("Can't write init info inti INA231");
   }

   /* write_val(file, &a15, INA231_REG_CONFIG, 0x0340)*/
   if ( ioctl(ina231_dev, I2C_SLAVE_FORCE, 0x0040) < 0)
   {
      ERROR("Can't init INA 231");
   }

   /* Set power sensing period */

   /* 408 ms*/
   /*0x4B40*/

   /* 664 ms*/
   /* 9340 */

   /* 1178 ms*/
   /* DB40 */

   /* 0.140 ms*/
   /* 0x0340*/
   if ( i2c_smbus_write_word_data( ina231_dev, 0x00, 0x0340) != 0)
   {
      ERROR("Can't write init info inti INA231");
   }
}

void power_meters_destructor()
{
   close(ina231_dev);
}

long long read_power_from_ina231()
{
   long long power;

   if( ioctl( ina231_dev, I2C_SLAVE_FORCE, 0x0040)<0)
   {
      ERROR("Can't read data from ");
   }

   __s32 res = i2c_smbus_read_word_data(ina231_dev, 0x03);
   if(res < 0)
   {
      ERROR("Can't read data from bus");
   }

   char *p = (char *)&res;
   res = ((p[0] << 8) | p[1]) & 0xFFFF;

   power = (long long) (1000000.0 *(res*25.0*LSB_UA(9))/1e6);
   return power;
}

int is_ina231_results_ready()
{
   int ready_status;

   if( ioctl( ina231_dev, I2C_SLAVE_FORCE, 0x0040)<0)
   {
      perror("Can't read data from ");
      exit(1);
   }

   __s32 res = i2c_smbus_read_word_data(ina231_dev, 0x06);
   if(res < 0)
   {
      ERROR("Can't read data from bus");
   }

   char *p = (char *)&res;
   ready_status = (int)(((p[0] << 8) | p[1]) & 0xFFFF);

   if ( ready_status == 1032)
   {
      return 1;
   }

   return 0;
}

float read_power_from_ina231_float()
{
   while( is_ina231_results_ready() == 0 )
   {
   }

   if( ioctl( ina231_dev, I2C_SLAVE_FORCE, 0x0040)<0)
   {
      ERROR("Can't read data from ");
   }

   __s32 res = i2c_smbus_read_word_data(ina231_dev, 0x03);
   if(res < 0)
   {
      ERROR("Can't read data from bus");
   }

   char *p = (char *)&res;
   res = ((p[0] << 8) | p[1]) & 0xFFFF;

   return ((float) (res*25.0*LSB_UA(9))/1e6);
}

double alea_get_power()
{
   struct timespec start_sample_time, stop_sample_time;
   double power, time;

   if ( ina231_dev == 0)
   {
      init_ina231();
   }

   /* Set powe averaging period */
   clock_gettime(CLOCK_MONOTONIC, &start_sample_time);

   start_counter_ina231();
   power = read_power_from_ina231_float();

   DEBUG("Sampled power %f\n", power);

   clock_gettime(CLOCK_MONOTONIC, &stop_sample_time);

   time = timediff(start_sample_time,stop_sample_time);

   DEBUG("Time %f\n", time);

   return (power);
}/* alea_get_power */


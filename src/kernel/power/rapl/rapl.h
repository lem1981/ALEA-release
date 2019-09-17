#define MSR_RAPL_POWER_UNIT             0x606

#define MSR_PP0_ENERGY_STATUS           0x639

#define POWER_UNIT_OFFSET       0
#define POWER_UNIT_MASK         0x0F

#define MSR_RAPL_MAX_CNT        65536

#define MSR_FILE "/dev/cpu/%d/msr"

#define RAPL_DEF_SAMPLE_POWER_PERIOD 1000000

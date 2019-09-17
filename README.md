ALEA profiler v.0.1.
--------------------------------------------------
ALEA is a cross-platform energy profiling tool.

For problem reports and suggestions on the implementation,
please contact:

   L.mukhanov@qub.ac.uk

--------------------
System requirements:
--------------------
 - Intel X86 processors with RAPL support
 - ARM based Odroid-XU or Odroid-XU3 platforms (with integrated INA231 power sensors)
 - Linux
 - bash
 - python(any version)
 - GNU Binutils

-------------------
Platforms settings:
-------------------
 ALEA supports energy profiling of several CPUs with shared memory.
 To take energy measurements for several CPUs the following environmental
 variables should be set up:
 ALEA_CPU_NUM - number of CPUs
 ALEA_CORE_NUM - number of cores per CPU

For example, to take energy measurements for 4 CPUs with 12 cores
per CPU the following variable should be exported:
export ALEA_CPU_NUM=4
export ALEA_CORE_NUM=12

-------------
How to build?
-------------
1. Enter the source directory:
   cd alea-profiler/src
2. Build sources:
   ./configure; make; make install

-----------
How to run?
-----------
alea_prof application

------------------------------------
How to see the results of profiling?
------------------------------------
alea_build_stat application
File "alea_build_stat" should be run in the same directory where "alea_prof" was run

------------------------------------------
How to interpret the results of profiling?
------------------------------------------
```
Basic blocks                                              Samples    Time,Sec       Energy,J          Power,W     
-----------------------------------------------------------------------------------------------------------------
sys                                                       229          2.85          224.11          78.55
bb96 bb96 bb96 bb96 bb96 bb96 bb96 bb96                   167          2.08          397.93          191.26
bb102 bb102 bb102 bb102 bb102 bb102 bb102 bb102           137          1.71          302.21          177.06
bb121                                                     61           0.76          64.19          84.46
sys sys sys sys sys sys sys sys                           37           0.46          43.93          95.30
bb145                                                     18           0.22          21.50          95.88
bb96 sys bb96 sys sys sys sys sys                         5            0.06          7.43          119.28
bb116                                                     3            0.04          2.51          67.11
bb138 sys sys sys sys sys sys sys                         3            0.04          2.86          76.47
bb208                                                     3            0.04          4.07          108.84
bb96 sys bb96 sys bb96 sys bb96 bb96                      1            0.01          2.49          199.66
bb6 sys sys sys sys sys sys sys                           1            0.01          1.50          120.28
bb122                                                     1            0.01          1.20          96.18
bb96 sys bb96 sys bb96 sys sys sys                        1            0.01          1.85          148.47
bb96 sys bb96 sys bb96 sys sys bb96                       1            0.01          2.14          172.15

Total energy 1079.90197922
Time 8.322158

In this table bb96 corresponds to the basic block with id 96 of a profiled application, sys corresponds to a basic block
from a dynamic library called by the application.
```
-------------------------------------------------------
What does a combination of "bb96 bb96 bb96 bb96 bb96 bb96 bb96 bb96" mean?
-------------------------------------------------------
It means that bb96 was executed(during 2.08 Seconds) in parallel by 8 threads at the same time.

---------------------------------------------------------------------------
How to get instructions of a basic block and associate it with source code?
---------------------------------------------------------------------------
For example, if this information should be retrieved for bb121 of  backprop application then the
following command should be executed:

alea_bb_info 121 backprop

Results:
Basic block info:
-----------------
First instruction:
backprop.c:99
Last instruction:
backprop.c:99 (discriminator 2)
-----------------
Basic block body:
  401045:	31 db                	xor    ebx,ebx
  401047:	66 0f 1f 84 00 00 00    nop    WORD PTR [rax+rax*1+0x0]
  40104e:	00 00 
  401050:	4b 8b 04 f7          	mov    rax,QWORD PTR [r15+r14*8]
  401054:	48 8d 2c 98          	lea    rbp,[rax+rbx*4]
  401058:	48 83 c3 01          	add    rbx,0x1
  40105c:	e8 1f fb ff ff          call   400b80 <rand@plt>
  401061:	f3 0f 2a c0          	cvtsi2ss xmm0,eax
  401065:	41 39 dc                cmp    r12d,ebx
  401068:	f3 0f 59 05 70 10 00    mulss  xmm0,DWORD PTR [rip+0x1070]      # 4020e0 <__dso_handle+0x198>
  40106f:	00 
  401070:	f3 0f 11 45 00          movss  DWORD PTR [rbp+0x0],xmm0
  401075:	7d d9                	jge    401050 <bpnn_randomize_weights+0x30>


#!/usr/bin/python
import sys
import math
import os

# FIXME: WE NEED TO USE NUMPY
addrs = {}
power = {}
clocks = {}
cores_id = {}
 
num_samples = 0
num = 0
system = 0
sys_power = 0
procedure_level=0
power_variance = {}
per_core_addr={}
per_core_power={}
per_core_power_variance={}

total_energy = 0
use_core_id = 0
per_core_presentation = 0

myHash={}

arch=str(sys.argv[2])

param_num = 2
while param_num < len(sys.argv):
  cmd_line = sys.argv[param_num]
  if (cmd_line.find("-lproc") == 0):
     print "Procedure level enabled"
     procedure_level=1
  param_num = param_num + 1

asmfile = open(str(sys.argv[1])+"_diss", "r")

if arch == "intel":
  basic_block_num=1
  for line in asmfile:

    if  (line.find("0000") == 0):
        basic_block_num = basic_block_num + 1
        continue

    start_addr = line.find(":")
    if start_addr == 8:
      addr = line[2:8].translate(None, ' ')
      if len(addr) > 0:

         if (procedure_level == 1):
           myHash[addr]="pr"+str(basic_block_num)
         else:
	   myHash[addr]="bb"+str(basic_block_num)	  
           if  ((line.find("j") == 32)):
               basic_block_num = basic_block_num + 1

if arch == "arm":
  basic_block_num=1
  for line in asmfile:
    if  line.find("0") == 0:
        basic_block_num = basic_block_num + 1
        continue

    start_addr = line.find(":")
    if start_addr == 8:
      addr = line[0:8].translate(None, ' ')
      if len(addr) > 0:
         if (procedure_level == 1):
           myHash[addr]="pr"+str(basic_block_num)
         else:
           myHash[addr]="bb"+str(basic_block_num)

#         print  str(basic_block_num)  + " " + addr
         if  line.find("b") == 22 or line.find("b") == 21:
             basic_block_num = basic_block_num + 1

asmfile.close()


stat_addr = open("energy.prof", "r")
alpha = 0.01
z = 1.96 

time = 0

for line in stat_addr :

    start_line = line.find("Power")

    if start_line >= 0:
      line_dump = line[start_line:]

      info = line_dump.split(' ')
      id = 7
      full_addr =''
      last_bb_id = -1

      if (len(info) <= id):
         continue

      while id <= len(info):
         info_addr = info[id]
         info_addr = info_addr[6:].rstrip()

#         print  info[id]
         if info_addr in myHash:
            info_addr=myHash[info_addr]
	 else: 
	    info_addr="sys"
	  
         if per_core_presentation == 1:
            core_addr = str(info[id-1])+str(info_addr)
            if core_addr in per_core_addr:
               per_core_addr[core_addr]=per_core_addr[core_addr] + 1
            else:
               per_core_addr[core_addr] = 1

            if core_addr in per_core_power:
               per_core_power[core_addr]=per_core_power[core_addr] + float(info[1])
            else:
               per_core_power[core_addr]= float(info[1])

         if use_core_id == 1:
	    if info_addr not in full_addr:
               full_addr = str(full_addr) + str(info[id-1])+str(info_addr)+" "
#	       print full_addr
         else:
            id_bb = info_addr[2:]
            if info_addr == "sys":
#            if info_addr not in full_addr:
	       full_addr = str(full_addr) + str(info_addr)+" "
            elif int(id_bb) >= last_bb_id:
               full_addr = str(full_addr) + str(info_addr)+" "
               last_bb_id = int(id_bb)
            else:
               full_addr = str(info_addr) + " " + str(full_addr)

         id = id + 5

      if full_addr in addrs:
         addrs[full_addr]=addrs.get(full_addr) + 1 
      else:
         addrs[full_addr] = 1
      num_samples = num_samples + 1

      if full_addr in power:
         power[full_addr]=power.get(full_addr) + float(info[1])
      else:
         power[full_addr] = float(info[1])
    

    if line.find("Program time") >= 0:
      info = line.split(' ')
      time = float(info[2])

if time == 0:
  time = 100.0

stat_addr = open("energy.prof", "r")

for line in stat_addr :
   start_line = line.find("Power")

   if start_line >= 0:
      line_dump = line[start_line:]

      info = line_dump.split(' ')
      id = 7
      full_addr =''
      last_bb_id = -1

      if (len(info) <= id):
         continue
      while id <= len(info):
         info_addr = info[id]
         info_addr = info_addr[6:].rstrip()
         if info_addr in myHash:
            info_addr=myHash[info_addr]
	 else: 
	    info_addr="sys"
	
         if per_core_presentation == 1:
            core_addr = str(info[id-1])+str(info_addr)
             
            if core_addr in per_core_power_variance:
               per_core_power_variance[core_addr]=per_core_power_variance[core_addr]+pow((per_core_power[core_addr]/per_core_addr[core_addr] - float(info[1])),2)
            else:
               per_core_power_variance[core_addr]=pow((per_core_power[core_addr]/per_core_addr[core_addr] - float(info[1])),2)

         if use_core_id == 1:
		if info_addr not in full_addr:
            	   full_addr = str(full_addr) + str(info[id-1])+str(info_addr)+" "

         else:
            id_bb = info_addr[2:]
            if info_addr == "sys":
#		if info_addr not in full_addr:
               full_addr = str(full_addr) + str(info_addr)+" "
            elif int(id_bb) >= last_bb_id:
               full_addr = str(full_addr) + str(info_addr)+" "
               last_bb_id = int(id_bb)
            else:
               full_addr = str(info_addr) + " " + str(full_addr)

         id = id + 5

      if full_addr in power_variance:
         power_variance[full_addr]=power_variance[full_addr] + pow((power[full_addr]/addrs[full_addr] - float(info[1])),2)
      else:
         power_variance[full_addr]=pow((power[full_addr]/addrs[full_addr] - float(info[1])),2)

   if line.find("Program time") >= 0:
     info = line.split(' ')
     time = float(info[2])

if (int(os.getenv('ALEA_SHOW_CONFIDENCE', -1)) != -1):
 print "Confidence interval 95%"
print '{0:55}'.format("Basic blocks") + "   " + '{0:8}'.format("Samples") + "   " + '{0:12}'.format("Time,Sec") + "   " + '{0:15}'.format("Energy,J") + "   " + '{0:12}'.format("Power,W")
print "-----------------------------------------------------------------------------------------------------------------"

for addr in sorted(addrs, key=addrs.get, reverse=True):
           
#    prob = float(float(addrs[addr])/float(num_samples))
#    down_bound = (prob - z*pow(((prob*(1-prob))/float(num_samples)),0.5))* time
#    if down_bound < 0:
#       down_bound = 0
#    up_bound = (prob + z* pow(((prob*(1-prob))/float(num_samples)),0.5)) * time
#    avg_instr_power = power[addr]/addrs[addr]
#    print str(addr) + "\n Info:" + " " + str(addrs[addr])+ " "+ str(prob * time) + " " + " " + str(down_bound) + " " + str(up_bound) + " "+ str(avg_instr_power)+ " " +  str(prob * time* avg_instr_power) + " " + str(down_bound*avg_instr_power) + " " + str(up_bound*avg_instr_power)+"\n"

     prob = float(float(addrs[addr])/float(num_samples))
     prob_delta = z*pow(((prob*(1-prob))/float(num_samples)),0.5)
     down_bound = (prob - prob_delta)* time
     if down_bound < 0:
         down_bound = 0
     up_bound = (prob + prob_delta) * time
     avg_instr_power = power[addr]/addrs[addr]
     power_delta = z*pow(((power_variance[addr])/float(num_samples-1)),0.5)

     down_energy = down_bound * (avg_instr_power - power_delta)
     up_energy = up_bound * (avg_instr_power + power_delta)

#     print '{0:35}'.format(str(addr)) + " " + str(addrs[addr])+ " "+ "{0:.2f}".format(float(prob * time)) + " $\pm$ " + "{0:.2f}".format(float( prob_delta* time)) +  " & " +  "{0:.2f}".format(prob * time* avg_instr_power) + " $\pm$ " + "{0:.2f}".format(up_energy - prob * time* avg_instr_power)+"/" + "{0:.2f}".format(prob * time* avg_instr_power  - down_energy) +  " & "+ "{0:.2f}".format(avg_instr_power)+ " $\pm$ " + "{0:.2f}".format(power_delta) + " \\ "

     if (int(os.getenv('ALEA_SHOW_CONFIDENCE', -1)) != -1):
        print '{0:55}'.format(str(addr)) + "   " + '{0:8}'.format(str(addrs[addr]))+ "   "+ "{0:.2f}".format(float(prob * time)) + "+/-" + "{0:.2f}".format(float( prob_delta* time)) + "   " + "{0:.2f}".format(prob * time* avg_instr_power) + "+" + "{0:.2f}".format(up_energy - prob * time* avg_instr_power) + "/-" + "{0:.2f}".format(prob * time* avg_instr_power  - down_energy) + "   " + "{0:.2f}".format(avg_instr_power)+ "+/-" + "{0:.2f}".format(power_delta)
     else:
         print '{0:55}'.format(str(addr)) + '{0:3}'.format(" ") + '{0:8}'.format(str(addrs[addr]))+'{0:5}'.format(" ")+ "{0:.2f}".format(float(prob * time)) + '{0:10}'.format(" ") + "{0:.2f}".format(prob * time* avg_instr_power) + '{0:10}'.format(" ") + "{0:.2f}".format(avg_instr_power)
    
     total_energy = total_energy+prob * time* avg_instr_power

print "Total energy " + str(total_energy)
print "Time " + str(time)


#print "Per core statistic " 

#for addr in sorted(per_core_addr, key=per_core_addr.get, reverse=True):
#     prob = float(float(per_core_addr[addr])/float(num_samples))
#     prob_delta = z*pow(((prob*(1-prob))/float(num_samples)),0.5)
#     down_bound = (prob - prob_delta)* time
#     if down_bound < 0:
#         down_bound = 0
#     up_bound = (prob + prob_delta) * time
#     avg_instr_power = per_core_power[addr]/per_core_addr[addr]
#     power_delta = z*pow(((per_core_power_variance[addr])/float(num_samples-1)),0.5)

#     down_energy = down_bound * (avg_instr_power - power_delta)
#     up_energy = up_bound * (avg_instr_power + power_delta)

#     print '{0:35}'.format(str(addr)) + " " + str(per_core_addr[addr])+ " "+ str(prob * time) + " +/-" + str( prob_delta* time) + " "+ str(avg_instr_power)+ " +/-" + str(power_delta) + " " +  str(prob * time* avg_instr_power) + " +" + str(up_energy - prob * time* avg_instr_power) + "/-" + str(prob * time* avg_instr_power  - down_energy)

#     print '{0:35}'.format(str(addr)) + " " + str(per_core_addr[addr])+ " "+ "{0:.2f}".format(float(prob * time)) + " $\pm$ " + "{0:.2f}".format(float( prob_delta* time)) +  " & " +  "{0:.2f}".format(prob * time* avg_instr_power) + " $\pm$ " + "{0:.2f}".format(up_energy - prob * time* avg_instr_power)+"/" + "{0:.2f}".format(prob * time* avg_instr_power  - down_energy) +  " & "+ "{0:.2f}".format(avg_instr_power)+ " $\pm$ " + "{0:.2f}".format(power_delta) + " \\ "


stat_addr.close()

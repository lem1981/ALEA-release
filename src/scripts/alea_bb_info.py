#!/usr/bin/python
import sys
import math
import os

block_id = str(sys.argv[1])

number_of_block_instr = 0
found_first_addr = 0

block_body=''
first_addr=''
last_addr=''

asmfile = open(str(sys.argv[2])+"_diss", "r")
exec_file = str(sys.argv[2])

procedure_level=0
param_num = 3
while param_num < len(sys.argv):
  cmd_line = sys.argv[param_num]
  if (cmd_line.find("-lproc") == 0):
     print "Procedure level enabled"
     procedure_level=1
  param_num = param_num + 1


if 'ARM' in open('/proc/cpuinfo').read():
    arch='arm'
else:
    arch='intel'
    
basic_block_num=1

for line in asmfile:

    if int(block_id) == basic_block_num:
       block_body = block_body + str(line)
       if len(first_addr) == 0:
          first_addr = line[2:8].translate(None, ' ')
       last_addr = line[2:8].translate(None, ' ')

    if basic_block_num > block_id:
       break

    if  (line.find("0000") == 0):
        basic_block_num = basic_block_num + 1
        continue

    if arch == "intel":
       start_addr = line.find(":")
       if start_addr == 8:
          addr = line[2:8].translate(None, ' ')
          if len(addr) > 0:

             if (procedure_level == 0):
               if  ((line.find("j") == 32)):
                  basic_block_num = basic_block_num + 1

    if arch == "arm":
      if  line.find("0") == 0:
        basic_block_num = basic_block_num + 1
        continue
 
      start_addr = line.find(":")
      if start_addr == 8:
         addr = line[0:8].translate(None, ' ')
         if len(addr) > 0:
            if  line.find("b") == 22 or line.find("b") == 21:
                basic_block_num = basic_block_num + 1


asmfile.close()


print "Basic block info:"
print "-----------------"
print "First instruction:"
os.system( "addr2line " + str(first_addr.rstrip()+ " -e " + exec_file))
print "Last instruction:"
os.system( "addr2line " + str(last_addr.rstrip()+ " -e " + exec_file))
print "-----------------"
print "Basic block body:"
print block_body

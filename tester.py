#!/usr/bin/python
import argparse
import subprocess
import os
perfect=True
parser = argparse.ArgumentParser()

parser.add_argument("--img", help="img",
					action="store", default='example1.img')
parser.add_argument("--comp_to", help="correct output",
					action="store", default='mylog1.txt')					
parser.add_argument("--cyc", help="cycles",
					action="store", default=42)
args = parser.parse_args()


str1="./sim_main "+args.img+" " + str(args.cyc) + ' > tmp.txt'
print(str1)
os.system(str1)
f = open(args.comp_to)
img_lines = f.read().splitlines()
f.close()
b = open('tmp.txt','r')
out_lines = b.read().splitlines()
b.close()
if (len(img_lines) != len(out_lines)):
	print ('-----------------length mismatch----------------')
	perfect=False
for i,line in enumerate(img_lines):
	if out_lines[i] != line:
		print ("-----------------line "+ str(i+1)+" mismatch----------------" )
		perfect = False
if perfect:
	print ('-----success------')
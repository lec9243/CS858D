#!/usr/bin/env python3
import random

def genDNA(n):
	dna = ["A", "T", "C", "G"]
	st = random.choices(dna, k = n)
	st = "".join(st)
	return st	


s = genDNA(554658)
f = open("mytestDNA2.fa", "w")
f.write(s)
f.close()


#!/usr/bin/env python

import random # for seed, random
import sys    # for stdout
import copy


# Computes the score of the optimal alignment of two DNA strands.
def findOptimalAlignment(strand1, strand2,answerSet):
	if answerSet.get((strand1,strand2))!=None: return answerSet.get((strand1,strand2))
	
	# if one of the two strands is empty, then there is only
	# one possible alignment, and of course it's optimal
	if len(strand1) == 0: return len(strand2) * -2
	if len(strand2) == 0: return len(strand1) * -2

	# There's the scenario where the two leading bases of
	# each strand are forced to align, regardless of whether or not
	# they actually match.
	bestWith = findOptimalAlignment(strand1[1:], strand2[1:],answerSet)
	if strand1[0] == strand2[0]:
		answerSet[(strand1,strand2)]=bestWith + 1;
		return bestWith + 1 # no benefit from making other recursive calls

	best = bestWith - 1
	
	# It's possible that the leading base of strand1 best
	# matches not the leading base of strand2, but the one after it.
	bestWithout = findOptimalAlignment(strand1, strand2[1:],answerSet)
	bestWithout -= 2 # penalize for insertion of space
	if bestWithout > best:
		best = bestWithout

	# opposite scenario
	bestWithout = findOptimalAlignment(strand1[1:], strand2,answerSet)
	bestWithout -= 2 # penalize for insertion of space	
	if bestWithout > best:
		best = bestWithout
	answerSet[(strand1,strand2)]=best;
	return best

def buildOptimalAlignment(strand1, strand2,answerSet):
	if len(strand1) == 0: return len(strand2) * -2
	if len(strand2) == 0: return len(strand1) * -2
	bestWith = findOptimalAlignment(strand1[1:], strand2[1:],answerSet)
	if strand1[0] == strand2[0]:
		buildOptimalAlignment(strand1[1:], strand2[1:],answerSet)
		return	bestWith+1;
	best = bestWith-1;
	
	bestWithout2 = findOptimalAlignment(strand1, strand2[1:],answerSet)
	bestWithout2 -= 2 # penalize for insertion of space
	bestWithout1 = findOptimalAlignment(strand1[1:], strand2,answerSet)
	bestWithout1 -= 2 # penalize for insertion of space
	if(best>max(bestWithout1,bestWithout2)):
		buildOptimalAlignment(strand1[1:], strand2[1:],answerSet)
		return best;
	if(bestWithout1>bestWithout2):
		answerSet["strand2"]=answerSet["strand2"][0:-len(strand2)]+" "+strand2;
		buildOptimalAlignment(strand1[1:],strand2,answerSet);
		return bestWithout1;
	else:
		answerSet["strand1"]=answerSet["strand1"][0:-len(strand1)]+" "+strand1;
		buildOptimalAlignment(strand1,strand2[1:],answerSet);
		return bestWithout2;



	
# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]

def generateRandomDNAStrand(minlength, maxlength):
	assert minlength > 0, \
	       "Minimum length passed to generateRandomDNAStrand" \
	       "must be a positive number" # these \'s allow mult-line statements
	assert maxlength >= minlength, \
	       "Maximum length passed to generateRandomDNAStrand must be at " \
	       "as large as the specified minimum length"
	strand = ""
	length = random.choice(xrange(minlength, maxlength + 1))
	bases = ['A', 'T', 'G', 'C']
	for i in xrange(0, length):
		strand += random.choice(bases)
	return strand

# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.

def printAlignment(score, out = sys.stdout):	
	out.write("Optimal alignment score is " + str(score) + "\n")

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.
 
 
def getStrAligmentScord(strand1,strand2):
	if len(strand1) == 0: return len(strand2) * -2
	if len(strand2) == 0: return len(strand1) * -2
	
	scord=0;
	if(strand1[0]==" " or strand2[0]==" "): 
		scord-=2
	else: 
		if(strand1[0] == strand2[0] ): 
			scord+=1
		else: 
			scord -= 1
		
	return getStrAligmentScord(strand1[1:],strand2[1:])+scord

def main():
	
	while (True):
		sys.stdout.write("Generate random DNA strands? ")
		print "\n"
		#answer = sys.stdin.readline()
		#if answer == "no\n": break
		
		strand1 = generateRandomDNAStrand(265, 280)
		strand2 = generateRandomDNAStrand(265, 280)
		strResult1=copy.copy(strand1)
		strResult2=copy.copy(strand2)
		answerSet={"strand1":strResult1,"strand2":strResult2}
		sys.stdout.write("Aligning these two strands: " + strand1 + "\n")
		sys.stdout.write("                            " + strand2 + "\n")
		alignment = buildOptimalAlignment(strand1, strand2,answerSet);
		printAlignment(alignment)
		print "The two aligment is:\n\t"+answerSet["strand1"]+"\n\t"+answerSet["strand2"]+"\n" 
		print findOptimalAlignment(answerSet["strand1"].replace(" ",""),answerSet["strand2"].replace(" ",""),answerSet);
		print getStrAligmentScord(answerSet["strand1"],answerSet["strand2"])
		                          
		break;
		
if __name__ == "__main__":
  main()

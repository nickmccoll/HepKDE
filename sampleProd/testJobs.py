#!/usr/bin/python
import os
import sys
import re
import argparse
import subprocess
from ROOT import TFile, TTree
parser = argparse.ArgumentParser(description='Check output files')
parser.add_argument("-c", "--config", dest="config", default="runTestAnalyzer.py", help="Configuration file to be run using cmsRun to run. [Default: runTestAnalyzer.py]")
parser.add_argument("-s", "--submitFile", dest="submitFile", default="submitall.sh", help="Submision file [Default: submitall.sh]")
parser.add_argument("-j", "--jobdir", dest="jobdir", default="/Users/nmccoll/test/test2", help="Directory for job files. [Default: jobs]")
parser.add_argument("-v", "--verify", dest="verify", type=bool, default=True, help="Check that each file can be opened. [Default: True]")
parser.add_argument("-o", "--outdir", dest="outdir", default="/eos/uscms/store/user/${USER}/13TeV/processed", help="Output directory for ntuples. [Default: \"/eos/uscms/store/user/${USER}/13TeV/ntuples\"]")
args = parser.parse_args()

filelist =  [re.match(r'submit_(.*).sh', f).group(1) for f in os.listdir(args.jobdir) if (re.match(r'submit_(.*).sh', f))]

filesToDelete = []
filesToSubmit = []


for f in filelist:
	fileName = "{0}/job_{1}.root".format(args.outdir, f)
	if os.path.isfile(fileName):
		if args.verify:
			tfile = TFile(fileName, "read");
			if(tfile.IsOpen()):
				t = tfile.Get("Events")
				if not t :
					filesToSubmit.append(f)	
					filesToDelete.append(fileName)					
				tfile.Close()
			else :
				filesToSubmit.append(f)	
				filesToDelete.append(fileName)
	else :
		filesToSubmit.append(f)

print "{0} files with bad output that should be deleted:".format(len(filesToDelete))		
for f in filesToDelete:
	print "rm {0}".format(f)
	
subscript = open(args.submitFile, "w")	
subscript.write("""	
#!/bin/bash
workdir=$CMSSW_BASE
cp {cfg} $workdir
""".format(cfg=args.config))

print "{0} files that will be added to the resubmission file: {1}".format(len(filesToSubmit),args.submitFile)
for f in filesToSubmit:
	subscript.write("""
cp ./{jobdir}/files_{num}.txt $workdir
./{jobdir}/submit_{num}.sh
""".format(jobdir=args.jobdir,num=f)
	)
subscript.close()
os.system("chmod +x {0}".format(args.submitFile))
			
		
	

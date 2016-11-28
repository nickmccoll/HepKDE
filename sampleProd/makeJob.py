#!/usr/bin/python
import os
import sys
import re
import argparse
import subprocess

parser = argparse.ArgumentParser(description='Prepare and submit ntupling jobs')
parser.add_argument("-c", "--config", dest="config", default="runTestAnalyzer.py", help="Configuration file to be run using cmsRun to run. [Default: runTestAnalyzer.py]")
parser.add_argument("-i", "--inputdir", dest="inputdir", default="/eos/uscms/store/user/${USER}/13TeV/ntuples", help="Input dataset. [Default: runTestAnalyzer.py]")
parser.add_argument("-d", "--isDas", dest="isDas", default=True, help="Output directory for ntuples. [Default: \"/eos/uscms/store/user/${USER}/]")
parser.add_argument("-j", "--jobdir", dest="jobdir", default="jobs", help="Directory for job files. [Default: jobs]")
parser.add_argument("-nf", "--numFiles", dest="numfiles", type=int, default=5, help="Number of files per job. [Default: 5]")
parser.add_argument("-o", "--outdir", dest="outdir", default="/eos/uscms/store/user/${USER}/13TeV/processed", help="Output directory for ntuples. [Default: \"/eos/uscms/store/user/${USER}/13TeV/ntuples\"]")
args = parser.parse_args()

script = "runJob.sh"

os.system("mkdir -p %s" % args.jobdir)
os.system("mkdir -p %s/logs" % args.jobdir)
if args.outdir.startswith("/eos/cms/store/user") or args.outdir.startswith("/store/user") :
	os.system("%s mkdir -p %s" % (eos, args.outdir))
else :
	os.system("mkdir -p %s" % args.outdir)

if(args.isDas):
	cmd = ("das_client --query \"file dataset=%s instance=prod/global\" --limit=0" % (args.inputdir))
	ps = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	result = ps.communicate()
	filelist = result[0].rstrip('\n').split('\n')
	prefix = ""
else :
	filelist = [os.path.join(args.inputdir, f) for f in os.listdir(args.inputdir) if (re.match(r'(.*).root', f) or re.match(r'(.*).lhe', f))]
	if args.inputdir.startswith("/eos/uscms/store/user") :
		prefix = "root://cmseos:1094/"


nFiles = len(filelist)
iF = 0
iF2 = 0
iJ = 0
while iF < nFiles:
	if(iF2 == 0):
		jobfiles = open("{0}/files_{1}.txt".format(args.jobdir, iJ), "w")	
	if args.inputdir.startswith("/eos/uscms/store/user") :
		jobfiles.write("root://cmseos:1094/%s" % (filelist[iF]))		
	else :
		jobfiles.write(filelist[iF])		
	jobfiles.write("\n")
	iF2 += 1
	iF += 1
	if(iF2 == args.numfiles or  iF ==  nFiles):
		jobfiles.close()
		
		jobscript = open("{0}/submit_{1}.sh".format(args.jobdir,iJ), "w")
		jobscript.write("""
cat > submit.cmd <<EOF
universe                = vanilla
Requirements            = (Arch == "X86_64") && (OpSys == "LINUX")
request_disk            = 10000000
request_memory          = 5000
Executable              = {runscript}
Arguments               = {cfg} {outputdir} job_{num}.root {workdir} files_{num}.txt
Output                  = logs/gen_{num}.out
Error                   = logs/gen_{num}.err
Log                     = logs/gen_{num}.log
use_x509userproxy       = true
initialdir              = {jobdir}
Should_Transfer_Files   = YES
transfer_input_files    = {workdir}/{cfg},{workdir}/files_{num}.txt
WhenToTransferOutput    = ON_EXIT
Queue
EOF

condor_submit submit.cmd;
rm submit.cmd""".format(
			runscript=script, cfg=args.config, workdir="${CMSSW_BASE}", num=iJ, jobdir=args.jobdir, outputdir=args.outdir
			))
		jobscript.close()
		os.system("chmod +x %s/submit_%d.sh" % (args.jobdir, iJ))		
		iF2 = 0
		iJ += 1

subscript = open("submitall.sh", "w")	
subscript.write("""	
#!/bin/bash
workdir=$CMSSW_BASE
cp {cfg} $workdir
""".format(cfg=args.config))

for ijob in range(iJ) :
	subscript.write("""
cp ./{jobdir}/files_{num}.txt $workdir
./{jobdir}/submit_{num}.sh
""".format(jobdir=args.jobdir,num=ijob)
	)

subscript.close()
os.system("chmod +x submitall.sh")

#!/usr/bin/python
import os
import sys
import re
import argparse
import subprocess

parser = argparse.ArgumentParser(description='Prepare and submit ntupling jobs')
parser.add_argument("-c", "--config", dest="config", default="runTestAnalyzer.py", help="Configuration file to be run using cmsRun to run. [Default: runTestAnalyzer.py]")
parser.add_argument("-j", "--jobdir", dest="jobdir", default="jobs", help="Directory for job files. [Default: jobs]")
parser.add_argument("-nj", "--numjobs", dest="numjobs", type=int, default=5, help="Number of jobs. [Default: 5]")
parser.add_argument("-ne", "--numperjob", dest="numperjob", type=int, default=5, help="Number of events per job. [Default: 5]")
parser.add_argument("-o", "--outdir", dest="outdir", default="/eos/uscms/store/user/${USER}/13TeV/ntuples", help="Output directory for ntuples. [Default: \"/eos/uscms/store/user/${USER}/13TeV/ntuples\"]")
args = parser.parse_args()

script = "produceSample.sh"

os.system("mkdir -p %s" % args.jobdir)
os.system("mkdir -p %s/logs" % args.jobdir)
if args.outdir.startswith("/eos/cms/store/user") or args.outdir.startswith("/store/user") :
	os.system("%s mkdir -p %s" % (eos, args.outdir))
else :
	os.system("mkdir -p %s" % args.outdir)


for ijob in range(args.numjobs) :
	outfile = "gen_{0}.root".format(ijob)
	jobscript = open("{0}/submit_{1}.sh".format(args.jobdir, ijob), "w")
	jobscript.write("""
cat > submit.cmd <<EOF
universe                = vanilla
Requirements            = (Arch == "X86_64") && (OpSys == "LINUX")
request_disk            = 10000000
Executable              = {runscript}
Arguments               = {cfg} {outputdir} {outputname} {maxevents} {workdir}
Output                  = logs/gen_{num}.out
Error                   = logs/gen_{num}.err
Log                     = logs/gen_{num}.log
use_x509userproxy       = true
initialdir              = {jobdir}
Should_Transfer_Files   = YES
transfer_input_files    = {workdir}/{cfg}
WhenToTransferOutput    = ON_EXIT
Queue
EOF

condor_submit submit.cmd;
rm submit.cmd""".format(
			runscript=script, cfg=args.config, workdir="${CMSSW_BASE}", num=ijob, jobdir=args.jobdir, outputdir=args.outdir, outputname=outfile, maxevents=args.numperjob
			))
	jobscript.close()
	os.system("chmod +x %s/submit_%d.sh" % (args.jobdir, ijob))

subscript = open("submitall.sh", "w")	
subscript.write("""	
#!/bin/bash
workdir=$CMSSW_BASE
cp {cfg} $workdir
""".format(cfg=args.config))

for ijob in range(args.numjobs) :
	subscript.write("""
./{jobdir}/submit_{num}.sh
""".format(jobdir=args.jobdir,num=ijob)
	)

subscript.close()
os.system("chmod +x submitall.sh")

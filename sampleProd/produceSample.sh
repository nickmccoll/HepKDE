#!/bin/bash

cfgfile=$1
outputdir=$2
outputname=$3
maxevents=$4
scramdir=$5

workdir=`pwd`

echo `hostname`
echo "${_CONDOR_SCRATCH_DIR}"
echo "scramdir: $scramdir"
echo "workdir: $workdir"
echo "args: $*"
ls -l

source /cvmfs/cms.cern.ch/cmsset_default.sh
cd $scramdir/src/
SCRAM_ARCH=slc6_amd64_gcc491
eval `scramv1 runtime -sh`
cd $workdir

cp ${scramdir}/${cfgfile} .
cp ${scramdir}/${filename} .

cmsRun $cfgfile print outputFile=${outputname} nEvents=${maxevents}

status=`echo $?`
echo "Status = $status"

if [[ "$outputdir" =~ ^/eos/uscms/.* ]]; then
  copypath=`echo ${outputdir} | sed "s:/eos/uscms::"`
  xrdcp -np ${outputname} root://cmseos:1094/${copypath}/
  rm ${outputname}
else
  mv ${outputname} ${outputdir}/
fi

exit $status

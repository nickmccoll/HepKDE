#!/bin/bash

cfgfile=$1
outputdir=$2
outputname=$3
infiles=$4
CMSSWVERS=$5

workdir=`pwd`

echo `hostname`
echo "${_CONDOR_SCRATCH_DIR}"
echo "scramdir: $scramdir"
echo "workdir: $workdir"
echo "args: $*"
ls -l

source /cvmfs/cms.cern.ch/cmsset_default.sh
SCRAM_ARCH=slc6_amd64_gcc530
eval `scramv1 project CMSSW ${CMSSWVERS}`

tar -xf cmssw.tar.gz -C ${CMSSWVERS}/src --warning=no-timestamp
rm cmssw.tar.gz

cd ${CMSSWVERS}/src/
eval `scramv1 runtime -sh` # cmsenv is an alias not on the workers
scramv1 build
echo "CMSSW: "$CMSSW_BASE
cd $workdir

cmsRun $cfgfile print outputFile=${outputname} inputFiles_load=${infiles}

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

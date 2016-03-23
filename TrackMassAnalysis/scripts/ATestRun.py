#!/usr/bin/env python
import ROOT
ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")
ROOT.EL.Algorithm()
ROOT.xAOD.Init("ATestRun").ignore();
sh = ROOT.SH.SampleHandler()
ROOT.SH.ScanDir().filePattern('group.perf-idtracking.6448154.EXT0._000128.DOAD_TIDE.pool.root.2').scan(sh,'/global/project/projectdirs/atlas/btamadio/TrackMass/samples/group.perf-idtracking.361026.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ6W.DAOD_IDTIDE.e3569_s2608_s2183_r6765_v2_EXT0.42274410/')
sh.printContent()
job = ROOT.EL.Job()
job.sampleHandler(sh)
#job.options().setDouble(ROOT.EL.Job.optMaxEvents,500)
alg = ROOT.TrackSkimmer()
job.algsAdd(alg)
driver = ROOT.EL.DirectDriver()
driver.submit(job,'submitDir')

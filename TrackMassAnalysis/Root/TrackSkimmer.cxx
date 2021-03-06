#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TrackMassAnalysis/TrackSkimmer.h>
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include <AsgTools/MessageCheck.h>
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include <TFile.h>


// this is needed to distribute the algorithm to the workers
ClassImp(TrackSkimmer)



TrackSkimmer :: TrackSkimmer ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
}



EL::StatusCode TrackSkimmer :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.
  job.useXAOD();
  ANA_CHECK_SET_TYPE(EL::StatusCode);
  ANA_CHECK(xAOD::Init());
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackSkimmer :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  TFile *outputFile = wk()->getOutputFile(m_outputName);
  m_tree = new TTree ("tree", "tree");
  m_tree->SetDirectory (outputFile);
  m_tree->Branch("EventNumber", &b_eventNumber);
  m_tree->Branch("mcEventWeight",&b_mcEventWeight);
  m_tree->Branch("mcChannelNumber",&b_mcChannelNumber);
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackSkimmer :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackSkimmer :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackSkimmer :: initialize ()
{

  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.
  ANA_CHECK_SET_TYPE(EL::StatusCode);
  xAOD::TEvent* event = wk()->xaodEvent();
  Info("initialize()","Number of events = %lli",event->getEntries());
  m_eventCounter = 0;
  m_isMC = false;

  m_jetCleaning = new JetCleaningTool("JetCleaning");
  m_jetCleaning->msg().setLevel( MSG::DEBUG ); 
  ANA_CHECK(m_jetCleaning->setProperty( "CutLevel", "LooseBad"));
  ANA_CHECK(m_jetCleaning->setProperty("DoUgly", false));
  ANA_CHECK(m_jetCleaning->initialize());
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackSkimmer :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.
  ANA_CHECK_SET_TYPE(EL::StatusCode);
  xAOD::TEvent* event = wk()->xaodEvent();
  if( m_eventCounter % 100 == 0 ){
    Info("execute()","Event Number = %i",m_eventCounter);
  }
  m_eventCounter++;
  const xAOD::EventInfo* eventInfo = 0;
  ANA_CHECK(event->retrieve( eventInfo, "EventInfo"));  
  if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
       m_isMC = true; // can do something with this later
  }   
  b_eventNumber = eventInfo->eventNumber();
  b_mcChannelNumber = eventInfo->mcChannelNumber();
  b_mcEventWeight = eventInfo->mcEventWeight();
  int numGoodJets = 0;
  const xAOD::JetContainer* jets = 0;
  ANA_CHECK(event->retrieve( jets, "AntiKt4EMTopoJets" ));
  Info("execute()", "  number of jets = %lu", jets->size());
  // loop over the jets in the container
  xAOD::JetContainer::const_iterator jet_itr = jets->begin();
  xAOD::JetContainer::const_iterator jet_end = jets->end();
  for( ; jet_itr != jet_end; ++jet_itr ) {
       Info("execute()", "  jet pt = %.2f GeV", ((*jet_itr)->pt() * 0.001));
       if( !m_jetCleaning->accept( **jet_itr )) continue; //only keep good clean jets
       numGoodJets++;
  } // just to print out something
  Info("execute()","  number of good jets = %i",numGoodJets);
  m_tree->Fill();
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackSkimmer :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackSkimmer :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.
  ANA_CHECK_SET_TYPE(EL::StatusCode);
  xAOD::TEvent* event = wk()->xaodEvent();
  if( m_jetCleaning ) {
    delete m_jetCleaning;
    m_jetCleaning = 0;
  }
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackSkimmer :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  return EL::StatusCode::SUCCESS;
}

#ifndef TrackMassAnalysis_TrackSkimmer_H
#define TrackMassAnalysis_TrackSkimmer_H

#include <EventLoop/Algorithm.h>
#include "JetSelectorTools/JetCleaningTool.h"
#include <TTree.h>

class TrackSkimmer : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  // float cutValue;



  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
public:
  // Tree *myTree; //!
  // TH1 *myHist; //!
  int m_eventCounter; //!
  bool m_isMC; //!
  JetCleaningTool *m_jetCleaning;//!
  std::string m_outputName;
  TTree *m_tree; //!
  int b_eventNumber; //!
  int b_mcChannelNumber; //!
  double b_mcEventWeight; //!
  vector<double> *b_jet_pt; //!
  vector<double> *b_jet_eta; //!
  vector<double> *b_jet_phi; //!
  vector<double> *b_jet_e; //!

  vector<double> *b_track_pt; //!
  vector<double> *b_track_eta; //!
  vector<double> *b_track_phi; //!
  vector<double> *b_track_e; //!

  vector<double> *b_pseudotrack_pt; //!
  vector<double> *b_pseudotrack_eta; //!
  vector<double> *b_pseudotrack_phi; //!
  vector<double> *b_pseudotrack_e; //!

  // this is a standard constructor
  TrackSkimmer ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();
  void setOutputName(std::string name){ m_outputName = name; }
  // this is needed to distribute the algorithm to the workers
  ClassDef(TrackSkimmer, 1);
};

#endif

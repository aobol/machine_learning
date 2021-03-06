#include "TSystem.h"
#include "TChain.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TVector3.h"
#include "TImage.h"
#include <algorithm>

using namespace std;
using namespace CLHEP;
using namespace TMath;

//this file is a root script reading in the root tree generated by geant4 simulation,
//then implementing an analysis to plot the angular distribution between two selected
//photons. It will automatically select the photon which has input energy closest to the 
//input photon energy and also within epsilon.
void photonADAnalyzerUpg (double firstPhotonEnergy_MeV, double secondPhotonEnergy_MeV, TString fileName, double epsilon = 0.1) {
  gDirectory->pwd();
  TFile *treeFile = new TFile(fileName);
  TTree *tr = (TTree*)treeFile->Get("tree");
  Long64_t numberofEntries = tr->GetEntries();

  vector<double>* E = NULL;
  vector<double>* x = NULL;
  vector<double>* y = NULL;
  vector<double>* z = NULL;

  tr->SetBranchAddress("E", &E);
  tr->SetBranchAddress("x", &x);
  tr->SetBranchAddress("y", &y);
  tr->SetBranchAddress("z", &z);

  ostringstream title;
  title<< "Angular Distribution Between" << firstPhotonEnergy_MeV << "MeV and " << secondPhotonEnergy_MeV << "MeV Photon";
  TString histTitle = title.str();
  title<<".root";
  TString fileTitle = title.str();
  TH1F * photonAD = new TH1F(histTitle,histTitle, 100, -1, 1);

  for(Int_t entry = 0;entry < numberofEntries; ++entry) {
    tr->GetEntry(entry);
    int iPhoton1 = -1, iPhoton2 = -1;
    double dPhoton1 = 1000.0, dPhoton2 = 1000.0;

    //select the photon where the energy difference between its energy and 
    //input photon energy is minimum.
    for(size_t iE = 0; iE < E->size(); ++iE) {
      if (Abs(E->at(iE) - firstPhotonEnergy_MeV) <= dPhoton1) {
      	dPhoton1 = Abs(E->at(iE) - firstPhotonEnergy_MeV);
      	iPhoton1 = iE;
      } 
     if (Abs(E->at(iE) - secondPhotonEnergy_MeV) <= dPhoton2) {
      	dPhoton2 = Abs(E->at(iE) - secondPhotonEnergy_MeV);
      	iPhoton2 = iE;
      }
     }
    //make sure the energy difference between given photon and Geant4 generated photon is 
    //within acceptable epsilon range.
    bool isAcceptableEpsilon = (dPhoton1/firstPhotonEnergy_MeV <= epsilon) && (dPhoton2/secondPhotonEnergy_MeV <= epsilon);
    if (iPhoton1 != -1 && iPhoton2 != -1 && iPhoton1 != iPhoton2 && isAcceptableEpsilon) {
    	TVector3 photon1(x->at(iPhoton1),y->at(iPhoton1), z->at(iPhoton1));
    	TVector3 photon2(x->at(iPhoton2),y->at(iPhoton2), z->at(iPhoton2));
    	double cosAngle = photon1.Dot(photon2)/(photon1.Mag()*photon2.Mag());
		photonAD->Fill(cosAngle);
	   } 
	}
    TFile f(fileTitle, "RECREATE");
    photonAD->Write();
    f.Close();

}

#include "Pythia8/Pythia.h"
#include "StringSpinner.h"

using namespace Pythia8;

int main() {

  Pythia pythia;
  Event& event = pythia.event;
  auto fhooks = std::make_shared<SimpleStringSpinner>();
  fhooks->plugInto(pythia);

  // Beam energies in GeV, minimal Q2, number of events to generate.
  ParticleData& pdt = pythia.particleData;
  double eNucleon  = pdt.m0(2212);
  double pLepton	= 160;
  double mLepton  = pdt.m0(13);
  double eLepton	  = sqrt(pow(pLepton,2) + pow(mLepton,2));
  double Q2min	  = 1.0;
  int    nEvent	  = 1e6;

  // Set up incoming beams, for frame with unequal beam energies.
  pythia.readString("Beams:frameType = 2");
  pythia.readString("Beams:idA = 13");      // BeamA = muon.
  pythia.readString("Beams:idB = 2212");    // BeamB = proton.
  pythia.settings.parm("Beams:eA", eLepton);  // Muon energy.
  pythia.settings.parm("Beams:eB", eNucleon); // Proton at rest.

  // Interaction mechanism.
  pythia.readString("WeakBosonExchange:ff2ff(t:gmZ) = on");

  // Phase-space cut: minimal Q2 of process.
  pythia.settings.parm("PhaseSpace:Q2Min", Q2min);

  // Go down to low x-Bjorken.
  pythia.readString("PhaseSpace:pTHatMinDiverge = 0.5");
  pythia.readString("PhaseSpace:mHatMin = 0.");

  // Set dipole recoil on. Necessary for DIS + shower.
  pythia.readString("SpaceShower:dipoleRecoil = off");

  // QED radiation off lepton not handled yet by the new procedure.
  pythia.readString("PDF:lepton = off");
  pythia.readString("TimeShower:QEDshowerByL = off");

  // Choice of PDF = CTEQ5L LO (pSet=2).
  pythia.readString("PDF:pSet = 2");
  pythia.readString("PDF:pHardSet = 2");

  // Switch off resonance decays, ISR, FSR, MPI and Bose-Einstein.
  pythia.readString("ProcessLevel:resonanceDecays = off");
  pythia.readString("PartonLevel:FSRinResonances = off");
  pythia.readString("PartonLevel:FSR = off");
  pythia.readString("PartonLevel:ISR = off");
  pythia.readString("PartonLevel:MPI = off");
  pythia.readString("HadronLevel:BoseEinstein = off");

  // Primordial kT is switched off.
  pythia.readString("BeamRemnants:primordialKT = off");
  pythia.readString("BeamRemnants:primordialKTremnant = 0.0");

  // Switches for hadron production and decay.
  pythia.readString("111:onMode = off"); // pi0
  pythia.readString("311:onMode = off"); // K0
  pythia.readString("211:onMode = off"); // pi+
  // Invariant mass distribution of resonances as in the string+3P0 model.
  pythia.readString("ParticleData:modeBreitWigner=3");

  // Switch off automatic event listing in favour of manual.
  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberShowEvent = 1");

  // Settings of string fragmentation parameters.
  pythia.readString("StringPT:enhancedFraction = 0.");
  pythia.readString("StringPT:enhancedWidth = 0.");

  // StringSpinner settings.
  // Value of the free parameters |GL/GT| and thetaLT = arg(GL/GT).
  pythia.readString("StringSpinner:GLGT = 5");
  pythia.readString("StringSpinner:thetaLT = 0");
  // Target transverse polarisation according to the transversity PDFs.
  pythia.readString("StringSpinner:targetPolarisation = 0.0,1.0,0.0");
  // Selection of the polarisation vector of a given quark.
  //pythia.readString("StringSpinner:uPolarisation = 0.0,1.0,0.0");

  // Initialize.
  pythia.init();

  // Variables for test calculation of the pi+ Collins asymmetry.
  Vec4    SNucleon(0.0, 1.0, 0.0, 0.0);
  double  Acoll[2]  = {0.0};
  int     Npi       = 0;
  double  DNN       = 0.0;
  double  STav      = 0.0;

  // Begin event loop.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {

    if (!pythia.next()) continue;

    // Construct a DISKinematics class.
    DISKinematics dis(event[1].p(), event[5].p(), event[2].p());

    // Phase space cuts according to the COMPASS analysis of Collins asymmetries.
    if (dis.W2 < 25.0) continue;
    if (dis.y < 0.1 || dis.y > 0.9) continue;

    // Momenta of the exchanged photon and target nucleon in the GNS.
    Vec4 pPhoton  = dis.GNS * dis.q;
    Vec4 pNucleon = dis.GNS * dis.hadin;

    // Rotate the target polarization vector to the GNS, and calculate the transverse polarization
    // and azimuthal angle.
    Vec4 SNucleonGNS  = dis.GNS * SNucleon;
    double ST   = SNucleonGNS.pT();
    double phiS = SNucleonGNS.phi();

    // List some events.
    if (iEvent < 20) event.list();

    // Loop inside the event output.
    for (int i = 0; i < event.size(); ++i){

      // Hadron momentum in the GNS, id and status.
      Vec4 pHad     = dis.GNS * event[i].p();
      int idHad     = event[i].id();
      int statusHad = event[i].status();

      // Hadrons fractional energy, azimuthal angle and transverse momentum squared in the GNS.
      double zh     = ( pHad * pNucleon ) / (pNucleon * pPhoton);
      double phiHad = pHad.phi();
      double pT2 = pHad.pT2();

      // Fill test Collins asymmetry for pi+.
      // The requirement statusHad=83 (primary hadrons) allows to obtain a large
      // test asymmetry with a small number of simulated events. In real analyses
      // this cut is not applied.
      if ( idHad==211 && zh>0.2 && sqrt(pT2)>0.1 && abs(statusHad)==83 ) {
        Acoll[0] += 2.0*sin(phiHad+phiS-M_PI);
        Acoll[1] += pow(2.0*sin(phiHad+phiS-M_PI),2);
        Npi += 1;
        DNN += 2.0*(1.0-dis.y)/(1.0+pow(1.0-dis.y,2));
        STav += ST;
      }

    } // End loop on particle within the same event.

  } // End loop on events.

  // Calculate test Collins asymmetry for pi+ and print the output.
  for(int i=0;i<2;i++) Acoll[i] /= Npi;
  DNN /= Npi;
  STav /= Npi;
  cout << "The average Collins asymmetry for pi+ is: \n";
  cout << Acoll[0]/(DNN*STav) << "+/-" 
       << sqrt((Acoll[1]-pow(Acoll[0],2))/Npi)/(DNN*STav)
       << endl;
  
  return 0;
}

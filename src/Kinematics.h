#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "TLorentzVector.h"
#include "LundReader.h"
#include "HadroniumParser.h"
#include "KinematicsStructs.h"
#include <vector>

class KinematicsCalculator {
private:
    TLorentzVector initialElectron;
    TLorentzVector initialProton;
    TLorentzVector finalElectron;
    TLorentzVector q; // Virtual photon
    int target_polarization, beam_polarization;
public:
    KinematicsCalculator(const LundEvent& event);

    EventKinematics CalculateEventKinematics() const;
    std::vector<SingleHadronKinematics> CalculateSingleHadronKinematics(const std::vector<std::vector<Hadronium>>& hadronia) const;
    std::vector<DiHadronKinematics> CalculateDiHadronKinematics(const std::vector<std::vector<Hadronium>>& hadronia) const;
    double phi_h(TLorentzVector p1, TLorentzVector p2) const;
    double phi_h(TLorentzVector p) const;
    double xF(TLorentzVector p) const;
    double Pt_COM(TLorentzVector dihadron) const;
    double phi_RT(TLorentzVector p1, TLorentzVector p2) const;
    double phi_Rperp(TLorentzVector p1, TLorentzVector p2) const;
    double com_th(TLorentzVector P1, TLorentzVector P2) const;
    double z(TLorentzVector part) const;
};

#endif // KINEMATICS_H

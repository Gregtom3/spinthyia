#include "LundReader.h"
#include "Kinematics.h"


KinematicsCalculator::KinematicsCalculator(const LundEvent& event)
    : initialElectron(event.particles[0].px, event.particles[0].py, event.particles[0].pz, event.particles[0].e),
      initialProton(event.particles[1].px, event.particles[1].py, event.particles[1].pz, event.particles[1].e) {
    // Final Electron is the first pid==11 particle that is final state
    for (const auto& particle : event.particles) {
        if (particle.particle_id == 11 && particle.status == 1) {
            finalElectron = TLorentzVector(particle.px, particle.py, particle.pz, particle.e);
            break;
        }
    }

    q = initialElectron - finalElectron; // Assuming operator- is defined for Particle
    target_polarization = event.target_polarization;
    beam_polarization = event.beam_polarization;
}

EventKinematics KinematicsCalculator::CalculateEventKinematics() const {
    TLorentzVector q = initialElectron - finalElectron;
    double Q2 = -q*q;
    double y = (q * initialProton) / (initialElectron * initialProton);
    double x = Q2 / (2.0 * (q * initialProton));
    double W = sqrt(initialProton.M2() + 2.0 * initialProton*q - Q2);
    return EventKinematics{x, Q2, y, W, target_polarization, beam_polarization};
}

std::vector<SingleHadronKinematics> KinematicsCalculator::CalculateSingleHadronKinematics(const std::vector<std::vector<Hadronium>>& hadronia) const {
    std::vector<SingleHadronKinematics> allHadronKinematics;

    for (const auto& hadronium : hadronia) {
        Hadronium hadron = combine_particles(hadronium); // Assuming this function is implemented elsewhere
        TLorentzVector p(hadron.px, hadron.py, hadron.pz, hadron.e);
        
        SingleHadronKinematics kinematics{
            this->Pt_COM(p), // Transverse momentum
            this->z(p), // z (fractional energy)
            this->phi_h(p), // Azimuthal angle
            p.M(), // Invariant mass
            this->xF(p), // xF (Feynman x)
            this->Mx(p)
        };
        
        allHadronKinematics.push_back(kinematics);
    }

    return allHadronKinematics;
}


std::vector<DiHadronKinematics> KinematicsCalculator::CalculateDiHadronKinematics(const std::vector<std::vector<Hadronium>>& hadronia) const {
    std::vector<DiHadronKinematics> allDiHadronKinematics;

    for (const auto& hadronium : hadronia) {
        
        TLorentzVector p1(hadronium.at(0).px, hadronium.at(0).py, hadronium.at(0).pz, hadronium.at(0).e);
        TLorentzVector p2(hadronium.at(1).px, hadronium.at(1).py, hadronium.at(1).pz, hadronium.at(1).e);
        TLorentzVector p = p1+p2;
        DiHadronKinematics kinematics{
            this->Pt_COM(p1),
            this->Pt_COM(p2),
            this->Pt_COM(p),
            this->z(p1),
            this->z(p2),
            this->z(p),
            this->phi_h(p),
            this->phi_RT(p1,p2),
            this->phi_Rperp(p1,p2),
            p.M(),
            this->xF(p1),
            this->xF(p2),
            this->xF(p),
            this->Mx(p)
        };
        
        allDiHadronKinematics.push_back(kinematics);
    }

    return allDiHadronKinematics;
}


double KinematicsCalculator::phi_h(TLorentzVector p1, TLorentzVector p2) const {
    TLorentzVector ph = p1 + p2;
    TVector3 Ph(ph.Px(), ph.Py(), ph.Pz());
    TVector3 qVect(q.Px(), q.Py(), q.Pz());
    TVector3 initialElectronVect(initialElectron.Px(), initialElectron.Py(), initialElectron.Pz());

    TVector3 qcrossL = qVect.Cross(initialElectronVect);
    TVector3 qcrossPh = qVect.Cross(Ph);

    double factor1 = (qcrossL * Ph) / abs(qcrossL * Ph);
    double factor2 = (qcrossL * qcrossPh) / qcrossL.Mag() / qcrossPh.Mag();

    return factor1 * acos(factor2);
}


double KinematicsCalculator::z(TLorentzVector part) const {
    return (initialProton * part) / (initialProton * q);
}

double KinematicsCalculator::Mx(TLorentzVector part) const {
    return (initialElectron + initialProton - finalElectron - part).M();
}


// Method to calculate phi_h for a single hadron
double KinematicsCalculator::phi_h(TLorentzVector p) const {
    TVector3 Ph(p.Px(), p.Py(), p.Pz());
    TVector3 qVect(q.Px(), q.Py(), q.Pz());
    TVector3 initialElectronVect(initialElectron.Px(), initialElectron.Py(), initialElectron.Pz());

    TVector3 qcrossL = qVect.Cross(initialElectronVect);
    TVector3 qcrossPh = qVect.Cross(Ph);

    double factor1 = (qcrossL * Ph) / abs(qcrossL * Ph);
    double factor2 = (qcrossL * qcrossPh) / qcrossL.Mag() / qcrossPh.Mag();

    return factor1 * acos(factor2);
}

double KinematicsCalculator::xF(TLorentzVector p) const {
    
    double W = sqrt(initialProton.M2() + 2.0 * initialProton*q + q*q);
        
    TLorentzVector com = q + initialProton;
    TVector3 comBoost = com.BoostVector();
    
    TLorentzVector pp = p;
    pp.Boost(-comBoost);

    return 2 * pp.Pz() / W;
}

double KinematicsCalculator::Pt_COM(TLorentzVector p) const {
    TLorentzVector com = q + initialProton;
    TVector3 comBoost = com.BoostVector();
    
    TLorentzVector pp = p;
    pp.Boost(-comBoost);

    return pp.Perp();
}

// Method to calculate phi_RT
double KinematicsCalculator::phi_RT(TLorentzVector p1, TLorentzVector p2) const {
    TLorentzVector r = 0.5 * (p1 - p2);
    TVector3 R(r.Px(), r.Py(), r.Pz());

    TVector3 Rperp;
    Rperp = R - (R.Dot(q.Vect()) / q.Vect().Mag2()) * q.Vect();

    TVector3 qVect(q.Px(), q.Py(), q.Pz());
    TVector3 initialElectronVect(initialElectron.Px(), initialElectron.Py(), initialElectron.Pz());

    TVector3 qcrossL = qVect.Cross(initialElectronVect);
    TVector3 qcrossRperp = qVect.Cross(Rperp);

    double factor1 = (qcrossL * Rperp) / abs(qcrossL * Rperp);
    double factor2 = (qcrossL * qcrossRperp) / qcrossL.Mag() / qcrossRperp.Mag();

    return factor1 * acos(factor2);
}


// Method to calculate phi_Rperp
double KinematicsCalculator::phi_Rperp(TLorentzVector p1, TLorentzVector p2) const {
    TLorentzVector r = 0.5 * (p1 - p2);
    TVector3 R(r.Px(), r.Py(), r.Pz());

    TVector3 Rperp;
    double z1 = z(p1);
    double z2 = z(p2);
    TVector3 P1perp = p1.Vect() - (p1.Vect().Dot(q.Vect()) / q.Vect().Mag2()) * q.Vect();
    TVector3 P2perp = p2.Vect() - (p2.Vect().Dot(q.Vect()) / q.Vect().Mag2()) * q.Vect();
    Rperp = (z2 * P1perp - z1 * P2perp) * (1 / (z1 + z2));

    TVector3 qVect(q.Px(), q.Py(), q.Pz());
    TVector3 initialElectronVect(initialElectron.Px(), initialElectron.Py(), initialElectron.Pz());

    TVector3 qcrossL = qVect.Cross(initialElectronVect);
    TVector3 qcrossRperp = qVect.Cross(Rperp);

    double factor1 = (qcrossL * Rperp) / abs(qcrossL * Rperp);
    double factor2 = (qcrossL * qcrossRperp) / qcrossL.Mag() / qcrossRperp.Mag();

    return factor1 * acos(factor2);
}

// Method to calculate dihadron theta in the center of mass frame
double KinematicsCalculator::com_th(TLorentzVector P1, TLorentzVector P2) const {
    TLorentzVector Ptotal = P1 + P2;
    TVector3 comBoost = Ptotal.BoostVector();

    P1.Boost(-comBoost);
    return P1.Vect().Angle(comBoost);
}

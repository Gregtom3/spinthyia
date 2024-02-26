// KinematicsStructs.h
#ifndef KINEMATICS_STRUCTS_H
#define KINEMATICS_STRUCTS_H

#include <vector>

struct EventKinematics {
    double x = 0.0;
    double Q2 = 0.0;
    double y = 0.0;
    double W = 0.0;
    int target_polarization = 0;
    int beam_polarization = 0;
};

struct SingleHadronKinematics {
    double pt = 0.0; 
    double z = 0.0;   
    double phi = 0.0;
    double Mh = 0.0;
    double xF = 0.0;
};

struct DiHadronKinematics {
    double pt1 = 0.0;
    double pt2 = 0.0;
    double pt = 0.0;
    double z1 = 0.0;
    double z2 = 0.0;
    double z  = 0.0;
    double phi_h = 0.0;
    double phi_RT = 0.0;
    double phi_Rperp = 0.0;
    double Mh = 0.0;
    double xF1 = 0.0;
    double xF2 = 0.0;
    double xF  = 0.0;
};

#endif // KINEMATICS_STRUCTS_H
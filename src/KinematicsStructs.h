// KinematicsStructs.h
#ifndef KINEMATICS_STRUCTS_H
#define KINEMATICS_STRUCTS_H

#include <vector>

enum class HadroniumAnalysisType {
    SingleHadron,
    DiHadron
};

struct EventKinematics {
    double x = 0.0;
    double Q2 = 0.0;
    double y = 0.0;
    double W = 0.0;
    double phi_S = 0.0;
    double epsilon = 0.0;
    double gamma = 0.0;
    double depolA = 0.0;
    double depolB = 0.0;
    double depolC = 0.0;
    double depolV = 0.0;
    double depolW = 0.0;
    int target_polarization = 0;
    int beam_polarization = 0;
};

struct SingleHadronKinematics {
    double pt = 0.0; 
    double z = 0.0;   
    double phi = 0.0;
    double Mh = 0.0;
    double xF = 0.0;
    double Mx = 0.0;
    int parentPid = 0;
    int grandParentPid = 0;
    int status = 0;
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
    double th = 0.0;
    double Mh = 0.0;
    double xF1 = 0.0;
    double xF2 = 0.0;
    double xF  = 0.0;
    double Mx  = 0.0;
    int parentPid1 = 0;
    int grandParentPid1 = 0;
    int status1 = 0;
    int parentPid2 = 0;
    int grandParentPid2 = 0;
    int status2 = 0;
};

#endif // KINEMATICS_STRUCTS_H
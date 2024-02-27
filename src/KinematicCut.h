// KinematicCut.h
#ifndef KINEMATIC_CUT_H
#define KINEMATIC_CUT_H

#include <string>
#include <functional>

class KinematicCut {
public:
    enum class CutType { MIN, MAX, RANGE };

    std::string variableName;
    CutType type;
    double minValue;
    double maxValue;

    KinematicCut(std::string variableName, CutType type, double value)
    : variableName(variableName), type(type), minValue(value), maxValue(value) {}

    KinematicCut(std::string variableName, double minValue, double maxValue)
    : variableName(variableName), type(CutType::RANGE), minValue(minValue), maxValue(maxValue) {}
};

#endif // KINEMATIC_CUT_H
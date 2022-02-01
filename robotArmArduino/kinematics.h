#pragma once

void solveForwardkinematics(float *xyz, const float lengths[], int nowDegrees[]);
bool solveInverseKinematics(float xyz[], const float lengths[], int *newDegrees);

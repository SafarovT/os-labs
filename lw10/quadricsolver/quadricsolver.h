#pragma once
#include <vector>


#ifdef COMPILING_DLL
	#define DLLEXPORT __declspec(dllexport)
#else
	#define DLLEXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	DLLEXPORT int SolveQuadratic(double coefficientA, double coefficientB, double coefficientC, double& root1, double& root2);

#ifdef __cplusplus
}
#endif

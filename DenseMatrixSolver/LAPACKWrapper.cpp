#include "LAPACKWrapper.h"
//#include "Architecture.h"

using namespace gsi;

// atlas version
//extern "C" {
//#include <atlas_clapack.h>
//}

// clapack version
extern "C" {
#define integer int
#define real double
#define doublereal double
#define complex int
#define doublecomplex int
#define logical int
#define L_fp void *
#define ftnlen int
#define VOID void

#include <clapack_default.h>
// #include <clapack.h>

#undef integer
#undef real
#undef doublereal
#undef complex
#undef doublecomplex
#undef logical
#undef L_fp
#undef ftnlen
#undef VOID
}



int LAPACKWrapper::dgesv( int nDim, int nNumRHS,
							    double * pMatrix, int nMatrixDim,
								int * nPivotBuf, 
								double * pRHSVectors, int nRHSDim )
{
	// CLAPACK version
	int nInfo;
	int nReturn = dgesv_(&nDim, &nNumRHS, pMatrix, &nMatrixDim, nPivotBuf, pRHSVectors, &nRHSDim, &nInfo);
	return nReturn;

	
//	int nReturn = -1, nInfo;
//	switch( Architecture::Get()->GetType() ) {
//		case Architecture::Unknown:
//			// ATLAS version
////			nReturn = clapack_dgesv(CblasColMajor, nDim, nNumRHS, pMatrix, nMatrixDim, nPivotBuf, pRHSVectors, nRHSDim);
//
//			// CLAPACK version
//			nReturn = dgesv_(&nDim, &nNumRHS, pMatrix, &nMatrixDim, nPivotBuf, pRHSVectors, &nRHSDim, &nInfo);
//
//			return nReturn;
//
//		case Architecture::Athlon_3DNow:
////			lapackX_dgesv(nDim, nNumRHS, pMatrix, nMatrixDim, nPivotBuf, pRHSVectors, nRHSDim, &nReturn);
//			return nReturn;
//
//
//		default:
//			return -1;
//	}
}

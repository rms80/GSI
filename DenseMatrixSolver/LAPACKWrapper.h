#pragma once

namespace gsi {

class LAPACKWrapper
{
public:


	static int dgesv( int nDim, int nNumRHS,
							double * pMatrix, int nMatrixDim,
							int * nPivotBuf, 
							double * pRHSVectors, int nRHSDim );

protected:
};


}
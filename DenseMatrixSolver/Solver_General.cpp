#include "Solver_General.h"
#include "LAPACKWrapper.h"

#include <math.h>


using namespace gsi;


Solver_General::Solver_General(DenseLinearSystem * pSystem)
{
	m_pSystem = pSystem;
}

Solver_General::~Solver_General(void)
{
	m_pSystem = NULL;
}


bool Solver_General::Solve()
{
	// RMS TODO: currently assuming square matrix...
	unsigned int nRows = m_pSystem->Matrix().Rows();
	if ( nRows != m_pSystem->Matrix().Columns() )
		return false;

	const DenseMatrix & matrix = m_pSystem->Matrix();

	int * nPivot = new int[ nRows ];
	if ( ! nPivot )
		return false;

	// pack up RHS vectors so we can pass a block-pointer
	unsigned int nRHS = m_pSystem->NumRHS();
	std::vector<double> vSolution(nRHS*nRows);
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		const Vector & v = m_pSystem->GetRHS(i);
		for ( unsigned int j = 0; j < nRows; ++j )
			vSolution[i*nRows + j] = v[j];
	}

	int nResult = LAPACKWrapper::dgesv( 
						nRows, nRHS, 
						(double *)m_pSystem->Matrix(), nRows, 
						nPivot, &vSolution[0], nRows );

	// unpack solution
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		Vector & v = m_pSystem->GetSolution(i);
		for ( unsigned int j = 0; j < nRows; ++j )
			v[j] = vSolution[i*nRows + j];
	}

	if ( nPivot ) {
		delete nPivot;   
		nPivot = NULL;
	}

	return false;
}





bool Solver_General::Factorize()
{
	return false;
}

bool Solver_General::Solve_Factorized()
{
	return false;
}



#include "Solver_UMFPACK.h"

#include <math.h>

extern "C" {
#include <taucs.h>
#include <umfpack.h>
}


using namespace gsi;


Solver_UMFPACK::Solver_UMFPACK(SparseLinearSystem * pSystem)
{
	m_pSystem = pSystem;
}

Solver_UMFPACK::~Solver_UMFPACK(void)
{
	m_pSystem = NULL;
}




class MatrixBuilder_UMFPACK : public SparseMatrix::IColumnFunction {
public:
	taucs_ccs_matrix * pMatrix;
	int * nCounter;
	virtual void NextEntry( unsigned int r, unsigned int c, double dVal ) {
		pMatrix->rowind[*nCounter] = r;
		pMatrix->values.d[*nCounter] = dVal;
		*nCounter = *nCounter+1;
	}
};



bool Solver_UMFPACK::Solve()
{
	const SparseMatrix & matrix = m_pSystem->Matrix();

	// figure out number of non-zeros in lower-triangular matrix
	unsigned int nNNZ = matrix.CountNonZeros(false);
	unsigned int nRows = matrix.Rows();
	unsigned int nCols = matrix.Columns();

	// use TAUCS to construct matrix
	int create_flags = TAUCS_DOUBLE;
	taucs_ccs_matrix * pTaucsMatrix = taucs_ccs_create(nRows, nCols, nNNZ, create_flags);

	if ( ! pTaucsMatrix )
		return false;
	int nCur = 0;
	for ( unsigned int c = 0; c < nCols; ++c ) {
		pTaucsMatrix->colptr[c] = nCur;

		MatrixBuilder_UMFPACK m;
		m.pMatrix = pTaucsMatrix;
		m.nCounter = &nCur;
		matrix.ApplyColumnFunction(c, &m);
	}
	pTaucsMatrix->colptr[nCols] = nCur;	

	void * Symbolic = NULL, * Numeric = NULL;
	double * null = (double *)NULL;
	int * col = pTaucsMatrix->colptr;
	int * row = pTaucsMatrix->rowind;
	double * val = pTaucsMatrix->values.d;
	double Control [UMFPACK_CONTROL];
	double Info [UMFPACK_INFO];

	umfpack_di_defaults(Control);

	Control[UMFPACK_STRATEGY] = UMFPACK_STRATEGY_UNSYMMETRIC;

	// pack up RHS vectors so we can pass a block-pointer
	unsigned int nRHS = m_pSystem->NumRHS();
	std::vector<double> vRHS(nRHS*nCols), vSolution(nRHS*nCols);
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		const Vector & v = m_pSystem->GetRHS(i);
		for ( unsigned int j = 0; j < nCols; ++j )
			vRHS[i*nCols + j] = v[j];
	}

	int status = umfpack_di_symbolic(nCols, nCols, col, row, val, &Symbolic, Control, Info);
	status = umfpack_di_numeric(col, row, val, Symbolic, &Numeric, Control, Info);
	status = umfpack_di_solve(UMFPACK_A, col, row, val, &vSolution[0], &vRHS[0], Numeric, Control, Info);

	// unpack solution
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		Vector & v = m_pSystem->GetSolution(i);
		for ( unsigned int j = 0; j < nCols; ++j )
			v[j] = vSolution[i*nCols + j];
	}

	umfpack_di_free_symbolic(&Symbolic);
	umfpack_di_free_numeric(&Numeric);

	// release taucs memory
	taucs_ccs_free(pTaucsMatrix);

	return status == UMFPACK_STATUS;
}





bool Solver_UMFPACK::Factorize()
{
	return false;
}

bool Solver_UMFPACK::Solve_Factorized()
{
	return false;
}



#include ".\SparseSymmetricMatrixSolver.h"

#include <math.h>

extern "C" {
#include <taucs.h>
#include <umfpack.h>
}


using namespace gsi;





SparseSymmetricMatrixSolver::SparseSymmetricMatrixSolver(unsigned int nRows, unsigned int nCols)
{
	Resize(nRows, nCols);

	m_n1Offset = 0;
	m_ePreconditionerMode = NoPreconditoner;
	m_eSolverMode = TAUCS_MinRes;
	m_eOrderMode = TAUCS_AUTOMATIC;

	m_nSolverMaxIterations = 0;		// use default
	m_fSolverConvergeTolerance = 0; // use default

	m_logfile = "";

	m_pTaucsMatrix = NULL;
	m_pTaucsFactorMatrix = NULL;
}

SparseSymmetricMatrixSolver::~SparseSymmetricMatrixSolver(void)
{
	if ( m_pTaucsMatrix != NULL )
		Cleanup_Factor_TAUCS();
}




void SparseSymmetricMatrixSolver::Set( unsigned int r, unsigned int c, double dValue )
{
	r -= m_n1Offset; c -= m_n1Offset;
	m_vColumns[c].insert( Entry(r,dValue) );
}

double SparseSymmetricMatrixSolver::Get( unsigned int r, unsigned int c)
{
	r -= m_n1Offset; c -= m_n1Offset;
	Column::iterator found(m_vColumns[c].find(Entry(r)));
	return (found == m_vColumns[c].end()) ? 0.0f : (*found).value;
}

void SparseSymmetricMatrixSolver::SetRHS( unsigned int r, double dValue, unsigned int nRHS  )
{
	r -= m_n1Offset;
	m_vRHS[nRHS][r] = dValue;
}

void SparseSymmetricMatrixSolver::SetRHSVec( double * dValue, unsigned int nRHS )
{
	for ( unsigned int i = 0; i < m_nRows; ++i )
		m_vRHS[nRHS][i] = dValue[i + m_n1Offset];
}


double SparseSymmetricMatrixSolver::GetRHS( unsigned int r, unsigned int nRHS )
{
	r -= m_n1Offset;
	return m_vRHS[nRHS][r];
}

double SparseSymmetricMatrixSolver::GetSolution( unsigned int r, unsigned int nRHS )
{
	r -= m_n1Offset;
	return m_vSolutions[nRHS][r];
}

double * SparseSymmetricMatrixSolver::GetSolutionVec( unsigned int nRHS )
{
	return & m_vSolutions[nRHS][0];
}




void SparseSymmetricMatrixSolver::Clear(bool bFree)
{
	if (bFree) {
		m_vColumns.resize(0);		// clear existing entries
	} else {
		for ( unsigned int i = 0; i < m_nCols; ++i )
			m_vColumns[i].clear();
	}

	m_nRows = m_nCols = 0;
}

void SparseSymmetricMatrixSolver::ClearRow( unsigned int r )
{
	Entry e(r, 0);
	for ( unsigned int i = 0; i < m_nCols; ++i ) {
		Column::iterator found( m_vColumns[i].find(e) );
		if ( found != m_vColumns[i].end() )
			m_vColumns[i].erase(found);
	}
}

void SparseSymmetricMatrixSolver::Resize( unsigned int nRows, unsigned int nCols )
{
	Clear(true);
	m_vColumns.resize(nCols);
	m_nRows = nRows;
	m_nCols = nCols;

	ResizeRHS( (m_vRHS.size() == 0)  ?  1  :  (unsigned int)m_vRHS.size() );
}

void SparseSymmetricMatrixSolver::ResizeRHS( unsigned int nRHS )
{
	m_vRHS.resize(nRHS);
	m_vSolutions.resize(nRHS);
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		
		m_vRHS[i].resize(0);
		m_vRHS[i].resize( m_nRows, 0 );

		m_vSolutions[i].resize(0);
		m_vSolutions[i].resize( m_nRows, 0 );
	}
}


unsigned int SparseSymmetricMatrixSolver::CountNonZeros(bool bLowerOnly)
{
	unsigned int nCount = 0;
	size_t nCols = m_vColumns.size();
	for ( unsigned int i = 0; i < nCols; ++i ) {
		if ( bLowerOnly ) {
			Column::iterator cur(m_vColumns[i].begin()), end(m_vColumns[i].end());
			while ( cur != end ) {
				Entry & e = *cur;  ++cur;
				if ( e.r <= i )
					nCount++;
			}
		} else 
			nCount += (unsigned int)m_vColumns[i].size();
	}
	return nCount;
}


double SparseSymmetricMatrixSolver::IsSymmetric()
{
	double dMaxErr = 0;
	for ( unsigned int r = 0; r < m_nRows; ++r ) {
		for ( unsigned int c = r; c < m_nCols; ++c) {
			double d1 = Get(r,c);
			double d2 = Get(c,r);
			if ( d1 != d2 ) {
				double dErr = d1 - d2;
				if ( fabs(dErr) > fabs(dMaxErr) )
					dMaxErr = dErr;
			}
		}
	}
	return fabs(dMaxErr);
}



bool SparseSymmetricMatrixSolver::Solve()
{
	switch ( m_eSolverMode ) {
		case UMFPACK:
			return Solve_UMFPACK();
			
		default:
			return Solve_TAUCS();
	}

}


bool SparseSymmetricMatrixSolver::Solve_UMFPACK()
{
	// use TAUCS to construct matrix
	// figure out number of non-zeros in lower-triangular matrix
	unsigned int nNNZ = CountNonZeros(false);
	int create_flags = TAUCS_DOUBLE;
	taucs_ccs_matrix * pTaucsMatrix = taucs_ccs_create(m_nRows, m_nCols, nNNZ, create_flags);

	if ( ! pTaucsMatrix )
		return false;
	int nCur = 0;
	for ( unsigned int c = 0; c < m_nCols; ++c ) {
		Column & col = m_vColumns[c];
		pTaucsMatrix->colptr[c] = nCur;

		Column::iterator cur(col.begin()), end(col.end());
		while ( cur != end ) {
			Entry & e = *cur;  ++cur;
			pTaucsMatrix->rowind[nCur] = e.r;
			pTaucsMatrix->values.d[nCur] = e.value;
			++nCur;
		}
	}
	pTaucsMatrix->colptr[m_nCols] = nCur;	

	void * Symbolic = NULL, * Numeric = NULL;
	double * null = (double *)NULL;
	int * col = pTaucsMatrix->colptr;
	int * row = pTaucsMatrix->rowind;
	double * val = pTaucsMatrix->values.d;
	double Control [UMFPACK_CONTROL];
	double Info [UMFPACK_INFO];

	umfpack_di_defaults(Control);

	Control[UMFPACK_STRATEGY] = UMFPACK_STRATEGY_UNSYMMETRIC;

	int status = umfpack_di_symbolic(m_nCols, m_nCols, col, row, val, &Symbolic, Control, Info);
	status = umfpack_di_numeric(col, row, val, Symbolic, &Numeric, Control, Info);
	status = umfpack_di_solve(UMFPACK_A, col, row, val, &m_vSolutions[0][0], &m_vRHS[0][0], Numeric, Control, Info);

	umfpack_di_free_symbolic(&Symbolic);
	umfpack_di_free_numeric(&Numeric);

	// release taucs memory
	taucs_ccs_free(pTaucsMatrix);

	return status == UMFPACK_STATUS;
}


bool SparseSymmetricMatrixSolver::Solve_TAUCS()
{
	char buf[1024];

	bool bUseLowerTriangular = true;


	// figure out number of non-zeros
	unsigned int nNNZ = CountNonZeros(bUseLowerTriangular);

	// allocate taucs matrix
	int create_flags = (bUseLowerTriangular) ? 
		TAUCS_DOUBLE | TAUCS_SYMMETRIC | TAUCS_TRIANGULAR | TAUCS_LOWER    :    TAUCS_DOUBLE | TAUCS_SYMMETRIC;
	taucs_ccs_matrix * pTaucsMatrix = taucs_ccs_create(m_nRows, m_nCols, nNNZ, create_flags);
	if ( ! pTaucsMatrix )
		return false;
	
	// initialize taucs matrix data - only fill lower-triangular part of matrix!
	int nCur = 0;
	for ( unsigned int c = 0; c < m_nCols; ++c ) {
		
		Column & col = m_vColumns[c];
		pTaucsMatrix->colptr[c] = nCur;

		Column::iterator cur(col.begin()), end(col.end());
		while ( cur != end ) {
			Entry & e = *cur;  ++cur;

			if ( !bUseLowerTriangular || e.r <= c ) {
				pTaucsMatrix->rowind[nCur] = e.r;
				pTaucsMatrix->values.d[nCur] = e.value;
				++nCur;
			}
		}
	}
	pTaucsMatrix->colptr[m_nCols] = nCur;	


	// set logfile if desired
	if ( m_logfile != std::string("") )
		taucs_logfile(const_cast<char *>(m_logfile.c_str()));

	// construct options strings
	std::vector<std::string> vOptionStrings;


	switch ( m_eSolverMode ) {
		case TAUCS_LLT:
			vOptionStrings.push_back( std::string("taucs.factor.LLT=true") );	break;
		case TAUCS_LU:
			vOptionStrings.push_back( std::string("taucs.factor.LU=true") );	break;
		case TAUCS_LDLT:
			vOptionStrings.push_back( std::string("taucs.factor.ldlt=true") );	break;
		case TAUCS_MF:
			vOptionStrings.push_back( std::string("taucs.factor.mf=true") );	break;
		case TAUCS_LL:
			vOptionStrings.push_back( std::string("taucs.factor.ll=true") );	break;
		case TAUCS_ConjGradient:
			vOptionStrings.push_back( std::string("taucs.solve.cg=true") );		break;
		case TAUCS_MinRes:
			vOptionStrings.push_back( std::string("taucs.solve.minres=true") );	break;
		default:
			break;
	};


	// set ordering mode option
	switch (m_eOrderMode) {
		case TAUCS_IDENTITY:
			vOptionStrings.push_back( "taucs.factor.ordering=identity" );	break;
		case TAUCS_GENMMD:
			vOptionStrings.push_back( "taucs.factor.ordering=genmmd" );		break;
		case TAUCS_MD:
			vOptionStrings.push_back( "taucs.factor.ordering=md" );			break;
		case TAUCS_MMD:
			vOptionStrings.push_back( "taucs.factor.ordering=mmd" );		break;
		case TAUCS_AMD:
			vOptionStrings.push_back( "taucs.factor.ordering=amd" );		break;
		case TAUCS_METIS:
			vOptionStrings.push_back( "taucs.factor.ordering=metis" );		break;
		case TAUCS_TREEORDER:
			vOptionStrings.push_back( "taucs.factor.ordering=treeorder" );	break;
		case TAUCS_COLAMD:
			vOptionStrings.push_back( "taucs.factor.ordering=colamd" );		break;
		case TAUCS_AUTOMATIC:
		default:
			break;
	}

	
	// set preconditioner
	switch ( m_ePreconditionerMode ) {
		case TAUCS_MaxiumuWeightBasis:
			vOptionStrings.push_back( "taucs.approximate.amwb=true" );		break;

		case NoPreconditoner:
		default:
			break;
	}


	if ( m_nSolverMaxIterations != 0 ) {
		sprintf_s(buf, 1024, "taucs.solve.maxits=%d", m_nSolverMaxIterations);
		vOptionStrings.push_back(buf);
	}
	if ( m_fSolverConvergeTolerance != 0 ) {
		sprintf_s(buf, 1024, "taucs.solve.convergetol=%e",m_fSolverConvergeTolerance);
		vOptionStrings.push_back(buf);
	}


	// make taucs-style option array 
	size_t nOptions = vOptionStrings.size();
	std::vector<char *> vOptions(nOptions+1, NULL);
	for ( unsigned int i = 0; i < nOptions; ++i )
		vOptions[i] = const_cast<char *>( vOptionStrings[i].c_str() );
	vOptions[nOptions] = NULL;

	// construct linear RHS vector
	std::vector<double> vRHS(m_nRows * m_vRHS.size());
	for ( unsigned int i = 0; i < m_vRHS.size(); ++i ) {
		for ( unsigned int j = 0; j < m_nRows; ++j )
			vRHS[i*m_nRows+j] = m_vRHS[i][j];
	}
	std::vector<double> vSolutions(vRHS.size());

	// solve!
	int nRetCode = taucs_linsolve(pTaucsMatrix, NULL, (int)m_vRHS.size(), &vSolutions[0], &vRHS[0], &vOptions[0], NULL);

	// break up linear solution vector
	for ( unsigned int i = 0; i < m_vRHS.size(); ++i ) {
		for ( unsigned int j = 0; j < m_nRows; ++j )
			m_vSolutions[i][j] = vSolutions[i*m_nRows+j];
	}

	// release taucs memory
	taucs_ccs_free(pTaucsMatrix);

	return nRetCode == TAUCS_SUCCESS;
}



bool SparseSymmetricMatrixSolver::Factorize()
{
	if ( m_eSolverMode != UMFPACK )
		return Factorize_TAUCS();
	return false;
}

bool SparseSymmetricMatrixSolver::Solve_Factorized()
{
	if ( m_pTaucsFactorMatrix != NULL )
		return Solve_Factor_TAUCS();
	return false;
}





bool SparseSymmetricMatrixSolver::Factorize_TAUCS()
{
	char buf[1024];

	bool bUseLowerTriangular = true;

	// figure out number of non-zeros
	unsigned int nNNZ = CountNonZeros(bUseLowerTriangular);

	// allocate taucs matrix
	int create_flags = (bUseLowerTriangular) ? 
		TAUCS_DOUBLE | TAUCS_SYMMETRIC | TAUCS_TRIANGULAR | TAUCS_LOWER    :    TAUCS_DOUBLE | TAUCS_SYMMETRIC;
	m_pTaucsMatrix = taucs_ccs_create(m_nRows, m_nCols, nNNZ, create_flags);;
	if ( ! m_pTaucsMatrix )
		return false;
	taucs_ccs_matrix * pTaucsMatrix = (taucs_ccs_matrix *)m_pTaucsMatrix;
	
	// initialize taucs matrix data - only fill lower-triangular part of matrix!
	int nCur = 0;
	for ( unsigned int c = 0; c < m_nCols; ++c ) {
		
		Column & col = m_vColumns[c];
		pTaucsMatrix->colptr[c] = nCur;

		Column::iterator cur(col.begin()), end(col.end());
		while ( cur != end ) {
			Entry & e = *cur;  ++cur;

			if ( !bUseLowerTriangular || e.r <= c ) {
				pTaucsMatrix->rowind[nCur] = e.r;
				pTaucsMatrix->values.d[nCur] = e.value;
				++nCur;
			}
		}
	}
	pTaucsMatrix->colptr[m_nCols] = nCur;	


	// set logfile if desired
	if ( m_logfile != std::string("") )
		taucs_logfile(const_cast<char *>(m_logfile.c_str()));

	// construct options strings
	m_vOptionStrings.resize(0);

	m_vOptionStrings.push_back("taucs.factor=true");

	switch ( m_eSolverMode ) {
		case TAUCS_LLT:
			m_vOptionStrings.push_back( std::string("taucs.factor.LLT=true") );	break;
		case TAUCS_LU:
			m_vOptionStrings.push_back( std::string("taucs.factor.LU=true") );	break;
		case TAUCS_LDLT:
			m_vOptionStrings.push_back( std::string("taucs.factor.ldlt=true") );	break;
		case TAUCS_MF:
			m_vOptionStrings.push_back( std::string("taucs.factor.mf=true") );	break;
		case TAUCS_LL:
			m_vOptionStrings.push_back( std::string("taucs.factor.ll=true") );	break;
		case TAUCS_ConjGradient:
		case TAUCS_MinRes:
			return false;		// not factorizable...
		default:
			break;
	};

	// set ordering mode option
	switch (m_eOrderMode) {
		case TAUCS_IDENTITY:
			m_vOptionStrings.push_back( "taucs.factor.ordering=identity" );	break;
		case TAUCS_GENMMD:
			m_vOptionStrings.push_back( "taucs.factor.ordering=genmmd" );		break;
		case TAUCS_MD:
			m_vOptionStrings.push_back( "taucs.factor.ordering=md" );			break;
		case TAUCS_MMD:
			m_vOptionStrings.push_back( "taucs.factor.ordering=mmd" );		break;
		case TAUCS_AMD:
			m_vOptionStrings.push_back( "taucs.factor.ordering=amd" );		break;
		case TAUCS_METIS:
			m_vOptionStrings.push_back( "taucs.factor.ordering=metis" );		break;
		case TAUCS_TREEORDER:
			m_vOptionStrings.push_back( "taucs.factor.ordering=treeorder" );	break;
		case TAUCS_COLAMD:
			m_vOptionStrings.push_back( "taucs.factor.ordering=colamd" );		break;
		case TAUCS_AUTOMATIC:
		default:
			break;
	}


	// set preconditioner
	switch ( m_ePreconditionerMode ) {
		case TAUCS_MaxiumuWeightBasis:
			m_vOptionStrings.push_back( "taucs.approximate.amwb=true" );		break;

		case NoPreconditoner:
		default:
			break;
	}


	if ( m_nSolverMaxIterations != 0 ) {
		sprintf_s(buf, 1024, "taucs.solve.maxits=%d", m_nSolverMaxIterations);
		m_vOptionStrings.push_back(buf);
	}
	if ( m_fSolverConvergeTolerance != 0 ) {
		sprintf_s(buf, 1024, "taucs.solve.convergetol=%e",m_fSolverConvergeTolerance);
		m_vOptionStrings.push_back(buf);
	}


	// make taucs-style option array 
	size_t nOptions = m_vOptionStrings.size();
	m_vOptions = std::vector<char *>(nOptions+1, NULL);
	for ( unsigned int i = 0; i < nOptions; ++i )
		m_vOptions[i] = const_cast<char *>( m_vOptionStrings[i].c_str() );
	m_vOptions[nOptions] = NULL;


	// factor!
	m_pTaucsFactorMatrix = NULL;
	fprintf(stderr,"[TAUCS] Factoring...\n");
	int nRetCode = taucs_linsolve(pTaucsMatrix, &m_pTaucsFactorMatrix, 0, NULL, NULL, &m_vOptions[0], NULL);

	return nRetCode == TAUCS_SUCCESS;
}


bool SparseSymmetricMatrixSolver::Solve_Factor_TAUCS()
{
	if ( ! m_pTaucsFactorMatrix ) 
		return false;

	// construct linear RHS vector
	std::vector<double> vRHS(m_nRows * m_vRHS.size());
	for ( unsigned int i = 0; i < m_vRHS.size(); ++i ) {
		for ( unsigned int j = 0; j < m_nRows; ++j )
			vRHS[i*m_nRows+j] = m_vRHS[i][j];
	}
	std::vector<double> vSolutions(vRHS.size());

	m_vOptionStrings[0] = std::string("taucs.factor=false");
	m_vOptions[0] = const_cast<char *>( m_vOptionStrings[0].c_str() );

	fprintf(stderr,"[TAUCS] Solving...\n");
	int nRetCode = taucs_linsolve((taucs_ccs_matrix *)m_pTaucsMatrix, &m_pTaucsFactorMatrix, (int)m_vRHS.size(), &vSolutions[0], &vRHS[0], &m_vOptions[0], NULL);
//	int nRetCode = taucs_linsolve((taucs_ccs_matrix *)m_pTaucsMatrix, &m_pTaucsFactorMatrix, (int)m_vRHS.size(), &vSolutions[0], &vRHS[0], NULL, NULL);

	// break up linear solution vector
	for ( unsigned int i = 0; i < m_vRHS.size(); ++i ) {
		for ( unsigned int j = 0; j < m_nRows; ++j )
			m_vSolutions[i][j] = vSolutions[i*m_nRows+j];
	}

	return nRetCode == TAUCS_SUCCESS;
}

bool SparseSymmetricMatrixSolver::Cleanup_Factor_TAUCS()
{
	taucs_ccs_matrix * pTaucsMatrix = (taucs_ccs_matrix *)m_pTaucsMatrix;
	taucs_ccs_free(pTaucsMatrix);
	m_pTaucsMatrix = NULL;

	fprintf(stderr,"[TAUCS] Cleanup...\n");
	int nRetCode = taucs_linsolve(NULL, &m_pTaucsFactorMatrix, 0, NULL, NULL, NULL, NULL);
	m_pTaucsFactorMatrix = NULL;

	return nRetCode == TAUCS_SUCCESS;
}
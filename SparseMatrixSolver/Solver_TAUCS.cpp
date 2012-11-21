#include "Solver_TAUCS.h"

#include <math.h>

extern "C" {
#include <taucs.h>
}


using namespace gsi;


Solver_TAUCS::Solver_TAUCS(SparseLinearSystem * pSystem)
{
	m_pSystem = pSystem;

	m_ePreconditionerMode = NoPreconditoner;
	m_eSolverMode = TAUCS_LLT;
	m_eOrderMode = TAUCS_AUTOMATIC;

	m_nSolverMaxIterations = 0;		// use default
	m_fSolverConvergeTolerance = 0; // use default

	m_logfile = "";

	m_pTaucsMatrix = NULL;
	m_pTaucsFactorMatrix = NULL;

	m_bStoreFactorization = false;
}

Solver_TAUCS::~Solver_TAUCS(void)
{
	m_pSystem = NULL;
}


void Solver_TAUCS::SetStoreFactorization(bool bEnable)
{
	m_bStoreFactorization = bEnable;
	if (! bEnable )
		free_taucs_matrix();
}

void Solver_TAUCS::OnMatrixChanged()
{
	free_taucs_matrix();
}




bool Solver_TAUCS::init_options()
{

	// set logfile if desired
	if ( m_logfile != std::string("") )
		taucs_logfile(const_cast<char *>(m_logfile.c_str()));

	m_vOptionStrings.resize(0);

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
			m_vOptionStrings.push_back( std::string("taucs.solve.cg=true") );		break;
		case TAUCS_MinRes:
			m_vOptionStrings.push_back( std::string("taucs.solve.minres=true") );	break;
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
		case TAUCS_MaximumWeightBasis:
			m_vOptionStrings.push_back( "taucs.approximate.amwb=true" );		break;

		case NoPreconditoner:
		default:
			break;
	}

	// set convergence criteria
	char buf[1024];
	if ( m_nSolverMaxIterations != 0 ) {
	#ifdef WIN32
		sprintf_s(buf, 1024, "taucs.solve.maxits=%d", m_nSolverMaxIterations);
		#else
		sprintf(buf, "taucs.solve.maxits=%d", m_nSolverMaxIterations);
		#endif
		m_vOptionStrings.push_back(buf);
	}
	if ( m_fSolverConvergeTolerance != 0 ) {
	#ifdef WIN32
		sprintf_s(buf, 1024, "taucs.solve.convergetol=%e",m_fSolverConvergeTolerance);
		#else
		sprintf(buf, "taucs.solve.convergetol=%e",m_fSolverConvergeTolerance);
		#endif
		m_vOptionStrings.push_back(buf);
	}

	return true;
}




class MatrixBuilder_TAUCS : public SparseMatrix::IColumnFunction {
public:
	taucs_ccs_matrix * pMatrix;
	bool bUseLowerTriangular;
	int * nCounter;
	virtual void NextEntry( unsigned int r, unsigned int c, double dVal ) {
		if ( !bUseLowerTriangular || r <= c ) {
			pMatrix->rowind[*nCounter] = r;
			pMatrix->values.d[*nCounter] = dVal;
			*nCounter = *nCounter+1;
		}		
	}
};


bool Solver_TAUCS::allocate_taucs_matrix()
{
	free_taucs_matrix();

	const SparseMatrix & matrix = m_pSystem->Matrix();

	// figure out number of non-zeros in lower-triangular matrix
	bool bUseLowerTriangular = true;
	unsigned int nNNZ = matrix.CountNonZeros(bUseLowerTriangular);
	unsigned int nRows = matrix.Rows();
	unsigned int nCols = matrix.Columns();

	// allocate taucs matrix
	int create_flags = (bUseLowerTriangular) ? 
		TAUCS_DOUBLE | TAUCS_SYMMETRIC | TAUCS_TRIANGULAR | TAUCS_LOWER    :    TAUCS_DOUBLE | TAUCS_SYMMETRIC;
	m_pTaucsMatrix = taucs_ccs_create(nRows, nCols, nNNZ, create_flags);
	if ( ! m_pTaucsMatrix )
		return false;

	taucs_ccs_matrix * pTaucsMatrix = (taucs_ccs_matrix *)m_pTaucsMatrix;

	// initialize taucs matrix data - only fill lower-triangular part of matrix!
	int nCur = 0;
	for ( unsigned int c = 0; c < nCols; ++c ) {
		pTaucsMatrix->colptr[c] = nCur;

		MatrixBuilder_TAUCS m;
		m.pMatrix = pTaucsMatrix;
		m.bUseLowerTriangular = bUseLowerTriangular;
		m.nCounter = &nCur;
		matrix.ApplyColumnFunction(c, &m);
	}
	pTaucsMatrix->colptr[nCols] = nCur;	

	return true;
}


void Solver_TAUCS::free_taucs_matrix()
{
	if ( m_pTaucsMatrix != NULL )
		taucs_ccs_free((taucs_ccs_matrix *)m_pTaucsMatrix);
	m_pTaucsMatrix = NULL;


	if ( m_pTaucsFactorMatrix != NULL ) {
		int nRetCode = taucs_linsolve(NULL, &m_pTaucsFactorMatrix, 0, NULL, NULL, NULL, NULL);
		m_pTaucsFactorMatrix = NULL;
	}
}



bool Solver_TAUCS::Solve()
{
	if ( m_bStoreFactorization && m_pTaucsFactorMatrix == NULL) {
		Compute_Factorization();
	}

	if ( m_pTaucsFactorMatrix )
		return Solve_Factorization();
	else
		return Solve_Direct();
}



bool Solver_TAUCS::Solve_Direct()
{
	if ( ! allocate_taucs_matrix() )
		return false;

	if (! init_options() )
		return false;

	// make taucs-style option array 
	size_t nOptions = m_vOptionStrings.size();
	m_vOptions = std::vector<char *>(nOptions+1, NULL);
	for ( unsigned int i = 0; i < nOptions; ++i )
		m_vOptions[i] = const_cast<char *>( m_vOptionStrings[i].c_str() );
	m_vOptions[nOptions] = NULL;

	// pack up RHS vectors so we can pass a block-pointer
	unsigned int nCols = m_pSystem->Matrix().Columns();
	unsigned int nRHS = m_pSystem->NumRHS();
	m_vRHS.resize(nRHS*nCols);
	m_vSolution.resize(nRHS*nCols);
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		const Vector & v = m_pSystem->GetRHS(i);
		for ( unsigned int j = 0; j < nCols; ++j )
			m_vRHS[i*nCols + j] = v[j];
	}

	// solve!
	int nRetCode = taucs_linsolve((taucs_ccs_matrix *)m_pTaucsMatrix, NULL, nRHS, &m_vSolution[0], &m_vRHS[0], &m_vOptions[0], NULL);

	// unpack solution
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		Vector & v = m_pSystem->GetSolution(i);
		for ( unsigned int j = 0; j < nCols; ++j )
			v[j] = m_vSolution[i*nCols + j];
	}

	free_taucs_matrix();

	return nRetCode == TAUCS_SUCCESS;
}



bool Solver_TAUCS::Compute_Factorization()
{
	if ( m_eSolverMode == TAUCS_ConjGradient || m_eSolverMode == TAUCS_MinRes )
		return false;

	if ( ! allocate_taucs_matrix() )
		return false;

	if (! init_options() )
		return false;

	m_vOptionStrings.push_back("taucs.factor=true");

	// make taucs-style option array 
	size_t nOptions = m_vOptionStrings.size();
	m_vOptions = std::vector<char *>(nOptions+1, NULL);
	for ( unsigned int i = 0; i < nOptions; ++i )
		m_vOptions[i] = const_cast<char *>( m_vOptionStrings[i].c_str() );
	m_vOptions[nOptions] = NULL;

	// factor!
	m_pTaucsFactorMatrix = NULL;
	int nRetCode = taucs_linsolve((taucs_ccs_matrix *)m_pTaucsMatrix, &m_pTaucsFactorMatrix, 0, NULL, NULL, &m_vOptions[0], NULL);

	return nRetCode == TAUCS_SUCCESS;
}


bool Solver_TAUCS::Solve_Factorization()
{
	if ( ! m_pTaucsFactorMatrix ) 
		return false;

	// construct linear RHS vector
	// pack up RHS vectors so we can pass a block-pointer
	unsigned int nCols = m_pSystem->Matrix().Columns();
	unsigned int nRHS = m_pSystem->NumRHS();
	m_vRHS.resize(nRHS*nCols);
	m_vSolution.resize(nRHS*nCols);
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		const Vector & v = m_pSystem->GetRHS(i);
		for ( unsigned int j = 0; j < nCols; ++j )
			m_vRHS[i*nCols + j] = v[j];
	}

	m_vOptionStrings.back() = std::string("taucs.factor=false");
	m_vOptions[m_vOptions.size()-2] = const_cast<char *>( m_vOptionStrings.back().c_str() );

	int nRetCode = taucs_linsolve((taucs_ccs_matrix *)m_pTaucsMatrix, &m_pTaucsFactorMatrix, (int)nRHS, &m_vSolution[0], &m_vRHS[0], &m_vOptions[0], NULL);

	// unpack solution
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		Vector & v = m_pSystem->GetSolution(i);
		for ( unsigned int j = 0; j < nCols; ++j )
			v[j] = m_vSolution[i*nCols + j];
	}

	return nRetCode == TAUCS_SUCCESS;
}



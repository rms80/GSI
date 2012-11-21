#include "SparseLinearSystem.h"

#include <math.h>

using namespace gsi;









/*
 * SparseLinearSystem class
 */






SparseLinearSystem::SparseLinearSystem(unsigned int nRows, unsigned int nCols)
{
	Resize(nRows,nCols);
}

SparseLinearSystem::~SparseLinearSystem(void)
{
}


const SparseMatrix & SparseLinearSystem::Matrix() const
{
	return m_matrix;
}

SparseMatrix & SparseLinearSystem::Matrix()
{
	return m_matrix;
}


void SparseLinearSystem::SetMatrix( const SparseMatrix & vSet )
{
	m_matrix = vSet;

	for ( unsigned int i = 0; i < m_vRHS.size(); ++i )
		m_vRHS[i].Resize(m_matrix.Columns());
	for ( unsigned int i = 0; i < m_vSolutions.size(); ++i )
		m_vSolutions[i].Resize(m_matrix.Columns());
}


void SparseLinearSystem::Resize( unsigned int nRows, unsigned int nColumns )
{
	m_matrix.Resize(nRows,nColumns);
	for ( unsigned int i = 0; i < m_vRHS.size(); ++i )
		m_vRHS[i].Resize(m_matrix.Columns());
	for ( unsigned int i = 0; i < m_vSolutions.size(); ++i )
		m_vSolutions[i].Resize(m_matrix.Columns());
}


void SparseLinearSystem::ResizeRHS( unsigned int nRHS, bool bSetToZero )
{
	m_vRHS.resize(nRHS);
	m_vSolutions.resize(nRHS);

	unsigned int nSize = m_matrix.Columns();
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		m_vRHS[i].Resize( nSize );
		m_vSolutions[i].Resize( nSize );
		if ( bSetToZero ) {
			m_vRHS[i].Clear();
			m_vSolutions[i].Clear();
		}
	}
}


void SparseLinearSystem::Set( unsigned int r, unsigned int c, double dValue )
{
	m_matrix.Set(r,c,dValue);
}

double SparseLinearSystem::Get( unsigned int r, unsigned int c ) const
{
	return m_matrix.Get(r,c);
}


unsigned int SparseLinearSystem::NumRHS() const
{
	return (unsigned int)m_vRHS.size();
}


const Vector & SparseLinearSystem::GetRHS( unsigned int iRHS ) const
{
	return m_vRHS[iRHS];
}
Vector & SparseLinearSystem::GetRHS( unsigned int iRHS )
{
	return m_vRHS[iRHS];
}


bool SparseLinearSystem::SetRHS( unsigned int iRHS, const Vector & vSet )
{
	if ( vSet.Size() != m_matrix.Columns() )
		return false;
	m_vRHS[iRHS] = vSet;
	return true;
}

void SparseLinearSystem::SetRHS( unsigned int i, double dValue, unsigned int iRHS)
{
	m_vRHS[iRHS][i] = dValue;
}

double SparseLinearSystem::GetRHS( unsigned int i, unsigned int iRHS ) const
{
	return m_vRHS[iRHS][i];
}



bool SparseLinearSystem::MultiplyRHS( const SparseMatrix & vMatrix )
{
	if ( m_vRHS.empty() )
		return true;
	size_t nSize = m_vRHS[0].Size();
	if ( vMatrix.Rows() != nSize )
		return false;
	size_t nRHS = m_vRHS.size();
	for ( unsigned int k = 0; k < nRHS; ++k )
		m_vRHS[k] = vMatrix * m_vRHS[k];
	return true;
}




const Vector & SparseLinearSystem::GetSolution( unsigned int iRHS ) const
{
	return m_vSolutions[iRHS];
}
Vector & SparseLinearSystem::GetSolution( unsigned int iRHS )
{
	return m_vSolutions[iRHS];
}
double SparseLinearSystem::GetSolution( unsigned int i, unsigned int iRHS ) const
{
	return m_vSolutions[iRHS][i];
}




bool SparseLinearSystem::Solve()
{
	return false;
}


bool SparseLinearSystem::Factorize()
{
	return false;
}

void SparseLinearSystem::ClearFactorization()
{
}


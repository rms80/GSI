#include "DenseLinearSystem.h"

#include <math.h>

using namespace gsi;


DenseLinearSystem::DenseLinearSystem(unsigned int nRows, unsigned int nCols)
{
	Resize(nRows,nCols);
}

DenseLinearSystem::~DenseLinearSystem(void)
{
}


const DenseMatrix & DenseLinearSystem::Matrix() const
{
	return m_matrix;
}

DenseMatrix & DenseLinearSystem::Matrix()
{
	return m_matrix;
}


void DenseLinearSystem::SetMatrix( const DenseMatrix & vSet )
{
	m_matrix = vSet;

	for ( unsigned int i = 0; i < m_vRHS.size(); ++i )
		m_vRHS[i].Resize(m_matrix.Columns());
	for ( unsigned int i = 0; i < m_vSolutions.size(); ++i )
		m_vSolutions[i].Resize(m_matrix.Columns());
}


void DenseLinearSystem::Resize( unsigned int nRows, unsigned int nColumns )
{
	m_matrix.Resize(nRows,nColumns);
	for ( unsigned int i = 0; i < m_vRHS.size(); ++i )
		m_vRHS[i].Resize(m_matrix.Columns());
	for ( unsigned int i = 0; i < m_vSolutions.size(); ++i )
		m_vSolutions[i].Resize(m_matrix.Columns());
}


void DenseLinearSystem::ResizeRHS( unsigned int nRHS, bool bSetToZero )
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


void DenseLinearSystem::Set( unsigned int r, unsigned int c, double dValue )
{
	m_matrix.Set(r,c,dValue);
}

double DenseLinearSystem::Get( unsigned int r, unsigned int c ) const
{
	return m_matrix.Get(r,c);
}


unsigned int DenseLinearSystem::NumRHS() const
{
	return (unsigned int)m_vRHS.size();
}


const Vector & DenseLinearSystem::GetRHS( unsigned int iRHS ) const
{
	return m_vRHS[iRHS];
}
Vector & DenseLinearSystem::GetRHS( unsigned int iRHS )
{
	return m_vRHS[iRHS];
}


bool DenseLinearSystem::SetRHS( unsigned int iRHS, const Vector & vSet )
{
	if ( vSet.Size() != m_matrix.Columns() )
		return false;
	m_vRHS[iRHS] = vSet;
	return true;
}

void DenseLinearSystem::SetRHS( unsigned int i, double dValue, unsigned int iRHS)
{
	m_vRHS[iRHS][i] = dValue;
}

double DenseLinearSystem::GetRHS( unsigned int i, unsigned int iRHS ) const
{
	return m_vRHS[iRHS][i];
}




const Vector & DenseLinearSystem::GetSolution( unsigned int iRHS ) const
{
	return m_vSolutions[iRHS];
}
Vector & DenseLinearSystem::GetSolution( unsigned int iRHS )
{
	return m_vSolutions[iRHS];
}
double DenseLinearSystem::GetSolution( unsigned int i, unsigned int iRHS ) const
{
	return m_vSolutions[iRHS][i];
}




bool DenseLinearSystem::Solve()
{
	return false;
}


bool DenseLinearSystem::Factorize()
{
	return false;
}

void DenseLinearSystem::ClearFactorization()
{
}



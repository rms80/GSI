#include "Vector.h"

#include <cmath>
#include <cstdlib>

using namespace gsi;

Vector::Vector(size_t nSize)
{
	m_pValues = NULL;
	Resize(nSize);
}

Vector::Vector(const Vector & copy)
{
	m_pValues = NULL;
	*this = copy;
}

Vector::~Vector()
{
	if ( m_pValues != NULL )
		delete [] m_pValues;
}

const Vector & Vector::operator=(const Vector & copy)
{
	Resize(copy.m_nSize);
	for ( unsigned int k = 0; k < m_nSize; ++k )
		m_pValues[k] = copy.m_pValues[k];
	return *this;
}


void Vector::Resize( size_t nSize )
{
	if ( m_pValues != NULL ) {
		delete [] m_pValues;
		m_pValues = NULL;
	}
	if ( nSize > 0 )
		m_pValues = new double[nSize];
	m_nSize = nSize;
}

size_t Vector::Size() const
{
	return m_nSize;
}

void Vector::Set( unsigned int i, double dValue )
{
	m_pValues[i] = dValue;
}

double Vector::Get( unsigned int i ) const
{
	return m_pValues[i];
}

void Vector::Clear()
{
	for ( unsigned int i = 0; i < m_nSize; ++i )
		m_pValues[i] = 0;
}


const double & Vector::operator[](unsigned int i) const
{
	return m_pValues[i];
}
double & Vector::operator[](unsigned int i)
{
	return m_pValues[i];
}


double * Vector::GetValues() { return m_pValues; }
const double * Vector::GetValues() const { return m_pValues; }


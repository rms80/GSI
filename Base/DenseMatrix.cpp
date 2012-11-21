#include "DenseMatrix.h"

#include <math.h>

using namespace gsi;


#define M(row,col) ( m_vMatrix[((row)*(m_nCols)) + (col)] )
#define M_B(row,col) ( B.m_vMatrix[((row)*(B.m_nCols)) + (col)] )
#define M_C(row,col) ( C.m_vMatrix[((row)*(C.m_nCols)) + (col)] )


DenseMatrix::DenseMatrix(unsigned int nRows, unsigned int nCols)
{
	Resize(nRows, nCols);
}

DenseMatrix::DenseMatrix(const DenseMatrix & copy)
{
	*this = copy;
}

DenseMatrix::~DenseMatrix(void)
{
}


DenseMatrix & DenseMatrix::operator=(const DenseMatrix & copy)
{
	Resize(copy.Rows(), copy.Columns());
	m_vMatrix = copy.m_vMatrix;
	return *this;
}


unsigned int DenseMatrix::Rows() const 
{ 
	return m_nRows; 
}

unsigned int DenseMatrix::Columns()  const
{ 
	return m_nCols; 
}

void DenseMatrix::Set( unsigned int r, unsigned int c, double dValue )
{
	M(r,c) = dValue;
}

double DenseMatrix::Get( unsigned int r, unsigned int c) const
{
	return M(r,c);
}

DenseMatrix::operator double *()
{
	return &m_vMatrix[0];
}
DenseMatrix::operator const double *()
{
	return &m_vMatrix[0];
}

void DenseMatrix::Clear(bool bFree)
{
	m_vMatrix.resize(0);
	m_nRows = m_nCols = 0;
	if (bFree) 
		m_vMatrix.clear();
}

void DenseMatrix::ClearRow( unsigned int r, double fValue )
{
	for ( unsigned int c = 0; c < m_nCols; ++c )
		M(r,c) = fValue;
}



void DenseMatrix::Resize( unsigned int nRows, unsigned int nCols, double fInitValue, bool bClearFirst )
{
	if ( bClearFirst )
		Clear(true);
	m_vMatrix.resize(nRows*m_nCols + nCols, fInitValue);
	m_nRows = nRows;
	m_nCols = nCols;
}



unsigned int DenseMatrix::CountNonZeros(bool bLowerOnly) const
{
	unsigned int nCount = 0;
	for ( unsigned int r = 0; r < m_nRows; ++r ) {
		for ( unsigned int c = 0; c < m_nCols; ++c ) {
			if ( M(r,c) != 0 )
				++nCount;
		}
	}
	return nCount;
}


bool DenseMatrix::IsSymmetric(double dThresh, double * pLargestError ) const
{
	double dMaxErr = 0;
	for ( unsigned int r = 0; r < m_nRows; ++r ) {
		for ( unsigned int c = r; c < m_nCols; ++c) {
			double d1 = M(r,c);
			double d2 = M(c,r);
			if ( d1 != d2 ) {
				double dErr = d1 - d2;
				if ( fabs(dErr) > fabs(dMaxErr) )
					dMaxErr = dErr;
			}
		}
	}
	dMaxErr = fabs(dMaxErr);
	if ( pLargestError )
		*pLargestError = dMaxErr;
	return dMaxErr < dThresh;
}



// returns largest non-symmetric error
bool DenseMatrix::IsPositiveDefinite(bool * bIsSemiDefinite) const
{
	size_t nRows = Rows();
	if ( nRows != Columns() )
		return false;

	bool bIsPosDef = true;
	bool bIsPosSemiDef = true;

	for ( unsigned int r = 0; r < nRows && ( bIsPosDef || bIsPosSemiDef ); ++r ) {
		double dDiag = Get(r,r);
		bIsPosDef = bIsPosDef && dDiag > 0;
		bIsPosSemiDef = bIsPosSemiDef && dDiag >= 0;

		for ( unsigned int c = 0; c < m_nCols; ++c ) {
			if ( c == r )
				continue;
			double dVal = M(r,c);
			bIsPosDef = bIsPosDef && dDiag > dVal;
			bIsPosSemiDef = bIsPosSemiDef && dDiag >= dVal;
		}
	}
	if ( bIsSemiDefinite )
		*bIsSemiDefinite = bIsPosSemiDef;
	return bIsPosDef;
}


//! set matrix to identity
void DenseMatrix::ToIdentity()
{
	Resize(m_nRows, m_nCols);
	for ( unsigned int c = 0; c < m_nCols; ++c )
		M(c,c) = 1.0f;
}


//! compute transpose of matrix
void DenseMatrix::Transpose(DenseMatrix & C) const
{
	C.Resize(m_nCols,m_nRows);
	for ( unsigned int r = 0; r < m_nRows; ++r ) {
		for ( unsigned int c = 0; c < m_nCols; ++c ) {
			M_C(c,r) = M(r,c);
		}
	}
}



//! compute this = this + B
bool DenseMatrix::Add( const DenseMatrix & B )
{
	if ( Rows() != B.Rows() || Columns() != B.Columns() )
		return false;			// matrices are incompatible

	for ( unsigned int r = 0; r < m_nRows; ++r ) {
		for ( unsigned int c = 0; c < m_nCols; ++c ) {
			M(r,c) += M_B(c,r);
		}
	}
	return true;
}

//! compute this = this - B
bool DenseMatrix::Subtract( const DenseMatrix & B )
{
	if ( Rows() != B.Rows() || Columns() != B.Columns() )
		return false;			// matrices are incompatible

	for ( unsigned int r = 0; r < m_nRows; ++r ) {
		for ( unsigned int c = 0; c < m_nCols; ++c ) {
			M(r,c) -= M_B(c,r);
		}
	}
	return true;
}

//! compute C = this + B
bool DenseMatrix::Add(const DenseMatrix & B, DenseMatrix & C ) const
{
	C = *this;
	return C.Add(B);
}

//! compute C = this - B
bool DenseMatrix::Subtract(const DenseMatrix & B, DenseMatrix & C ) const
{
	C = *this;
	return C.Subtract(B);
}


//! compute this = val * this
void DenseMatrix::Multiply( double dVal )
{
	if ( dVal == 0 ) {
		Resize(m_nRows, m_nCols);
		return;
	}
	if ( dVal == 1 )
		return;

	for ( unsigned int r = 0; r < m_nRows; ++r ) {
		for ( unsigned int c = 0; c < m_nCols; ++c ) {
			M(r,c) *= dVal;
		}
	}
}



//! compute C = this*B;
bool DenseMatrix::Multiply(const DenseMatrix & B, DenseMatrix & C) const
{
	unsigned int nRowsA = Rows();
	unsigned int nColsA = Columns();
	unsigned int nRowsB = B.Rows();
	unsigned int nColsB = B.Columns();

	if ( nColsA != nRowsB )
		return false;			// matrices are incompatible

	C.Resize(nRowsA,nColsB);
	for ( unsigned int ra = 0; ra < nRowsA; ++ra ) {
		for ( unsigned int cb = 0; cb < nColsB; ++cb ) {

			double rcsum = 0;
			for ( unsigned int ca = 0; ca < nColsA; ++ca )
				rcsum += M(ra,ca) * M_B(ca,cb);

			M_C(ra,cb) = rcsum;
		}
	}

	return true;
}


//! compute C = this*B;
bool DenseMatrix::Multiply(const Vector & B, Vector & C) const
{
	unsigned int nRows = Rows();
	unsigned int nCols = Columns();
	if ( nCols != B.Size() )
		return false;			// incompatible sizes

	C.Resize(nRows);
	for ( unsigned int ra = 0; ra < nRows; ++ra ) {
		double rsum = 0;
		for ( unsigned int ca = 0; ca < nCols; ++ca )
			rsum += M(ra,ca) * B[ca];

		C[ra] = rsum;
	}

	return true;
}





void DenseMatrix::Print(FILE * out) const
{
	for ( unsigned int r = 0; r < Rows(); ++r  ) {
		fprintf(out, "Row %2d: ", r);
		for ( unsigned int c = 0; c < Columns(); ++c )
			fprintf(out, "%3.2f  ", M(r,c));
		fprintf(out, "\r\n");
	}
}






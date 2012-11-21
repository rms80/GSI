#include "SparseMatrix.h"

#include <vector>
#include <set>
#include <cmath>
#include <cstdlib>

using namespace gsi;


struct ColumnEntry {
	unsigned int r;
	unsigned int c;
	double value;
	ColumnEntry() { r = 0; c = 0; value = 0; }
	ColumnEntry(unsigned int row, unsigned int col) { r = row; c = col; value = 0; }
	ColumnEntry(unsigned int row, unsigned int col, double val) { r = row; c = col; value = val; }
	bool operator<( const ColumnEntry & e2 ) const { return r < e2.r; }
};
typedef std::set<ColumnEntry> Internal_Column;

class SparseMatrix::Column
{
public:
	Internal_Column c;
};


class ColumnIterCompare { 
public: 
	bool operator()( const Internal_Column::iterator & i, const Internal_Column::iterator & j ) const 
		{ 	return (*i).c < (*j).c;	} 
};
typedef std::set< Internal_Column::iterator, ColumnIterCompare > Internal_Row;


class SparseMatrix::Row 
{
public:
	Internal_Row r;
};


SparseMatrix::SparseMatrix(unsigned int nRows, unsigned int nCols)
{
	m_pColumns = NULL;
	m_pRows = NULL;
	Resize(nRows, nCols);
}

SparseMatrix::SparseMatrix(const SparseMatrix & copy)
{
	m_pColumns = NULL;
	m_pRows = NULL;
	*this = copy;
}

SparseMatrix::~SparseMatrix(void)
{
	if ( m_pColumns )
		delete [] m_pColumns;
	if ( m_pRows )
		delete [] m_pRows;
}


SparseMatrix & SparseMatrix::operator=(const SparseMatrix & copy)
{
	unsigned int nCols = copy.Columns();
	Resize(copy.Rows(), nCols);
	for ( unsigned int c = 0; c < nCols; ++c ) {
		m_pColumns[c] = copy.m_pColumns[c];
		Column & col = m_pColumns[c];
		Internal_Column::iterator ccur(col.c.begin()), cend(col.c.end());
		while ( ccur != cend )  {
			Internal_Column::iterator coli = ccur++;
			m_pRows[(*coli).r].r.insert(coli);
		}
	}
	return *this;
}


unsigned int SparseMatrix::Rows() const 
{ 
	return m_nRows; 
}

unsigned int SparseMatrix::Columns()  const
{ 
	return m_nCols; 
}

void SparseMatrix::Set( unsigned int r, unsigned int c, double dValue )
{
	std::pair<Internal_Column::iterator, bool> inserted = m_pColumns[c].c.insert( ColumnEntry(r,c,dValue) );
	if ( ! inserted.second ) {
		(*inserted.first).value = dValue;
	} else {
		Row & row = m_pRows[r];
		Internal_Column::iterator coli = inserted.first;
		row.r.insert(coli);
	}
}

double SparseMatrix::Get( unsigned int r, unsigned int c) const
{
	Internal_Column::const_iterator found(m_pColumns[c].c.find(ColumnEntry(r,c)));
	return (found == m_pColumns[c].c.end()) ? 0.0f : (*found).value;
}



double SparseMatrix::Entry::operator=(double dValue) const
{
	pMatrix->Set(this->r, this->c, dValue);
	return dValue;
}
double SparseMatrix::Entry::operator=(const SparseMatrix::Entry & e2) const
{
	double dValue = (double)e2;
	pMatrix->Set(this->r, this->c, dValue );	
	return dValue;
}
SparseMatrix::Entry::operator double() const
{
	return pMatrix->Get(this->r,this->c);
}
SparseMatrix::Entry SparseMatrix::operator()(unsigned int r, unsigned int c)
{
	Entry e; e.r = r; e.c = c; e.pMatrix = this;
	return e;
}




SparseMatrix::ConstEntry::operator double() const
{
	return pMatrix->Get(this->r,this->c);
}
SparseMatrix::ConstEntry SparseMatrix::operator()(unsigned int r, unsigned int c) const
{
	ConstEntry e; e.r = r; e.c = c; e.pMatrix = this;
	return e;
}




SparseMatrix::Column * SparseMatrix::GetColumn(unsigned int c)
{
	return & m_pColumns[c];
}
const SparseMatrix::Column * SparseMatrix::GetColumn(unsigned int c) const
{
	return & m_pColumns[c];
}


SparseMatrix::Row * SparseMatrix::GetRow(unsigned int r)
{
	return &m_pRows[r];
}
const SparseMatrix::Row * SparseMatrix::GetRow(unsigned int r) const
{
	return &m_pRows[r];
}



void SparseMatrix::Clear(bool bFree)
{
	if (bFree) {
		if ( m_pColumns ) {
			delete [] m_pColumns;
			m_pColumns = NULL;
		}
		if ( m_pRows ) {
			delete [] m_pRows;
			m_pRows = NULL;
		}
	} else {
		for ( unsigned int i = 0; i < m_nCols; ++i )
			m_pColumns[i].c.clear();
		for ( unsigned int i = 0; i < m_nRows; ++i )
			m_pRows[i].r.clear();
	}
}

void SparseMatrix::ClearRow( unsigned int r )
{
	ColumnEntry e(r, 0);
	for ( unsigned int i = 0; i < m_nCols; ++i ) {
		Internal_Column::iterator found( m_pColumns[i].c.find(e) );
		if ( found != m_pColumns[i].c.end() )
			m_pColumns[i].c.erase(found);
	}
	m_pRows[r].r.clear();
}


void SparseMatrix::ToRandom( bool bSymmetric )
{
	srand(3131771);
	for ( unsigned int r = 0; r < Rows(); ++r ) {
		for ( unsigned int c = (bSymmetric) ? r : 0; c < Columns(); ++c ) {
			//double dValue = (double)rand() / (double)RAND_MAX;
			double dValue = rand() % 50;
			Set(r,c,dValue);
			if ( bSymmetric )
				Set(c,r,dValue);
		}
	}
}





void SparseMatrix::Resize( unsigned int nRows, unsigned int nCols )
{
	Clear(true);

	if ( m_pColumns != NULL ) {
		delete [] m_pColumns;
		m_pColumns = NULL;
	}
	if ( nCols > 0 )
		m_pColumns = new Column[nCols];

	if ( m_pRows != NULL ) {
		delete [] m_pRows;
		m_pRows = NULL;
	}
	if ( nRows > 0 )
		m_pRows = new Row[nRows];

	m_nRows = nRows;
	m_nCols = nCols;
}



void SparseMatrix::ApplyColumnFunction( unsigned int c, IColumnFunction * f ) const
{
	const SparseMatrix::Column & col = *GetColumn(c);
	Internal_Column::const_iterator cur(col.c.begin()), end(col.c.end());
	while ( cur != end ) {
		const ColumnEntry & e = *cur;  ++cur;
		f->NextEntry(e.r, c, e.value);
	}
}



unsigned int SparseMatrix::CountNonZeros(bool bLowerOnly) const
{
	unsigned int nCount = 0;
	size_t nCols = m_nCols;
	for ( unsigned int i = 0; i < nCols; ++i ) {
		if ( bLowerOnly ) {
			Internal_Column::const_iterator cur(m_pColumns[i].c.begin()), end(m_pColumns[i].c.end());
			while ( cur != end ) {
				const ColumnEntry & e = *cur;  ++cur;
				if ( e.r <= i )
					nCount++;
			}
		} else 
			nCount += (unsigned int)m_pColumns[i].c.size();
	}
	return nCount;
}


bool SparseMatrix::IsSymmetric(double dThresh, double * pLargestError ) const
{
	double dMaxErr = 0;
	//for ( unsigned int r = 0; r < m_nRows; ++r ) {
	//	for ( unsigned int c = r; c < m_nCols; ++c) {
	//		double d1 = Get(r,c);
	//		double d2 = Get(c,r);
	//		if ( d1 != d2 ) {
	//			double dErr = d1 - d2;
	//			if ( fabs(dErr) > fabs(dMaxErr) )
	//				dMaxErr = dErr;
	//		}
	//	}
	//}
	size_t nCols = Columns();
	for ( unsigned int c = 0; c < nCols; ++c ) {
		Internal_Column::const_iterator cur(m_pColumns[c].c.begin()), end(m_pColumns[c].c.end());
		while ( cur != end ) {
			const ColumnEntry & e = *cur;  ++cur;
			double d1 = e.value;
			double d2 = Get(c, e.r);
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
bool SparseMatrix::IsPositiveDefinite(bool * bIsSemiDefinite) const
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

		Internal_Row::const_iterator cur(m_pRows[r].r.begin()), end(m_pRows[r].r.end());
		while ( cur != end ) {
			Internal_Column::iterator coli = *cur++;
			if ( (*coli).r == r )
				continue;

#ifdef DEBUG
			if ( (*coli).r != r )
				abort();
#endif
			double dVal = (*coli).value;
			bIsPosDef = bIsPosDef && dDiag > dVal;
			bIsPosSemiDef = bIsPosSemiDef && dDiag >= dVal;
		}
	}
	if ( bIsSemiDefinite )
		*bIsSemiDefinite = bIsPosSemiDef;
	return bIsPosDef;
}


//! set matrix to identity
void SparseMatrix::ToIdentity()
{
	Resize(m_nRows,m_nCols);
	unsigned int nCols = Columns();
	for ( unsigned int c = 0; c < nCols; ++c )
		Set(c,c, 1);
}


//! compute transpose of matrix
void SparseMatrix::Transpose(SparseMatrix & store) const
{
	unsigned int nCols = Columns();
	store.Resize(nCols,Rows());
	for ( unsigned int c = 0; c < nCols; ++c ) {
		const Column & col = *GetColumn(c);
		Internal_Column::const_iterator ccur(col.c.begin()), cend(col.c.end());
		while ( ccur != cend )  {
			store.Set( (*ccur).c, (*ccur).r, (*ccur).value );
			++ccur;
		}
	}
}
SparseMatrix SparseMatrix::Transpose() const
{
	SparseMatrix tmp;
	Transpose(tmp);
	return tmp;
}


//! compute this = this + B
bool SparseMatrix::Add( const SparseMatrix & B )
{
	if ( Rows() != B.Rows() || Columns() != B.Columns() )
		return false;			// matrices are incompatible

	unsigned int nCols = Columns();
	for ( unsigned int c = 0; c < nCols; ++c ) {
		Internal_Column::const_iterator ccur(B.m_pColumns[c].c.begin()), cend(B.m_pColumns[c].c.end());
		while ( ccur != cend ) {
			double d = Get( (*ccur).r, c );
			Set( (*ccur).r, c, d + (*ccur).value );
			++ccur;
		}
	}
	return true;
}



//! compute this = this - B
bool SparseMatrix::Subtract( const SparseMatrix & B )
{
	if ( Rows() != B.Rows() || Columns() != B.Columns() )
		return false;			// matrices are incompatible

	unsigned int nCols = Columns();
	for ( unsigned int c = 0; c < nCols; ++c ) {
		Internal_Column::const_iterator ccur(B.m_pColumns[c].c.begin()), cend(B.m_pColumns[c].c.end());
		while ( ccur != cend ) {
			double d = Get( (*ccur).r, c );
			Set( (*ccur).r, c, d - (*ccur).value );
			++ccur;
		}
	}
	return true;
}



//! compute C = this + B
bool SparseMatrix::Add(const SparseMatrix & B, SparseMatrix & C ) const
{
	C = *this;
	return C.Add(B);
}
SparseMatrix SparseMatrix::operator+(const SparseMatrix & B) const
{
	SparseMatrix C = *this;
	C.Add(B);
	return C;
}

//! compute C = this - B
bool SparseMatrix::Subtract(const SparseMatrix & B, SparseMatrix & C ) const
{
	C = *this;
	return C.Subtract(B);
}
SparseMatrix SparseMatrix::operator-(const SparseMatrix & B) const
{
	SparseMatrix C = *this;
	C.Subtract(B);
	return C;
}


//! compute this = val * this
void SparseMatrix::Multiply( double dVal )
{
	if ( dVal == 0 ) {
		Resize(m_nRows,m_nCols);
		return;
	}

	unsigned int nCols = Columns();
	for ( unsigned int c = 0; c < nCols; ++c ) {
		Internal_Column::const_iterator ccur(m_pColumns[c].c.begin()), cend(m_pColumns[c].c.end());
		while ( ccur != cend ) {
			double d = Get( (*ccur).r, c );
			Set( (*ccur).r, c, dVal * d );
			++ccur;
		}
	}
}
SparseMatrix SparseMatrix::operator*(double dVal) const
{
	SparseMatrix C = *this;
	C.Multiply(dVal);
	return C;
}


//! compute C = this*B;
bool SparseMatrix::Multiply(const SparseMatrix & B, SparseMatrix & C) const
{
	unsigned int nRowsA = Rows();
	unsigned int nColsA = Columns();
	unsigned int nRowsB = B.Rows();
	unsigned int nColsB = B.Columns();

	if ( nColsA != nRowsB )
		return false;			// matrices are incompatible

	std::vector<unsigned int> vj;
	std::vector<double> vf;

	C.Resize(nRowsA,nColsB);
	for ( unsigned int ai = 0; ai < nRowsA; ++ai ) {
		
		// flatten current row into a linear vector
		vj.resize(0);
		vf.resize(0);
		Internal_Row::const_iterator  rcur(m_pRows[ai].r.begin()), rend(m_pRows[ai].r.end());
		while ( rcur != rend ) {
			vj.push_back( (*rcur)->c );
			vf.push_back( (*rcur)->value );
			++rcur;
		}
		size_t rowCount = vj.size();

		// compute vector product with each row
		for ( unsigned int bj = 0; bj < nColsB; ++bj) {
			unsigned int j = 0;		// index into row vector
			double rcsum = 0;

			Internal_Column::const_iterator ccur(B.m_pColumns[bj].c.begin()), cend(B.m_pColumns[bj].c.end());
			while ( j < rowCount && ccur != cend ) {
				unsigned int colrow = (*ccur).r;
				double v = (*ccur++).value;

				while ( j < rowCount && vj[j] < colrow)
					++j;
				if ( j < rowCount && vj[j] == colrow )
					rcsum += v*vf[j];
			}

			if ( rcsum != 0 )
				C.Set(ai,bj, rcsum);
		}
	}

	return true;
}
SparseMatrix SparseMatrix::operator*(const SparseMatrix & B) const
{
	SparseMatrix tmp;
	Multiply(B, tmp);
	return tmp;
}




//! compute C = this*B;
bool SparseMatrix::Multiply(const Vector & B, Vector & C) const
{
	unsigned int nRows = Rows();
	unsigned int nCols = Columns();
	if ( nCols != B.Size() )
		return false;			// incompatible sizes

	C.Resize(nRows);
	for ( unsigned int ai = 0; ai < nRows; ++ai ) {

		double rsum = 0;
		Internal_Row::const_iterator  rcur(m_pRows[ai].r.begin()), rend(m_pRows[ai].r.end());
		while ( rcur != rend ) {
			rsum += B.Get( (*rcur)->c ) * ( (*rcur)->value );
			rcur++;
		}

		C.Set(ai, rsum);
	}

	return true;
}
Vector SparseMatrix::operator*(const Vector & B) const
{
	Vector C(B.Size());
	Multiply(B, C);
	return C;
}





void SparseMatrix::PrintColumns(FILE * out) const
{
	for ( unsigned int c = 0; c < Columns(); ++c ) {
		fprintf(out, "Col %2d: ", c);
		const Column & col= *GetColumn(c);
		Internal_Column::const_iterator ccur(col.c.begin()), cend(col.c.end());
		while ( ccur != cend )  {
			fprintf(out, "(%2d,%3.2f)  ", (*ccur).r, (*ccur).value);
			++ccur;
		}
		fprintf(out, "\r\n");
	}
}

void SparseMatrix::PrintRows(FILE * out) const
{
	for ( unsigned int r = 0; r < Rows(); ++r ) {
		fprintf(out, "Row %2d: ", r);
		const Row & row = *GetRow(r);
		Internal_Row::const_iterator rcur(row.r.begin()), rend(row.r.end());
		while ( rcur != rend )  {
			fprintf(out, "(%2d,%3.2f)  ", (*rcur)->c, (*rcur)->value);
			++rcur;
		}
		fprintf(out, "\r\n");
	}
}


void SparseMatrix::DebugPrint( int (*printfunc)(const char * str, ...) ) const
{
	for ( unsigned int r = 0; r < Rows(); ++r ) {
		const Row & row = *GetRow(r);
		Internal_Row::const_iterator rcur(row.r.begin()), rend(row.r.end());
		while ( rcur != rend )  {
			(*printfunc)("(%6d,%6d) %f\r\n", r, (*rcur)->c, (*rcur)->value);
			++rcur;
		}
	}
}

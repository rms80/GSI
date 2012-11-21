// SparseMatrixSolverTests.cpp : Defines the entry point for the console application.
//


#include "config.h"

#ifdef DO_SPARSE_MATRIX_TESTS

#include "SparseMatrix.h"






void sparse_matrix_tests()
{
	gsi::SparseMatrix M(3,3);
	M.ToRandom(true);

	printf("M: \n");
	M.DebugPrint(&printf);

	gsi::SparseMatrix M2 = M*M;

	printf("M2: \n");
	M2.DebugPrint(&printf);
	
	getchar();
}

#endif // DO_SPARSE_MATRIX_TESTS


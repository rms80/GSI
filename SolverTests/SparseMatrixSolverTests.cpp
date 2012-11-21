// SparseMatrixSolverTests.cpp : Defines the entry point for the console application.
//


#include "config.h"

#ifdef DO_SPARSE_MATRIX_SOLVER_TESTS


#include "SparseLinearSystem.h"
#include "SparseSymmetricMatrixSolver.h"
#include "Solver_UMFPACK.h"
#include "Solver_TAUCS.h"



void PrintSolutions( gsi::SparseLinearSystem * pSystem )
{
	unsigned int nRHS = pSystem->NumRHS();
	unsigned int nElem = pSystem->Matrix().Columns();
	for ( unsigned int i = 0; i < nRHS; ++i ) {
		fprintf(stderr, "   [S%d]   ", i);
		for ( unsigned int c = 0; c < nElem; ++c )
			fprintf(stderr, "%d:%f ", c, pSystem->GetSolution(i).Get(c));
		fprintf(stderr,"\n");
	}
}



void test_UMFPACK( gsi::SparseLinearSystem * pSystem )
{

	fprintf(stderr, "[UMFPACK Solve]\n  ");
	gsi::Solver_UMFPACK umfpack_solve(pSystem);
	if ( umfpack_solve.Solve() )
		PrintSolutions(pSystem);
	else
		fprintf(stderr, "FAIL\n");



}




void test_TAUCS( gsi::SparseLinearSystem * pSystem )
{
	gsi::Solver_TAUCS taucs_solve(pSystem);

	//taucs_solve.SetLogFileName("stderr");


	if ( pSystem->Matrix().IsSymmetric() ) {

		fprintf(stderr, "[TAUCS Solve LLT]\n  ");
		taucs_solve.SetSolverMode(gsi::Solver_TAUCS::TAUCS_LLT);
		if ( taucs_solve.Solve() )
			PrintSolutions(pSystem);
		else
			fprintf(stderr, "FAIL\n");


		fprintf(stderr, "[TAUCS Solve MinRes]\n  ");
		taucs_solve.SetSolverMode(gsi::Solver_TAUCS::TAUCS_MinRes);
		if ( taucs_solve.Solve() ) 
			PrintSolutions(pSystem);
		else
			fprintf(stderr, "FAIL\n");

		if ( pSystem->Matrix().IsPositiveDefinite() ) {

			fprintf(stderr, "[TAUCS Solve ConjGradient]\n  ");
			taucs_solve.SetSolverMode(gsi::Solver_TAUCS::TAUCS_ConjGradient);
			if ( taucs_solve.Solve() )
				PrintSolutions(pSystem);
			else
				fprintf(stderr, "FAIL\n");

		} else 
			fprintf(stderr, "** solve_TAUCS(): matrix is not positive definite - skipping tests\n");


	} else
		fprintf(stderr, "** solve_TAUCS(): matrix is not symmetric - skipping tests\n");


	fprintf(stderr, "[TAUCS Factor/Solve LLT]\n  ");
	taucs_solve.SetSolverMode(gsi::Solver_TAUCS::TAUCS_LLT);
	taucs_solve.SetStoreFactorization(true);
	if ( taucs_solve.Solve() )
		PrintSolutions(pSystem);
	else
		fprintf(stderr, "FAILED\n");
	taucs_solve.SetStoreFactorization(false);


	fprintf(stderr, "[TAUCS Solve LU]\n  ");
	taucs_solve.SetSolverMode(gsi::Solver_TAUCS::TAUCS_LU);
	if ( taucs_solve.Solve() )
		PrintSolutions(pSystem);
	else
		fprintf(stderr, "FAIL\n");

}







void sparse_matrix_solver_tests()
{
	// [RMS] this matrix is not symmetric...
	double testmat[25] = {
		1.0000000000000000E+00,
		0.0000000000000000E+00,
		0.7609443032860241E-01,
		-0.2329363725573828E+00,
		-0.9289707748310860E-01,

		0.0000000000000000E+00,
		3.0000000000000000E+00,
		0.4694417327100169E+00,
		-0.9398923227776024E+00,
		0.0000000000000000E+00,

		0.7609443032860241E-01,
		0.4694417327100169E+00,
		0.9655072082868443E+00,
		0.2355733319969389E+00,
		-0.5561645006365978E-01,

		-0.2329363725573828E+00,
		-0.9398923227776024E+00,
		0.2355733319969389E+00,
		1.8563254093076225E+00,
		0.0000000000000000E+00,

		-0.9289707748310860E-01,
		0.0000000000000000E+00,
		-0.5561645006365978E-01,
		1.0000000000000000E+00,
		2.0000000000000000E+00 };


	// symmetrize
	for ( int r = 0; r < 5; ++r ) {
		for ( int c = 0; c < 5; ++c ) {
			double d1 = testmat[r*5+c];
			double d2 = testmat[c*5+r];
			testmat[r*5+c] = 0.5*(d1+d2);
			testmat[c*5+r] = 0.5*(d1+d2);
		}
	}


	gsi::SparseLinearSystem linsys(5,5);
	for ( int r = 0; r < 5; ++r ) {
		for ( int c = 0; c < 5; ++c ) {
			double dVal = testmat[r*5+c];
			if ( dVal != 0.0f )
				linsys.Set(r, c, dVal);
		}
	}

	linsys.ResizeRHS(1);
	for ( int r = 0; r < 5; ++r )
		linsys.SetRHS(r, (double)r, 0);

	test_UMFPACK(&linsys);
	test_TAUCS(&linsys);


	// do column and row iteration on transpose - should be the same, even if matrix is weird
	fprintf(stderr, "column iteration\r\n");
	linsys.Matrix().PrintColumns(stderr);

	gsi::SparseMatrix t;
	linsys.Matrix().Transpose(t);
	fprintf(stderr, "row iteration\r\n");
	t.PrintRows(stderr);

	gsi::SparseMatrix mmt;
	linsys.Matrix().Multiply(t, mmt);
	fprintf(stderr,"matrix * transpose(matrix)\r\n");
	mmt.PrintRows(stderr);


	getchar();
}

#endif // DO_SPARSE_MATRIX_TESTS


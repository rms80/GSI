// SolverTests.cpp : Defines the entry point for the console application.
//

#include "config.h"

#ifdef DO_DENSE_MATRIX_SOLVER_TESTS

#include <DenseSymmetricSolver.h>
#include <LAPACKWrapper.h>
#include <Architecture.h>
#include <DenseLinearSystem.h>
#include <LinearSystemSolver.h>

#include <vector>
#include <iostream>

void dense_matrix_solver_tests()
{
	rmssolver::Architecture::Get()->ForceType( rmssolver::Architecture::Unknown );
	//rmssolver::Architecture::Get()->ForceType( rmssolver::Architecture::Athlon_3DNow );
	//rmssolver::LAPACKWrapper::ForceBestPackage( rmssolver::LAPACKWrapper::INTEL_MKL );


	rmssolver::DenseSymmetricSolverd solver1(5);
	rmssolver::DenseLinearSystemd solver2(5, 5, 1);
	solver1.SetToIdentity();
	solver2.SetToIdentity();
	for ( unsigned int i = 0 ; i < solver1.GetNumRows(); ++i ) {
		solver1.SetValue( i, i, 1.0 );
		solver2.SetValue( i, i, 1.0 );
		if ( i > 0 ) {
			solver1.SetValue( i, 0, (double)i );

			solver2.SetValue( i, 0, (double)i );
			solver2.SetValue( 0, i, (double)i );
		}

		solver1.SetRHSValue(i, (double)i / 10.0 );
		solver2.SetRHSValue(0, i, (double)i / 10.0 );
	};

	bool bResult1 = solver1.Solve();
	rmssolver::LinearSystemSolverd linear(&solver2);
	bool bResult2 = linear.Solve();

	std::vector<double> dResult( solver1.GetNumRows(), 0 );
	for ( unsigned int i = 0; i < solver1.GetNumRows(); ++i ) {
		dResult[i] = solver1.GetSolutionValue(i);
		std::cerr << dResult[i] << " ";
	}

	std::cerr << std::endl;

	std::vector<double> dResult2( solver2.GetNumRows(), 0 );
	for ( unsigned int i = 0; i < solver2.GetNumRows(); ++i ) {
		dResult2[i] = solver2.GetSolutionValue(0, i);
		std::cerr << dResult2[i] << " ";
	}

	getchar();
}



#endif // DO_DENSE_MATRIX_TESTS
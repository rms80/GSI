#include "config.h"

int main(int argc, char * argv[])
{

#ifdef DO_SPARSE_MATRIX_TESTS
	void sparse_matrix_tests();
	sparse_matrix_tests();
#endif


#ifdef DO_SPARSE_MATRIX_SOLVER_TESTS
	void sparse_matrix_solver_tests();
	sparse_matrix_solver_tests();
#endif

#ifdef DO_DENSE_MATRIX_SOLVER_TESTS
	void dense_matrix_solver_tests();
	dense_matrix_solver_tests();
#endif

	return 0;
}


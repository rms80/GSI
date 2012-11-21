#ifndef __GSI_SOLVER_GENERAL__
#define __GSI_SOLVER_GENERAL__

#include "DenseLinearSystem.h"

namespace gsi {

/*
 * Wrapper for LAPACK dgesv
 */

class Solver_General
{
public:
	Solver_General(DenseLinearSystem * pSystem);
	~Solver_General(void);

	/*
	 * solve system. returns false if there were any problems (which  is not super-helpful...)
	 */
	virtual bool Solve();

	/*
	 * Factorize and then solve system 
	 */
	virtual bool Factorize();
	virtual bool Solve_Factorized();

protected:
	DenseLinearSystem * m_pSystem;
};



}  // namespace gsi


#endif  // __GSI_SOLVER_GENERAL__
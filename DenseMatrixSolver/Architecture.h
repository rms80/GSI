#pragma once

#include "RMSSolver.h"

namespace rmssolver {

class RMSSOLVER_ITEM Architecture
{
public:

	// architecture types wrt available instruction sets
	enum Type {
		Unknown,
		Any_SSE2,
		Athlon_3DNow,
		Intel_MKL
	};

	static Architecture * Get();

	Type GetType();

	void ForceType( Type eType );
	void RestoreDetected( );

	static char * GetString(Type eType);

protected:

	Type m_eDetectedType;
	Type m_eType;
	Type DetermineArch();

	static Architecture * m_pSingleton;
	Architecture(void);
	~Architecture(void);
};


} // end namespace rmssolver
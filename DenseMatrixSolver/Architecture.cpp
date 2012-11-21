#include ".\architecture.h"

using namespace rmssolver;

Architecture * Architecture::m_pSingleton = NULL;

Architecture * Architecture::Get() 
{
	if ( m_pSingleton == NULL )
		m_pSingleton = new Architecture();
	return m_pSingleton;
}

Architecture::Architecture(void)
{
	m_eDetectedType = DetermineArch();
	m_eType = m_eDetectedType;
}

Architecture::~Architecture(void)
{
}



Architecture::Type Architecture::GetType()
{
	return m_eType;
}


void Architecture::ForceType( Type eType )
{
	m_eType = eType;
}

void Architecture::RestoreDetected( )
{
	m_eType = m_eDetectedType;
}

Architecture::Type Architecture::DetermineArch()
{
	// [RMS] how do we do this...??

	return Architecture::Unknown;
}



char * Architecture::GetString(Type eType)
{
	switch ( eType ) {
		default:
		case Unknown:
			return "Unknown";
		case Any_SSE2:
			return "Any_SSE2";
		case Athlon_3DNow:
			return "Athlon_3DNow";
		case Intel_MKL:
			return "Intel_MKL";
	}
}

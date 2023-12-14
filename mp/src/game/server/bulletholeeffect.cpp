#include "cbase.h"

class CMyLogicalEntity : public CLogicalEntity
{
public:
	DECLARE_CLASS(CMyLogicalEntity, CLogicalEntity);
	
private:

	int	m_nThreshold;	// Count at which to fire our output
	int	m_nCounter;	// Internal counter

	COutputEvent	m_OnThreshold;	// Output event when the counter reaches the threshold
};

DECLARE_DATADESC();
LINK_ENTITY_TO_CLASS(my_logical_entity, CMyLogicalEntity);
// Construct
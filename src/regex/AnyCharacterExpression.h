#pragma once
#include "RegularExpression.h"
class AnyCharacterExpression :
	public RegularExpression
{
protected:
	virtual int match_first(const char * str, unsigned int len) const;
	AnyCharacterExpression(const AnyCharacterExpression &re);

public:
	AnyCharacterExpression() {};
	virtual RegularExpression * clone() const;
};


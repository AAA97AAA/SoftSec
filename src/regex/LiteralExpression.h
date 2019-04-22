#pragma once
#include "RegularExpression.h"
class LiteralExpression :
	public RegularExpression
{
protected:
	const char litr;
	virtual int match_first(const char * str, unsigned int len) const;

public:
	LiteralExpression(const char litr);
};
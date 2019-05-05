#pragma once
#include "RegularExpression.h"
class AlternationExpression :
	public RegularExpression
{
protected:
	const RegularExpression * const first;
	const RegularExpression * const rest;
	
	virtual int match_first(const char * str, unsigned int len) const;
	AlternationExpression(const AlternationExpression &re);

public:
	AlternationExpression(const RegularExpression * const first, const RegularExpression * const rest);
	~AlternationExpression();
	virtual RegularExpression * clone() const;
};


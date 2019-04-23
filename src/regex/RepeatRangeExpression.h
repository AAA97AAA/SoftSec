#pragma once
#include "RegularExpression.h"
class RepeatRangeExpression :
	public RegularExpression
{
protected:
	const RegularExpression * const regex;
	const unsigned int min, max;
	virtual int match_first(const char * str, unsigned int len) const;
	RepeatRangeExpression(const RepeatRangeExpression &re);

public:
	RepeatRangeExpression(const RegularExpression * const regex, const unsigned int min, const unsigned int max);
	~RepeatRangeExpression();
	virtual int match(const char * str, unsigned int len) const;
	virtual RegularExpression * clone() const;
};


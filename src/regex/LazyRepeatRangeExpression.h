#pragma once
#include "RepeatRangeExpression.h"
class LazyRepeatRangeExpression :
	public RepeatRangeExpression
{
protected:
	LazyRepeatRangeExpression(const LazyRepeatRangeExpression &re);

public:
	LazyRepeatRangeExpression(const RegularExpression * const regex, const unsigned int min, const unsigned int max);
	virtual int match(const char * str, unsigned int len) const;
	virtual RegularExpression * clone() const;
};
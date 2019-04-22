#pragma once
#include "RegularExpression.h"
class RepeatNExpression :
	public RegularExpression
{
private:
	const RegularExpression * const regex;
	const unsigned int N;
	virtual int match_first(const char * str, unsigned int len) const;

public:
	RepeatNExpression(const RegularExpression * const regex, unsigned int N);
	~RepeatNExpression();
};


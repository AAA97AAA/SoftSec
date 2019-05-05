#pragma once
#include "RegularExpression.h"

class RepeatNExpression :
	public RegularExpression
{
private:
	RegularExpression * const regex_;
	const unsigned int N;
	virtual int match_first(const char * str, unsigned int len) const;

protected:
	RepeatNExpression(const RepeatNExpression &re);

public:
	RepeatNExpression(const RegularExpression * const regex, unsigned int N);
	~RepeatNExpression();
	virtual RegularExpression * clone() const;
};


#pragma once
#include "RegularExpression.h"
#include <tuple>
class SearchExpression :
	public RegularExpression
{
private:
	const RegularExpression * const regex;
	virtual int match_first(const char * str, unsigned int len) const;
public:
	SearchExpression(RegularExpression *regex);
	virtual std::tuple<int, int> search(const char * str, unsigned int len) const;
};


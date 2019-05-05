#pragma once
#include "RegularExpression.h"
#include <tuple>
class SearchExpression :
	public RegularExpression
{
private:
	const RegularExpression * const regex;
	virtual int match_first(const char * str, unsigned int len) const;
	SearchExpression(const SearchExpression &re);

public:
	SearchExpression(RegularExpression *regex);
	~SearchExpression();
	virtual std::tuple<int, int> search(const char * str, unsigned int len) const;
	virtual RegularExpression * clone() const;
};


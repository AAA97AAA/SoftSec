#include "SearchExpression.h"
#include <tuple>

SearchExpression::SearchExpression(RegularExpression * regex) : regex(regex)
{
}

int SearchExpression::match_first(const char * str, unsigned int len) const
{
	return regex->match(str, len);
}

std::tuple<int, int> SearchExpression::search(const char * str, unsigned int len) const
{
	int i = 0;
	int mlen = -1;
	bool match = false;
	
	while (i < len && match == false) {
		mlen = match_first(&str[i], len - i);
		if (mlen < 0) {
			++i;
		} else {
			match = true;
		}
	}

	return std::tuple<int, int>(i, mlen);
}

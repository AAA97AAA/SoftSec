#include "AlternationExpression.h"

AlternationExpression::AlternationExpression(const RegularExpression * const first, const RegularExpression * const rest)
	: first(first), rest(rest)
{
}

AlternationExpression::~AlternationExpression()
{
	if (first != nullptr) {
		delete first;
	}
	if (rest != nullptr) {
		delete rest;
	}
}

int AlternationExpression::match_first(const char * str, unsigned int len) const
{
	if (first == nullptr) {
		return 0;
	} else {
		int mlen = first->match(str, len);
		if (mlen < 0) {
			if (rest == nullptr) {
				return 0;
			}
			return rest->match(str, len);
		} else {
			return mlen;
		}
	}
}

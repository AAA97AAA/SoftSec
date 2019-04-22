#include "RepeatNExpression.h"

RepeatNExpression::RepeatNExpression(const RegularExpression * const regex, unsigned int N)
	: regex(regex), N(N)
{
}

RepeatNExpression::~RepeatNExpression()
{
	if (regex != nullptr) {
		delete regex;
	}
}

int RepeatNExpression::match_first(const char * str, unsigned int len) const
{
	int mlen = 0;
	int mlen_regex = 0;
	unsigned int i;

	for (i = 0; i < N && mlen_regex >= 0; mlen += mlen_regex, ++i) {
		mlen_regex = regex->match(&str[mlen], len - mlen);
	}

	if (mlen_regex >= 0) {
		return mlen;
	} else {
		return -1;
	}
}
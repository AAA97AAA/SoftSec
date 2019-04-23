#include "RepeatRangeExpression.h"

RepeatRangeExpression::RepeatRangeExpression(const RegularExpression * const regex, const unsigned int min, const unsigned int max)
	: regex(regex), min(min), max(max)
{
}

RepeatRangeExpression::RepeatRangeExpression(const RepeatRangeExpression &re) :
	RegularExpression(re),
	regex(re.regex->clone()),
	min(re.min), max(re.max)
{
}

RepeatRangeExpression::~RepeatRangeExpression()
{
	if (regex != nullptr) {
		delete regex;
	}
}

int RepeatRangeExpression::match_first(const char * str, unsigned int len) const
{
	return regex->match(str, len);
}

int RepeatRangeExpression::match(const char * str, unsigned int len) const
{
	unsigned int ubound = max;
	int mlen;
	int mlen_regex;
	int mlen_next = -1;
	bool backtrack = true;

	do {
		unsigned int i = 0;
		bool streak = true;
		mlen = 0;
		while (i < ubound && streak) {
			mlen_regex = match_first(&str[mlen], len - mlen);
			if (mlen_regex >= 0) {
				++i;
				mlen += mlen_regex;
			}
			else {
				streak = false;
			}
		}
		if (i < min) {
			return -1; // could not find a sequence long enough
		}
		if (streak == false) { // we cannot possibly reach ubound
			ubound = i; // reduce search space
		}
		mlen_next = match_next(mlen, str, len);
		if (mlen_next < 0) {
			if (ubound > min) {
				--ubound;
			} else {
				return -1; // could not find a sequence short enough
			}
		} else {
			backtrack = false;
		}
	} while (backtrack);

	return mlen + mlen_next;
}

RegularExpression * RepeatRangeExpression::clone() const
{
	return new RepeatRangeExpression(*this);
}
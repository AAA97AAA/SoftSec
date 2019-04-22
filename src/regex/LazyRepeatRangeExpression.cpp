#include "LazyRepeatRangeExpression.h"

LazyRepeatRangeExpression::LazyRepeatRangeExpression(const RegularExpression * const regex, const unsigned int min, const unsigned int max)
	: RepeatRangeExpression::RepeatRangeExpression(regex, min, max)
{
}

int LazyRepeatRangeExpression::match(const char * str, unsigned int len) const
{
	unsigned int lbound = min;
	int mlen;
	int mlen_regex;
	int mlen_next = -1;
	bool backtrack = true;

	do {
		unsigned int i = 0;
		bool streak = true;
		mlen = 0;
		while (i < lbound && streak) {
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
		if (streak == false) {
			// On the nth iteration, streak becomes false, meaning that
			// on the (n-1)th iteration, we had already reached the maximum length
			// of the repeat expression. But since the loop proceeded to the nth iteration,
			// then match_next failed in the (n-1)th iteration.
			// With streak == false, no further progress will be made, so we can terminate
			return -1;
		}
		mlen_next = match_next(mlen, str, len);
		if (mlen_next < 0) {
			if (lbound < max) {
				++lbound;
			}
			else {
				return -1; // could not find a sequence short enough
			}
		}
		else {
			backtrack = false;
		}
	} while (backtrack);

	return mlen + mlen_next;
}

#include "RegularExpression.h"
#include <assert.h>

RegularExpression::~RegularExpression()
{
	if (this->next != nullptr) {
		delete this->next;
	}
}

RegularExpression * const RegularExpression::add(RegularExpression * const regex)
{
	// Ideally, `regex` should not cause a loop in the linked list,
	//   unless this is done explicitly.

	if (next == nullptr) {
		next = regex;
	} else {
		next->add(regex);
	}
	return regex;
}

int RegularExpression::match_next(unsigned int mlen, const char * str, unsigned int len) const
{
	assert(len >= mlen);
	if (next != nullptr) {
		return next->match(&str[mlen], len - mlen);
	} else {
		return 0; // end of pattern
	}
}

int RegularExpression::match(const char * str, unsigned int len) const
{
	int mlen = match_first(str, len);
	
	if (mlen < 0) {
		return -1;
	}

	int mlen_next = match_next(mlen, str, len);
	if (mlen_next < 0) {
		return -1;
	}

	return mlen + mlen_next;
}
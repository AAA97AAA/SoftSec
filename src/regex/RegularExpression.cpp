#include "RegularExpression.h"
#include <assert.h>

RegularExpression::RegularExpression() :
	next_(nullptr)
{
}

RegularExpression::RegularExpression(const RegularExpression &re) :
	RegularExpression()
{
	if (re.next_ != nullptr) {
		add(re.next_->clone());
	}
}

RegularExpression::~RegularExpression()
{
	if (this->next_ != nullptr) {
		delete this->next_;
	}
}

RegularExpression * RegularExpression::add(RegularExpression * const regex)
{
	// Ideally, `regex` should not cause a loop in the linked list,
	//   unless this is done explicitly by the programmer

	if (next_ == nullptr) {
		next_ = regex;
	} else {
		next_->add(regex);
	}
	return regex;
}

int RegularExpression::match_next(unsigned int mlen, const char * str, unsigned int len) const
{
	assert(len >= mlen);
	const RegularExpression *n = next_;
	if (n != nullptr) {
		return n->match(&str[mlen], len - mlen);
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
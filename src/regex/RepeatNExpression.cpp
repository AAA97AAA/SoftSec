#include "RepeatNExpression.h"

RepeatNExpression::RepeatNExpression(const RegularExpression * const regex, unsigned int N)
	: regex_(regex->clone()), N(N)
{
	delete regex; // after cloning, we no longer want to own it
	RegularExpression *tail = nullptr, *head = nullptr, *tmp;
	for (unsigned int i = 1; i < N; ++i) {
		tmp = regex_->clone();
		if (tail != nullptr) {
			tail->add(tmp);
		} else {
			head = tmp;
		}
		tail = tmp;
	}
	regex_->add(head);
}

RepeatNExpression::RepeatNExpression(const RepeatNExpression &re) :
	RegularExpression(re),
	regex_(re.regex_->clone()),
	N(re.N)
{
}

RepeatNExpression::~RepeatNExpression()
{
	if (regex_ != nullptr) {
		delete regex_;
	}
}

int RepeatNExpression::match_first(const char * str, unsigned int len) const
{
	int mlen = 0;

	mlen = regex_->match(str, len);

	if (mlen >= 0) {
		return mlen;
	} else {
		return -1;
	}
}

RegularExpression * RepeatNExpression::clone() const
{
	return new RepeatNExpression(*this);
}
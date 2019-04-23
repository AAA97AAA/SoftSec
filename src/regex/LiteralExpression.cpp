#include "LiteralExpression.h"

LiteralExpression::LiteralExpression(const char litr) : litr(litr)
{
}

LiteralExpression::LiteralExpression(const LiteralExpression &re) :
	RegularExpression(re),
	litr(re.litr)
{
}

int LiteralExpression::match_first(const char * str, unsigned int len) const
{
	if (len == 0 || str[0] != this->litr) {
		return -1;
	}

	return 1;
}

RegularExpression * LiteralExpression::clone() const
{
	return new LiteralExpression(*this);
}
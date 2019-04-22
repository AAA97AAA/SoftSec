#include "LiteralExpression.h"

LiteralExpression::LiteralExpression(const char litr) : litr(litr)
{
}

int LiteralExpression::match_first(const char * str, unsigned int len) const
{
	if (len == 0 || str[0] != this->litr) {
		return -1;
	}

	return 1;
}
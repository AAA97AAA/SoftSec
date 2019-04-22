#include "AnyCharacterExpression.h"

int AnyCharacterExpression::match_first(const char * str, unsigned int len) const
{
	if (len == 0 || str[0] == '\n') {
		return -1;
	}

	return 1;
}

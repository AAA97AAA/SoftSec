#include "AnyCharacterExpression.h"

AnyCharacterExpression::AnyCharacterExpression(const AnyCharacterExpression &re) :
	RegularExpression(re)
{
}

int AnyCharacterExpression::match_first(const char * str, unsigned int len) const
{
	if (len == 0 || str[0] == '\n') {
		return -1;
	}

	return 1;
}

RegularExpression * AnyCharacterExpression::clone() const
{
	return new AnyCharacterExpression(*this);
}
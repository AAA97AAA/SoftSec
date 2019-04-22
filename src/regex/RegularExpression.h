#pragma once
class RegularExpression
{
protected:
	RegularExpression * next;
	virtual int match_first(const char * str, unsigned int len) const = 0;
	virtual int match_next(unsigned int mlen, const char * str, unsigned int len) const;

public:
	virtual ~RegularExpression();
	virtual RegularExpression * const add(RegularExpression * const regex);
	virtual int match(const char * str, unsigned int len) const;
};
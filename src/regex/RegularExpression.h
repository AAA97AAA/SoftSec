#pragma once

class RegularExpression
{
private:
	RegularExpression * next_;

protected:
	RegularExpression(const RegularExpression &re);
	virtual int match_first(const char * str, unsigned int len) const = 0;
	virtual int match_next(unsigned int mlen, const char * str, unsigned int len) const;

public:
	RegularExpression();
	virtual ~RegularExpression();
	virtual RegularExpression * add(RegularExpression * const regex);
	virtual int match(const char * str, unsigned int len) const;
	virtual RegularExpression * clone() const = 0;
};
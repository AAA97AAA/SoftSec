#include "AlternationExpression.h"
#include "AnyCharacterExpression.h"
#include "LazyRepeatRangeExpression.h"
#include "LiteralExpression.h"
#include "RegularExpression.h"
#include "RepeatNExpression.h"
#include "RepeatRangeExpression.h"
#include "SearchExpression.h"

RegularExpression * parse(const char ** str, size_t *len, int depth, int * err);
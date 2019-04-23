#include "LiteralExpression.h"
#include "RepeatNExpression.h"
#include "RepeatRangeExpression.h"
#include "LazyRepeatRangeExpression.h"
#include "AnyCharacterExpression.h"
#include "AlternationExpression.h"
#include "RegularExpression.h"
#include <stddef.h>
#include <limits.h>
#include <assert.h>

enum State {
    STATE_PARSE_MIN = 0,
    STATE_PARSE_MAX,
    STATE_COMPLETE,
    STATE_TRY_PARSE_LAZY,
    STATE_EXIT
};

static RegularExpression * try_parse_range(const char ** str, size_t *len, RegularExpression * regex)
{
    // only change str if parsing succeeds, otherwise parse as literals
    const char *stmp = *str;
	size_t ltmp = *len;
    assert(*stmp == '{');
    stmp++; ltmp--; // consume opening bracket
    
    RegularExpression *next = nullptr;
    
    unsigned int min = 0, max = UINT_MAX;
    bool range = false, lazy = false;
    bool max_available = false;
    
    // parse count or min
    State next_state = STATE_PARSE_MIN;
    while (ltmp > 0) {
        char c = *stmp;
        switch (next_state) {
        case STATE_PARSE_MIN:
            if (c == ',') {
                range = true;
                next_state = STATE_PARSE_MAX;
            } else if (c >= '0' && c <= '9') {
                min = min * 10 + (c - '0');
                // TODO fix integer overflow
            } else if (c == '}') {
                next_state = STATE_TRY_PARSE_LAZY; // parsing complete
            } else {
                goto error; // invalid character
            }
			stmp++; ltmp--;
            break;
        case STATE_PARSE_MAX:
            if (c >= '0' && c <= '9') {
                if (!max_available) {
                    max = 0;
                    max_available = true;
                }
                max = max * 10 + (c - '0');
                // TODO fix integer overflow
            } else if (c == '}') {
                next_state = STATE_TRY_PARSE_LAZY; // parsing complete
            } else {
                goto error; // invalid character
            }
			stmp++; ltmp--;
            break;
        case STATE_TRY_PARSE_LAZY:
            if (c == '?') {
                lazy = true;
            }
			next_state = STATE_EXIT;
            break;
        default:
            break;
        };

        if (next_state == STATE_EXIT) {
            break;
        }
    }
    
    if (next_state < STATE_COMPLETE) {
        goto error; // unbalanced brackets
    }
    
    if (min > max) {
        goto error; // invalid range
    }

	if (regex == nullptr) {
		goto error; // no preceding expression
	}
    
finish:
    *str = stmp;
	*len = ltmp;
    
    if (!range) {
        next = new RepeatNExpression(regex, min);
    } else if (!lazy) {
        next = new RepeatRangeExpression(regex, min, max);
    } else {
        next = new LazyRepeatRangeExpression(regex, min, max);
    }
    
error:
    return next;
}

RegularExpression * parse(const char ** str, size_t *len, int depth, int * err)
{
    RegularExpression *head = nullptr, *next = nullptr;
    bool closed = false;
	const char *stmp = *str;
	size_t ltmp = *len;
	while (ltmp > 0) {
		switch (*stmp) {
		case '(':
			stmp++; ltmp--; // consume the opening parentheses
			next = parse(&stmp, &ltmp, depth + 1, err);
			if (*err != 0) {
				goto error;
			}
			if (*stmp != ')') {
				*err = -1; // unbalanced parentheses
				goto error;
			}
			stmp++; ltmp--; // consume the closing parentheses
			break;
		case ')':
			if (depth == 0) {
				*err = -1; // unbalanced parentheses
				goto error;
			}
			closed = true;
			goto finish; // TODO make sure this is correct
			break;
		case '.':
			next = new AnyCharacterExpression();
			stmp++; ltmp--; // consume dot
			break;
		case '|':
			stmp++; ltmp--;
			head = new AlternationExpression(head, parse(&stmp, &ltmp, depth, err));
			if (*err != 0) {
				goto error;
			}
			continue;
			break;
		case '\\':
			stmp++; ltmp--; // consume the escape backslash
			if (ltmp == 0) {
				*err = -3; // unexpected end of string
				goto error;
			}
			/* intentional fallthrough */
		default:
			next = new LiteralExpression(*stmp);
			stmp++; ltmp--; // consume literal
			break;
		}

		// Check for repetitions
		switch (*stmp) {
		case '{':
			if (next == nullptr || (next = try_parse_range(&stmp, &ltmp, next)) == nullptr) {
				next = new LiteralExpression('{');
				stmp++; ltmp--; // consume literal
			}
			else {
				// do nothing
				// next is already set in the conditional
				// try_parse_range should have already consumed the range
				// TODO clean this shit code
			}
			break;
		case '*':
			if (next == nullptr) {
				*err = -2; // repetition modifier not preceded by expression
				goto error;
			}
			stmp++; ltmp--; // consume the '*' modifier
			if (ltmp == 0 || *stmp != '?') {
				next = new RepeatRangeExpression(next, 0, UINT_MAX);
			}
			else {
				stmp++; ltmp--; // consume the '?' modifier
				next = new LazyRepeatRangeExpression(next, 0, UINT_MAX);
			}
			break;
		case '+':
			if (next == nullptr) {
				*err = -2; // repetition modifier not preceded by expression
				goto error;
			}
			stmp++; ltmp--; // consume the '+' modifier
			if (ltmp == 0 || *stmp != '?') {
				next = new RepeatRangeExpression(next, 1, UINT_MAX);
			}
			else {
				stmp++; ltmp--; // consume the '?' modifier
				next = new LazyRepeatRangeExpression(next, 1, UINT_MAX);
			}
			break;
		case '?':
			if (next == nullptr) {
				*err = -2; // repetition modifier not preceded by expression
				goto error;
			}
			stmp++; ltmp--; // consume the '?' modifier
			if (ltmp == 0 || *stmp != '?') {
				next = new RepeatRangeExpression(next, 0, 1);
			}
			else {
				stmp++; ltmp--; // consume the '?' modifier
				next = new LazyRepeatRangeExpression(next, 0, 1);
			}
			break;
		default:
			break;
		}

        if (head == nullptr) {
            head = next;
        } else {
            head->add(next);
        }
    }
    goto finish;
    
error:
    if (head != nullptr) {
        delete head;
        head = nullptr;
    }
    
finish:
    if (depth > 0 && !closed) {
        if (*err == 0) {
            *err = -1; // unbalanced parentheses
        }
    } else if (*err == 0) {
		*str = stmp;
		*len = ltmp;
	}
    return head;
}
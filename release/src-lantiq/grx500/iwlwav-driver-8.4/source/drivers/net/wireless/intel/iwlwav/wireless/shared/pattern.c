/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "pattern.h"

static BOOL
pmatch(char *pattern, char *string)
{
  char *p = pattern;
  char *q = string;
  char c;
  BOOL skip_next_meta_character = FALSE;
  BOOL is_regular_character;

  do {
    c = *p++; 
    is_regular_character = FALSE;
    
    if (!skip_next_meta_character) { 
      switch (c) {
        case '\\' :
          skip_next_meta_character = TRUE;
          break;
        case '\0':
          break;
        case '?':
          if (*q++ == '\0')
            return FALSE;
          break;
        case '*':
          c = *p;
          if (c != '?' && c != '*' && c != '[') {
            while (*q != c) {
	      if (*q == '\0')
	        return FALSE;
	      q++;
	    }
          }
          do {
            if (pmatch(p, q))
	      return TRUE;
          } while (*q++ != '\0');
          return FALSE;
        case '[': {
          char *endp = p;
          BOOL found = FALSE;
          char chr = *q++;
          for (;;) {
            if (*endp == '\0')
	      return FALSE;		/* no matching ] */
	    if (*++endp == ']')
	      break;
          }
          if (chr == '\0')
            return FALSE;
          c = *p++;
          do {
            if (*p == '-' && p[1] != ']') {
	      p++;
	      if (chr >= c && chr <= *p)
                found = TRUE;
	      p++;
	    } else if (chr == c)
	      found = 1;
          } while ((c = *p++) != ']');
          if (!found)
            return FALSE;
          break;
        }
        default:
          is_regular_character = TRUE;
          break;
      }
    } else {
      is_regular_character = TRUE;
      skip_next_meta_character = FALSE;
    }

    if (is_regular_character && *q++ != c)
      return FALSE;
    
  } while(c != '\0');

  return (*q == '\0');
}

/*
 * Returns true if the pattern matches the string.
 */

BOOL __MTLK_IFUNC
mtlk_pattern_match(char *pattern, char *string)
{
  if (pattern[0] == '!')
    return !pmatch(pattern + 1, string);
  else
    return pmatch(pattern, string);
}

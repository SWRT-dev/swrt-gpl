/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mtlkstrtok.h"
#include "mtlk_osal.h"

#define LOG_LOCAL_GID   GID_STRTOK
#define LOG_LOCAL_FID   0

#define MTLK_STRTOK_ARRAY_STEP 10

static int
_mtlk_strtok_extend_tokens (mtlk_strtok_t *strtok)
{
  int    res = MTLK_ERR_NO_MEM;
  char **tokens = 
    (char **)mtlk_osal_mem_alloc((strtok->max_nof_tokens + MTLK_STRTOK_ARRAY_STEP) * sizeof(char *),
                                 MTLK_MEM_TAG_STRTOK);

  if (tokens) {
    int i = 0;

    for (; i < strtok->max_nof_tokens; i++) {
      tokens[i] = strtok->tokens[i]; /* copy old token pointers */
    }

    strtok->max_nof_tokens += MTLK_STRTOK_ARRAY_STEP; /* extend max_nod_tokens */
    for (; i < strtok->max_nof_tokens; i++) {
      tokens[i] = NULL; /* copy extended token pointers to NULL */
    }

    if (strtok->tokens) {
      mtlk_osal_mem_free(strtok->tokens);
    }
    strtok->tokens = tokens;
    res            = MTLK_ERR_OK;
  }

  return res;
}

static int
_mtlk_strtok_add_token (mtlk_strtok_t *strtok, const char *start, uint32 len)
{
  int   res   = MTLK_ERR_UNKNOWN;
  char *token;

  if (strtok->max_nof_tokens == strtok->nof_tokens) {
    res = _mtlk_strtok_extend_tokens(strtok);
    if (res != MTLK_ERR_OK) {
      goto end;
    }
  }

  token = (char *)mtlk_osal_mem_alloc(len + 1, MTLK_MEM_TAG_STRTOK);
  if (!token) {
    res = MTLK_ERR_NO_MEM;
    goto end;
  }

  mtlk_osal_strlcpy(token, start, len + 1);

  strtok->tokens[strtok->nof_tokens] = token;
  ++strtok->nof_tokens;

  res = MTLK_ERR_OK;

end:
  return res;
}

MTLK_INIT_STEPS_LIST_BEGIN(str_tok)
MTLK_INIT_INNER_STEPS_BEGIN(str_tok)
MTLK_INIT_STEPS_LIST_END(str_tok);

int __MTLK_IFUNC
mtlk_strtok_init (mtlk_strtok_t *strtok)
{
  MTLK_ASSERT(strtok != NULL);

  memset(strtok, 0, sizeof(*strtok));

  MTLK_INIT_TRY(str_tok, MTLK_OBJ_PTR(strtok))
  MTLK_INIT_FINALLY(str_tok,  MTLK_OBJ_PTR(strtok))
  MTLK_INIT_RETURN(str_tok,  MTLK_OBJ_PTR(strtok), mtlk_strtok_cleanup, (strtok))
}

int __MTLK_IFUNC
mtlk_strtok_parse (mtlk_strtok_t *strtok, const char *str, char delimiter)
{
  int         res = MTLK_ERR_OK;
  const char *t   = str;

  MTLK_ASSERT(strtok != NULL);
  MTLK_ASSERT(str != NULL);
  MTLK_ASSERT(delimiter != 0);

  while (1) {
    if (*t != delimiter && *t != 0) {
      ++t;
      continue;
    }

    /* token detected */
    if (t != str) { /* token's len != 0 */
      res = _mtlk_strtok_add_token(strtok, str, (uint32)(t - str));
      if (res != MTLK_ERR_OK) {
        break;
      }
    }

    if (*t == 0) { /* end of line detected */
      break;
    }
    
    t = str = t + 1; /* advance pointers */
  }

  return res;
}

int __MTLK_IFUNC
mtlk_strtok_add_token (mtlk_strtok_t *strtok, const char *token)
{
  int len;

  MTLK_ASSERT(strtok != NULL);
  MTLK_ASSERT(token != NULL);

  len = strlen(token);

  return len?_mtlk_strtok_add_token(strtok, token, len):MTLK_ERR_PARAMS;
}

uint32 __MTLK_IFUNC
mtlk_strtok_get_nof_tokens (const mtlk_strtok_t *strtok)
{
  MTLK_ASSERT(strtok != NULL);

  return strtok->nof_tokens;
}

const char* __MTLK_IFUNC
mtlk_strtok_get_token (const mtlk_strtok_t *strtok, uint32 idx)
{
  MTLK_ASSERT(strtok != NULL);

  return (idx < strtok->nof_tokens)?strtok->tokens[idx]:NULL;
}

void __MTLK_IFUNC
mtlk_strtok_purge (mtlk_strtok_t *strtok)
{
  uint32 i = 0;

  for (; i < strtok->nof_tokens; i++) {
    mtlk_osal_mem_free(strtok->tokens[i]);
  }
  if (strtok->tokens) {
    mtlk_osal_mem_free(strtok->tokens);
    strtok->tokens = NULL;
  }
  strtok->nof_tokens     = 0;
  strtok->max_nof_tokens = 0;
}

void __MTLK_IFUNC
mtlk_strtok_cleanup (mtlk_strtok_t *strtok)
{
  MTLK_CLEANUP_BEGIN(str_tok,  MTLK_OBJ_PTR(strtok))
  MTLK_CLEANUP_END(str_tok,  MTLK_OBJ_PTR(strtok))
}


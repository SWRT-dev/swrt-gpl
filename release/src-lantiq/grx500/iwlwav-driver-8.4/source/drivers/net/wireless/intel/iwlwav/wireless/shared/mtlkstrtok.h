/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_STR_TOK_H__
#define __MTLK_STR_TOK_H__

/*! \file  mtlkstrtok.h

    \brief String tokenizer module

    Module that allows tocparse a string by delimiter and provides a dynamic
    string array functionality.
*/

#include "mtlkstartup.h"

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

/*! \typedef struct mtlk_strtok_internal mtlk_strtok_t
    \brief   "Black box" MTLK String Tokenizer object.

    \warning must not be used directly. 
             Use mtlk_strtok_...() API instead.
*/
typedef struct mtlk_strtok_internal
{
  char **tokens;
  uint32 nof_tokens;
  uint32 max_nof_tokens;
  MTLK_DECLARE_INIT_STATUS;
} __MTLK_IDATA mtlk_strtok_t;

/*! \fn      int mtlk_strtok_init(mtlk_strtok_t *strtok)
    \brief   Initiates MTLK String Tokenizer object.

    This function initializes internals of MTLK String Tokenizer object.

    \param   strtok MTLK String Tokenizer object

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_strtok_init(mtlk_strtok_t *strtok);

/*! \fn      int mtlk_strtok_parse(mtlk_strtok_t *strtok, 
                                   const char    *str, 
                                   char           delimiter)
    \brief   Parses string.

    This function parses 0-terminated string and fills up the MTLK String 
    Tokenizer object with the found tokens.

    \param   strtok    MTLK String Tokenizer object
    \param   str       0-treminated string to parse
    \param   delimiter delimiter to be used

    \return  MTLK_ERR... code
*/
int         __MTLK_IFUNC mtlk_strtok_parse(mtlk_strtok_t *strtok, 
                                           const char    *str, 
                                           char           delimiter);

/*! \fn      int mtlk_strtok_add_token(mtlk_strtok_t *strtok, 
                                       const char    *token)
    \brief   Adds token.

    This function adds token string to token's array.

    \param   strtok    MTLK String Tokenizer object
    \param   token     0-treminated token to add

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_strtok_add_token(mtlk_strtok_t *strtok, 
                                       const char    *token);

/*! \fn      int mtlk_strtok_get_nof_tokens(mtlk_strtok_t *strtok)

    \brief   Returns number of stored tokens.

    This function returns a number of tokens stored within the token's array.

    \param   strtok    MTLK String Tokenizer object

    \return  number of stored tokens
*/
uint32 __MTLK_IFUNC mtlk_strtok_get_nof_tokens(const mtlk_strtok_t *strtok);

/*! \fn      int mtlk_strtok_get_token(mtlk_strtok_t *strtok, uint32 idx)

    \brief   Returns token by index.

    This function returns a token by index specified.

    \param   strtok    MTLK String Tokenizer object
    \param   idx       desired token's index

    \return  desired token, NULL if index is out-of-range.
*/
const char* __MTLK_IFUNC mtlk_strtok_get_token(const mtlk_strtok_t *strtok, uint32 idx);

/*! \fn      void mtlk_strtok_purge(mtlk_strtok_t *strtok)
    \brief   Removes all stored tokens.

    This function removes all stored tokens from tokens array.

    \param   strtok MTLK String Tokenizer object
*/
void        __MTLK_IFUNC mtlk_strtok_purge(mtlk_strtok_t *strtok);

/*! \fn      void mtlk_strtok_cleanup(mtlk_strtok_t *strtok)
    \brief   Cleans up MTLK String Tokenizer object.

    This function cleans up the internals of MTLK String Tokenizer object.

    \param   strtok MTLK String Tokenizer object
*/
void __MTLK_IFUNC mtlk_strtok_cleanup(mtlk_strtok_t *strtok);

#define MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_STR_TOK_H__ */

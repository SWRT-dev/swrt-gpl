/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * $Id$
 *
 *
 *
 *  Originaly written by Artem Migaev
 *
 */

#ifndef _ETH_PARSER_H_
#define _ETH_PARSER_H_

#ifdef CPTCFG_IWLWAV_ETH_PARSER
void mtlk_eth_parser(void *data, int length, const char *if_name, const char *f);
#else
#define mtlk_eth_parser(a, b, c, d) {}
#endif

#endif /* _ETH_PARSER_H_ */

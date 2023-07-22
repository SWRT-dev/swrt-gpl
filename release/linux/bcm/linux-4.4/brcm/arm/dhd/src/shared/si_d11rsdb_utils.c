/*
 * Misc utility routines for accessing SOC Interconnects
 * of Broadcom rsdb chips.
 *
 * Copyright (C) 2016, Broadcom. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: siutils.c 459515 2014-03-04 03:34:52Z $
 */
#include <bcm_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <bcmdevs.h>
#include <hndsoc.h>
#include "siutils_priv.h"

void
si_d11rsdb_core_disable(si_t *sih, uint32 bits)
{
	si_info_t *sii = SI_INFO(sih);
	void *pmacai = NULL;
	void *smacai = NULL;
	uint origidx = 0;

	/* save current core index */
	origidx = si_coreidx(&sii->pub);

	si_setcore(sih, D11_CORE_ID, 1);
	smacai = sii->curwrap;
	si_setcore(sih, D11_CORE_ID, 0);
	pmacai = sii->curwrap;

	ai_d11rsdb_core_disable(sii, bits, pmacai, smacai);

	si_setcoreidx(sih, origidx);
}

void
si_d11rsdb_core_reset(si_t *sih, uint32 bits, uint32 resetbits)
{
	si_info_t *sii = SI_INFO(sih);
	void *pmacai = NULL;
	void *smacai = NULL;
	uint origidx = 0;

	/* save current core index */
	origidx = si_coreidx(&sii->pub);

	si_setcore(sih, D11_CORE_ID, 1);
	smacai = sii->curwrap;
	si_setcore(sih, D11_CORE_ID, 0);
	pmacai = sii->curwrap;

	ai_d11rsdb_core_reset(sih, bits, resetbits, pmacai, smacai);

	si_setcoreidx(sih, origidx);
}

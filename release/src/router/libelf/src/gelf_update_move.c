/* Update move structure at the given index.
   Copyright (C) 2000, 2001, 2002, 2005, 2009, 2014, 2015 Red Hat, Inc.
   This file is part of elfutils.
   Written by Ulrich Drepper <drepper@redhat.com>, 2000.

   This file is free software; you can redistribute it and/or modify
   it under the terms of either

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at
       your option) any later version

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at
       your option) any later version

   or both in parallel, as here.

   elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see <http://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <gelf.h>
#include <stdlib.h>

#include "libelfP.h"


int
gelf_update_move (Elf_Data *data, int ndx, GElf_Move *src)
{
  int result = 0;
  Elf_Data_Scn *data_scn = (Elf_Data_Scn *) data;
  Elf *elf;

  if (data == NULL)
    return result;

  if (unlikely (data->d_type != ELF_T_MOVE))
    {
      /* The type of the data better should match.  */
      __libelf_seterrno (ELF_E_DATA_MISMATCH);
      return result;
    }

  elf = data_scn->s->elf;
  rwlock_wrlock (elf->lock);

  if (elf->class == ELFCLASS32)
    {
      Elf32_Move *move;

      /* Check whether input values are too large for 32 bit conversion.  */
      if (unlikely (src->m_poffset > 0xffffffffull)
	  || unlikely (GELF_M_SYM (src->m_info) > 0xffffff)
	  || unlikely (GELF_M_SIZE (src->m_info) > 0xff))
	{
	  __libelf_seterrno (ELF_E_INVALID_DATA);
	  goto out;
	}

      /* Check whether we have to resize the data buffer.  */
      if (INVALID_NDX (ndx, Elf32_Move, data))
	{
	  __libelf_seterrno (ELF_E_INVALID_INDEX);
	  goto out;
	}

      move = &((Elf32_Move *) data->d_buf)[ndx];

      move->m_value = src->m_value;
      move->m_info = ELF32_M_INFO (GELF_M_SYM (src->m_info),
				   GELF_M_SIZE (src->m_info));
      move->m_poffset = src->m_poffset;
      move->m_repeat = src->m_repeat;
      move->m_stride = src->m_stride;
    }
  else
    {
      eu_static_assert (sizeof (GElf_Move) == sizeof (Elf64_Move));

      /* Check whether we have to resize the data buffer.  */
      if (INVALID_NDX (ndx, GElf_Move, data))
	{
	  __libelf_seterrno (ELF_E_INVALID_INDEX);
	  goto out;
	}

      ((Elf64_Move *) data->d_buf)[ndx] = *src;
    }

  result = 1;

  /* Mark the section as modified.  */
  data_scn->s->flags |= ELF_F_DIRTY;

 out:
  rwlock_unlock (elf->lock);

  return result;
}

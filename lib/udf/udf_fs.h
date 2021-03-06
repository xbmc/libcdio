/*
  Copyright (C) 2006, 2008, 2012 Rocky Bernstein <rockyb@gnu.org>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CDIO_UDF_UDF_FS_H_
#define CDIO_UDF_UDF_FS_H_

#include <cdio/ecma_167.h>
/**
 * Check the descriptor tag for both the correct id and correct checksum.
 * Return zero if all is good, -1 if not.
 */
CDIO_EXTERN int udf_checktag(const udf_tag_t *p_tag, udf_Uint16_t tag_id);

#endif /* CDIO_UDF_UDF_FS_H_ */


/* 
 * Local variables:
 *  c-file-style: "gnu"
 *  tab-width: 8
 *  indent-tabs-mode: nil
 * End:
 */

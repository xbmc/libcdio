/*
  $Id: testpregap.c,v 1.1 2008/03/18 05:59:19 edsdead Exp $

  Copyright (C) 2003, 2004, 2005 Rocky Bernstein <rocky@panix.com>
  Copyright (C) 2008 Robert W. Fuller <hydrologiccycle@gmail.com>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* 
   Regression test for cdio_get_pregap_lsn()
*/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <cdio/cdio.h>
#include <cdio/cd_types.h>
#include <cdio/logging.h>

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>

static void 
log_handler (cdio_log_level_t level, const char message[])
{
  switch(level) {
  case CDIO_LOG_DEBUG:
  case CDIO_LOG_INFO:
    return;
  default:
    printf("cdio %d message: %s\n", level, message);
  }
}

typedef struct _pregap_list_t {

  char *image;
  track_t track;
  lsn_t pregap;

} pregap_list_t;

static pregap_list_t pregapList[] =
{
    { "t2.toc", 1, 4425 },
    { "t2.toc", 2, CDIO_INVALID_LSN },
    { "p1.cue", 1, 0 },
    { "p1.cue", 2, 150 },
    { "p1.cue", 3, CDIO_INVALID_LSN },
/*    { "p1.nrg", 1, 0 }, Nero did not create the proper pre-gap - bleh */
    { "p1.nrg", 2, 225 },
    { "p1.nrg", 3, CDIO_INVALID_LSN }
};

#define NELEMS(v) (sizeof(v) / sizeof(v[0]))

/* gcc -Wall -I../include testpregap.c ../lib/driver/.libs/libcdio.a */

int
main(int argc, const char *argv[])
{
  CdIo_t *cdObj;
  char *image;
  lsn_t pregap;
  int i;
  int rc = 0;
  
  cdio_log_set_handler (log_handler);

  if (! (cdio_have_driver(DRIVER_NRG) && cdio_have_driver(DRIVER_BINCUE)
   && cdio_have_driver(DRIVER_CDRDAO)) )  {
    printf("You don't have enough drivers for this test\n");
    exit(77);
  }

  for (i = 0;  i < NELEMS(pregapList);  ++i) {

    image = pregapList[i].image;

    cdObj = cdio_open(image, DRIVER_UNKNOWN);
    if (!cdObj) {
      printf("unrecognized image: %s\n", image);
      return 50;
    }

    pregap = cdio_get_track_pregap_lsn(cdObj, pregapList[i].track);
    if (pregap != pregapList[i].pregap) {
      printf("%s should have had pregap of lsn=%d instead of lsn=%d\n",
       image, pregapList[i].pregap, pregap);
      rc = i + 1;
    } else {
      printf("%s had expected pregap\n", image);
    }

    cdio_destroy(cdObj);
  }

  return rc;
}
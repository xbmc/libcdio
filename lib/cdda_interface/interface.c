/*
  $Id: interface.c,v 1.7 2005/01/06 03:38:58 rocky Exp $

  Copyright (C) 2005 Rocky Bernstein <rocky@panix.com>
  Copyright (C) 1998 Monty xiphmont@mit.edu
  
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
/******************************************************************
 * Top-level interface module for cdrom drive access.  SCSI, ATAPI, etc
 *    specific stuff are in other modules.  Note that SCSI does use
 *    specialized ioctls; these appear in common_interface.c where the
 *    generic_scsi stuff is in scsi_interface.c.
 *
 ******************************************************************/

#include "common_interface.h"
#include "low_interface.h"
#include "utils.h"
#include <cdio/bytesex.h>

static void _clean_messages(cdrom_drive_t *d)
{
  if(d){
    if(d->messagebuf)free(d->messagebuf);
    if(d->errorbuf)free(d->errorbuf);
    d->messagebuf=NULL;
    d->errorbuf=NULL;
  }
}

/* doubles as "cdrom_drive_free()" */
int 
cdda_close(cdrom_drive_t *d)
{
  if(d){
    if(d->opened)
      d->enable_cdda(d,0);

    cdio_destroy (d->p_cdio);
    _clean_messages(d);
    if (d->cdda_device_name) free(d->cdda_device_name);
    if (d->drive_model)      free(d->drive_model);
    if (d->sg)               free(d->sg);
    
    free(d);
  }
  return(0);
}

/* finish initializing the drive! */
int 
cdda_open(cdrom_drive_t *d)
{
  int ret;
  if(d->opened)return(0);

#if HAVE_SCSI_HANDLING
  switch(d->interface){
  case GENERIC_SCSI:  
    if((ret=scsi_init_drive(d)))
      return(ret);
    break;
  case COOKED_IOCTL:  
    if((ret=cooked_init_drive(d)))
      return(ret);
    break;
#ifdef CDDA_TEST
  case TEST_INTERFACE:  
    if((ret=test_init_drive(d)))
      return(ret);
    break;
#endif
  default:
    cderror(d, "100: Interface not supported\n");
    return(-100);
  }
#else 
  d->interface = COOKED_IOCTL;
  if ( (ret=cooked_init_drive(d)) )
    return(ret);
#endif
  
  /* Check TOC, enable for CDDA */
  
  /* Some drives happily return a TOC even if there is no disc... */
  {
    int i;
    for(i=0; i<d->tracks; i++)
      if(d->disc_toc[i].dwStartSector<0 ||
	 d->disc_toc[i+1].dwStartSector==0){
	d->opened=0;
	cderror(d,"009: CDROM reporting illegal table of contents\n");
	return(-9);
      }
  }

  if((ret=d->enable_cdda(d,1)))
    return(ret);
    
  /*  d->select_speed(d,d->maxspeed); most drives are full speed by default */
  if(d->bigendianp==-1)d->bigendianp=data_bigendianp(d);
  return(0);
}

int 
cdda_speed_set(cdrom_drive_t *d, int speed)
{
  return d->set_speed ? d->set_speed(d, speed) : 0;
}

long cdda_read(cdrom_drive_t *d, void *buffer, lsn_t beginsector, long sectors)
{
  if(d->opened){
    if(sectors>0){
      sectors=d->read_audio(d,buffer,beginsector,sectors);

      if(sectors!=-1){
	/* byteswap? */
	if(d->bigendianp==-1) /* not determined yet */
	  d->bigendianp=data_bigendianp(d);
	
	if(d->bigendianp!=bigendianp()){
	  int i;
	  uint16_t *p=(uint16_t *)buffer;
	  long els=sectors*CDIO_CD_FRAMESIZE_RAW/2;
	  
	  for(i=0;i<els;i++)
	    p[i]=UINT16_SWAP_LE_BE_C(p[i]);
	}
      }
    }
    return(sectors);
  }
  
  cderror(d,"400: Device not open\n");
  return(-400);
}

void 
cdda_verbose_set(cdrom_drive_t *d,int err_action, int mes_action)
{
  d->messagedest=mes_action;
  d->errordest=err_action;
}

extern char *cdda_messages(cdrom_drive_t *d)
{
  char *ret=d->messagebuf;
  d->messagebuf=NULL;
  return(ret);
}

extern char *cdda_errors(cdrom_drive_t *d)
{
  char *ret=d->errorbuf;
  d->errorbuf=NULL;
  return(ret);
}

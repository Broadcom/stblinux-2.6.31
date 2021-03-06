/*
 * Copyright (c) 2000-2003 Silicon Graphics, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 * Mountain View, CA  94043, or:
 *
 * http://www.sgi.com
 *
 * For further information regarding this notice, see:
 *
 * http://oss.sgi.com/projects/GenInfo/SGIGPLNoticeExplan/
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <disk/volume.h>
#include "xvm.h"

int
mnt_is_xvm_subvol(
	dev_t		dev)
{
	return get_driver_block_major("xvm", major(dev));
}

/*
 * If the logical device is a xvm striped volume, then it returns the
 * stripe unit and stripe width information.
 * Input parameters:	the logical volume
 *			the subvolume type - (SVTYPE_RT or
 *					      SVTYPE_DATA)
 * Output parameters:	the stripe unit and width in 512 byte blocks
 *                      true/false - was this device an XVM volume?
 */
int
xvm_get_subvol_stripe(
	char		*dev,
	sv_type_t	type,
	int		*sunit,
	int		*swidth,
	struct stat64	*sb)
{
	int fd;
	xvm_getdev_t getdev;
	xvm_subvol_stripe_t subvol_stripe;

	if (!mnt_is_xvm_subvol(sb->st_rdev))
		return 0;

	/*
	 * This will actually open the data subvolume.
	 */
	if ((fd = open(dev, O_RDONLY)) < 0)
		return 0;

	/*
	 * Go and get the the information for the correct
	 * subvolume.
	 */
	if (ioctl(fd, DIOCGETVOLDEV, &getdev) < 0) {
		close(fd);
		return 0;
	}
	if ( (type == SVTYPE_RT) && (getdev.rt_subvol_dev) )
		subvol_stripe.dev = getdev.rt_subvol_dev;
	else if ( (type == SVTYPE_DATA) && (getdev.data_subvol_dev) )
		subvol_stripe.dev = getdev.data_subvol_dev;
	else {
		close(fd);
		return 0;
	}

	if (ioctl(fd, DIOCGETVOLSTRIPE, &subvol_stripe) < 0) {
		close(fd);
		return 0;
	}

	*sunit = subvol_stripe.unit_size;
	*swidth = *sunit * subvol_stripe.width_size;
	close(fd);
	return 1;
}

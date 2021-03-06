/* vi: set sw=4 ts=4: */
/*
 * posix_fadvise() for uClibc
 * http://www.opengroup.org/onlinepubs/009695399/functions/posix_fadvise.html
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/* need to hide the 64bit prototype or the strong_alias()
 * will fail when __NR_fadvise64_64 doesnt exist */
#define posix_fadvise64 __hideposix_fadvise64

#include "syscalls.h"
#include <fcntl.h>

#undef posix_fadvise64

#ifdef __NR_fadvise64
#define __NR_posix_fadvise __NR_fadvise64
_syscall4(int, posix_fadvise, int, fd, off_t, offset,
          off_t, len, int, advice);

#if defined __UCLIBC_HAS_LFS__ && (!defined __NR_fadvise64_64 || !defined _syscall6)
extern __typeof(posix_fadvise) posix_fadvise64;
strong_alias(posix_fadvise,posix_fadvise64)
#endif

#else
int posix_fadvise(int fd attribute_unused, off_t offset attribute_unused, off_t len attribute_unused, int advice attribute_unused)
{
	__set_errno(ENOSYS);
	return -1;
}
#endif

/* $Header: rd_arhdr.c,v 1.8 91/01/18 09:54:49 ceriel Exp $ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <types.h>
#include <arch.h>
#include "object.h"
#include <unistd.h>

int
rd_arhdr(fd, arhdr)
	register struct ar_hdr	*arhdr;
{
	char buf[AR_TOTAL];
	register char *c = buf;
	register char *p = arhdr->ar_name;
	register int i;

	i = read(fd, c, AR_TOTAL);
	if (i == 0) return 0;
	if (i != AR_TOTAL) {
		rd_fatal();
	}
	i = 14;
	while (i--) {
		*p++ = *c++;
	}
	arhdr->ar_date.t_date = (int)get2(c); c += 2;
	arhdr->ar_date.t_time = (int)get2(c); c += 2;
	arhdr->ar_uid = *c++;
	arhdr->ar_gid = *c++;
	arhdr->ar_mode = get2(c); c += 2;
	arhdr->ar_size = (long) get2(c) << 16; c += 2;
	arhdr->ar_size |= (long) get2(c) & 0xffff;
	return 1;
}

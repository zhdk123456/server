/******************************************************
Copyright (c) 2011-2013 Percona LLC and/or its affiliates.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*******************************************************/

/* This file is required to abstract away regex(3) calls so that
my_regex is used on Windows and native calls are used on POSIX platforms. */

#ifndef XB_REGEX_H
#define XB_REGEX_H

#ifdef _WIN32
#include <pcreposix.h>
#else /* ! _WIN32 */
#include <regex.h>
#endif

typedef regex_t xb_regex_t;
typedef regmatch_t xb_regmatch_t;


#define xb_regexec(preg,string,nmatch,pmatch,eflags)	\
	regexec(preg, string, nmatch, pmatch, eflags)

#define xb_regerror(errcode,preg,errbuf,errbuf_size)	\
	regerror(errcode, preg, errbuf, errbuf_size)

#define xb_regcomp(preg,regex,cflags)				\
	regcomp(preg, regex, cflags)

#define xb_regfree(preg) regfree(preg)

#define xb_regex_end() do { } while (0)

#endif /* _WIN32 */


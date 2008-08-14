/*
getlogin_r is missing in mingw; the following code was suggested here  
http://article.gmane.org/gmane.comp.kde.devel.cygwin/410
in 2002-11-30 by Martin Fuchs <martin-fuchs@gmx.at> 

% $Id: getlogin_r.c,v 1.1 2008-08-11 07:55:24 schloegl Exp $
% Copyright (C) 2008 Alois Schloegl <a.schloegl@ieee.org>
% This file is part of the "BioSig for C/C++" repository 
% (biosig4c++) at http://biosig.sf.net/ 
    
 */

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <errno.h>

#define	EOK		0

#define	SUCCESS	EOK
#define	FAILURE	ERANGE

int getlogin_r(char* name, size_t namesize)
{
	HANDLE hToken;
	DWORD tu_len;
	TOKEN_USER* ptu;
	TCHAR domain[256];
	DWORD domain_len;
	DWORD name_len;
	SID_NAME_USE name_use;

	if (OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, &hToken) ||
		(GetLastError()==ERROR_NO_TOKEN && OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))) {

		ptu = 0;

		if (GetTokenInformation(hToken, TokenUser, 0, 0, &tu_len))
			return FAILURE;

		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return FAILURE;

		if (!(ptu = (TOKEN_USER*)alloca(tu_len)))
			return FAILURE;

		if (!GetTokenInformation(hToken, TokenUser, ptu, tu_len, &tu_len))
			return FAILURE;

		domain_len = 256;
		name_len = namesize;

		if (!LookupAccountSid(0, ptu->User.Sid, name, &name_len, domain, &domain_len, &name_use))
			return FAILURE;

		CloseHandle(hToken);
	}

	return SUCCESS;
}

/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

//$Id: LinException.h 5803 2009-01-18 08:46:51Z antonn $
#ifndef _LIN_EXCEPTION_H_INCLUDED_
#define _LIN_EXCEPTION_H_INCLUDED_

class CLinException :
    public exception
{
private:
    const CLinException& operator=(const CLinException&); //Do not allow assignments
public:
	CLinException()
        : errorString(string("Linux API error: ") + strerror(errno))
    {}
	CLinException(const string &strError, int nErrorCode = errno)
        : errorString(strError + ": " + strerror(nErrorCode))
    {}
	
	~CLinException() throw()
	{}
    const string errorString;
    virtual const char *what() const throw()
    {
        return errorString.c_str();
    }
};

#endif //_LIN_EXCEPTION_H_INCLUDED_

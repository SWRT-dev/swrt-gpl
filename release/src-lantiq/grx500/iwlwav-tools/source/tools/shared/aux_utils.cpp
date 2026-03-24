/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

//$Id: aux_utils.cpp 5805 2009-01-18 09:59:55Z antonn $
#include "mtlkinc.h"
#include "aux_utils.h"

//#define LOCAL_DBG_TRACE(x) DBG_TRACE("Utils: " << x) 
#define LOCAL_DBG_TRACE(x)

class CTokenizerException : public exception
{
private:
    const CTokenizerException& operator=(const CTokenizerException&); //Do not allow assignments
    const string errorString;

public:
	CTokenizerException(const string &strErrorText)
        : errorString(strErrorText)
    {}
    ~CTokenizerException() throw()
    {}
    virtual const char *what() const throw()
    {
        return errorString.c_str();
    }
};

CStrTokenizer::token_itr::token_itr(const CStrTokenizer& tokenizer,
                                    const string& separator,
                                    size_t position )
									: m_separator(separator)
                                    , m_tokenizer(tokenizer)
                                    , m_pos(position)
{
    size_t nextPos = m_tokenizer.m_string.find_first_of( m_separator, m_pos );
    if( nextPos != std::string::npos )
    {
        m_token = m_tokenizer.m_string.substr( m_pos, nextPos - m_pos );
        m_pos = nextPos + m_separator.length();
    }
    else
    {
        if( m_pos < m_tokenizer.m_string.length() )
        {
            m_token = m_tokenizer.m_string.substr( m_pos );
            m_pos = m_tokenizer.m_string.length();
        }
        else
        {
            m_pos = m_tokenizer.m_string.length()+1;
        }
    }
}
CStrTokenizer::token_itr::operator bool() const
{
    return m_pos <= m_tokenizer.m_string.length();
}
CStrTokenizer::token_itr& CStrTokenizer::token_itr::operator = (const token_itr& other)
{
    if( m_tokenizer != other.m_tokenizer )
    {
        throw CTokenizerException("Attempt to assign tokenizers that are not compatible");
    }
    m_pos = other.m_pos;
    m_token = other.m_token;
    m_separator = other.m_separator;
    return *this;
}
CStrTokenizer::token_itr& CStrTokenizer::token_itr::operator ++()
{
    token_itr temp(m_tokenizer, m_separator, m_pos);
    return (*this = temp);
}

#if defined(WIN32)
#include <windows.h>
#include "WinException.h"

void GetDirectoryContents(const string& strDir, string strFileExtension, vector<string>& Result, bool fFilesOnly)
{
    Result.clear();
    strFileExtension = strDir + GetFileSystemPathSeparator() + "*." + strFileExtension;

    WIN32_FIND_DATA DirEntry = {0};
    HANDLE hFind = ::FindFirstFile( strFileExtension.c_str(), &DirEntry );

    if(INVALID_HANDLE_VALUE == hFind)
    {//No files matching given mask found
        return;
    }

    do 
    {
        if((DirEntry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY 
            ||
            !fFilesOnly)
        {
            Result.push_back(DirEntry.cFileName);
        }
    }
    while(::FindNextFile( hFind, &DirEntry ));

    DWORD dwFindError = ::GetLastError();
    ::FindClose( hFind );
    if( ERROR_NO_MORE_FILES != dwFindError )
    {
        throw CWinException("Error while trying to enumerate directory " + strDir, dwFindError);
    }
}

#elif defined(LINUX)

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include "LinException.h"

void GetDirectoryContents(const string& strDir, string strFileExtension, vector<string>& Result, bool fFilesOnly)
{
	LOCAL_DBG_TRACE("Reading directory contents...");
	LOCAL_DBG_TRACE("\tDirectory: " << strDir);
	LOCAL_DBG_TRACE("\tLooking for files with extension: " << strFileExtension);
	LOCAL_DBG_TRACE("\tSkip directories: " << (fFilesOnly ? "yes" : "no"));

    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(strDir.c_str())) == NULL)
    {
    	throw CLinException("Error while trying to enumerate directory " + strDir);
    }

    while ((dirp = readdir(dp)) != NULL) 
    {
    	LOCAL_DBG_TRACE("\t\tEntry found: " << dirp->d_name);
    	
    	struct stat si;
    	string strEntryPath = strDir + "/" + dirp->d_name;
    	
    	if( 0 != stat(strEntryPath.c_str(), &si) )
        	throw CLinException("Error while reading info for " + strEntryPath);
    	
    	if ((S_ISDIR(si.st_mode) && !fFilesOnly) || S_ISREG(si.st_mode))
    	{
    		string strName(dirp->d_name);
    		
    		if ( ((strName == ".") || (strName == "..")) && S_ISDIR(si.st_mode) )
    		{
    	    	LOCAL_DBG_TRACE("\t\tEntry skipped because it is a 'special' directory");
    			continue;
    		}
    		
    		if(strName.length() < strFileExtension.length())
    		{
    	    	LOCAL_DBG_TRACE("\t\tEntry skipped because its name is shorter than extension we are looking for");
    			continue;
    		}
    		
    		if (strName.substr(strName.length() - strFileExtension.length()) == strFileExtension)
    		{
    	    	LOCAL_DBG_TRACE("\t\tEntry to be reported");
    			Result.push_back(strName);
    		}
    		else LOCAL_DBG_TRACE("\t\tEntry skipped because its extention does not math the one we are looking for");
    	}
    	else LOCAL_DBG_TRACE("\t\tEntry skipped because it is a directory");
    }
    
    closedir(dp);
}

#else
#error Unknown operating system
#endif

#if defined(WIN32)
#define os_getcwd _getcwd
#include <direct.h>
#elif defined(LINUX)
#include <unistd.h>
#define os_getcwd getcwd
#else
#endif

string GetCurrDir()
{
    char* buffer = os_getcwd( NULL, 0 );

    if(buffer)
    {
        string result((NULL == buffer) ? string() : string(buffer));
        free(buffer);
        return result;
    }
    else return string();
}

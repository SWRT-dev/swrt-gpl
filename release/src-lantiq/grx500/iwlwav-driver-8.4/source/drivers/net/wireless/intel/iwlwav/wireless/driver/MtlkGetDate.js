//
// $Id$
//
// Script for getting the current date for INF file
//
// (c) 2008 Metalink Broadband (Israel)
//
// Author: Vasyl' Nikolaenko
//

// Input/Output streams
var stdOut              = WScript.StdOut;

// Run main() and return it's result value to the caller
WScript.Quit(Main());

function Main()
{
    var date = new Date();
    var dateString = (date.getMonth() + 1) + "/";
    dateString += date.getDate() + "/";
    dateString += date.getYear();

    stdOut.WriteLine("#define MTLK_CURRENT_DATE " + dateString);

    return 0;
}

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
extern char* tzname[];

main()
{
    time_t now; // time_t == typedef from <sys/types.h>
    struct tm* sp;

    (void)time(&now);   // now == seconds from 01.01.1970, 00:00:00 UTC
    /*
        t1 = time (0); <=> returning value
        t3 = time (tp); <=> pointer
        (void) time (&t4); <=> address
    */

    printf("PRINT 1: %s", ctime(&now));  // ctime returns symbol's address == address of string's beginning, in the end there's endline-symbol
    // format: Sun Sep 16 01:03:52 1973\n\0

    setenv("TZ", "PST8PDT", 1);
    // changing to California time

    sp = localtime(&now);   // localtime returns tm-value's address
    /*
        localtime calls tzset to get the value of variable shell TZ
        using this value tzset initializes array of tzname-strings' pointers and external variables timezone and altzone
        timezone == difference between UTC and current time-zone (seconds)
        how do localtime works: counts difference between time and timezone and calls gmtime
    */

    printf("PRINT 2: %d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year + 1900, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);
    /*
        tm's member tm_isdst:
        > 0 <=> there's season time changing
        == 0 <=> no changing
        < 0 <=> no information
    */
    exit(0);
}

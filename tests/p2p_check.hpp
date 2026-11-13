#ifndef TESTS_P2P_CHECK_HPP
#define TESTS_P2P_CHECK_HPP

#include <stdio.h>

inline int& CheckFailures()
{
    static int failures = 0;
    return failures;
}

inline void Check(bool condition, const char* what)
{
    if (!condition)
    {
        ++CheckFailures();
        printf("FAIL %s\n", what);
    }
}

int RunCryptoTests();
int RunRendezvousTests();
int RunStreamTests();
int RunNodeTests();
int RunWireTests();
int RunIdentityTests();

#endif

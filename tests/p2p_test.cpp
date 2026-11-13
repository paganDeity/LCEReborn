#include <stdio.h>

#include "p2p_check.hpp"

int main()
{
    RunCryptoTests();
    RunRendezvousTests();
    RunStreamTests();
    RunNodeTests();
    RunWireTests();
    RunIdentityTests();
    printf(CheckFailures() == 0 ? "ok\n" : "%d failures\n", CheckFailures());
    return CheckFailures() == 0 ? 0 : 1;
}

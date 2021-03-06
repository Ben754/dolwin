#include "pch.h"

void SpinLock::Lock()
{
    while (_InterlockedCompareExchange(&_lock,
        1, // exchange
        0)  // comparand
        == 1)
    {
        // spin!
    }
}

void SpinLock::Unlock()
{
    _InterlockedExchange(&_lock, 0);
}

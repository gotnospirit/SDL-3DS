/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_THREAD_3DS

/* Semaphore functions for the 3DS. */

#include <stdio.h>
#include <stdlib.h>

#include "SDL_error.h"
#include "SDL_thread.h"

#include <3ds.h>

struct SDL_semaphore {
    Handle  semid;
};


/* Create a semaphore */
SDL_sem *SDL_CreateSemaphore(Uint32 initial_value)
{
    SDL_sem *sem;

    sem = (SDL_sem *) malloc(sizeof(*sem));
    if (sem != NULL) {
        /* TODO: Figure out the limit on the maximum value. */
        svcCreateSemaphore(&sem->semid, initial_value, 255);
        if (sem->semid < 0) {
            SDL_SetError("Couldn't create semaphore");
            free(sem);
            sem = NULL;
        }
    } else {
        SDL_OutOfMemory();
    }

    return sem;
}

/* Free the semaphore */
void SDL_DestroySemaphore(SDL_sem *sem)
{
    if (sem != NULL) {
        if (sem->semid > 0) {
	    svcCloseHandle(sem->semid);
            sem->semid = 0;
        }

        free(sem);
    }
}

/* TODO: This routine is a bit overloaded.
 * If the timeout is 0 then just poll the semaphore; if it's SDL_MUTEX_MAXWAIT, pass
 * NULL to sceKernelWaitSema() so that it waits indefinitely; and if the timeout
 * is specified, convert it to microseconds. */
int SDL_SemWaitTimeout(SDL_sem *sem, Uint32 timeout)
{
    Uint32 *pTimeout;
       unsigned int res;

    if (sem == NULL) {
        SDL_SetError("Passed a NULL sem");
        return 0;
    }

    if (timeout == 0) {
        res = 0;//sceKernelPollSema(sem->semid, 1); 3DS STUB
        if (res < 0) {
            return SDL_MUTEX_TIMEDOUT;
        }
        return 0;
    }

    if (timeout == SDL_MUTEX_MAXWAIT) {
        pTimeout = NULL;
    } else {
        timeout *= 1000;  /* Convert to microseconds. */
        pTimeout = &timeout;
    }

    /*res = sceKernelWaitSema(sem->semid, 1, pTimeout); 3DS STUB
       switch (res) {
               case SCE_KERNEL_ERROR_OK:
                       return 0;
               case SCE_KERNEL_ERROR_WAIT_TIMEOUT:
                       return SDL_MUTEX_TIMEDOUT;
               default:
                       return SDL_SetError("WaitForSingleObject() failed");
    }*/
    return 0;
}

int SDL_SemTryWait(SDL_sem *sem)
{
    return SDL_SemWaitTimeout(sem, 0);
}

int SDL_SemWait(SDL_sem *sem)
{
    return SDL_SemWaitTimeout(sem, SDL_MUTEX_MAXWAIT);
}

/* Returns the current count of the semaphore */
Uint32 SDL_SemValue(SDL_sem *sem)
{
    //SceKernelSemaInfo info; 3DS STUB

    if (sem == NULL) {
        SDL_SetError("Passed a NULL sem");
        return 0;
    }

    /*if (sceKernelReferSemaStatus(sem->semid, &info) >= 0) { 3DS STUB
        return info.currentCount;
    }*/

    return 0;
}

int SDL_SemPost(SDL_sem *sem)
{
    int res;

    if (sem == NULL) {
        return SDL_SetError("Passed a NULL sem");
    }

    res = svcReleaseSemaphore(NULL, sem->semid, 1);
    if (res < 0) {
        return SDL_SetError("svcReleaseSemaphore() failed");
    }

    return 0;
}

#endif /* SDL_THREAD_3DS */

/* vim: ts=4 sw=4
 */

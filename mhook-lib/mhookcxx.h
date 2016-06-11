//Copyright (c) 2016 Felix Krull
//
//Permission is hereby granted, free of charge, to any person obtaining a 
//copy of this software and associated documentation files (the "Software"), 
//to deal in the Software without restriction, including without limitation 
//the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the 
//Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included 
//in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
//OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
//THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
//FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
//IN THE SOFTWARE.

#ifndef _MHOOK_CXX_H_
#define _MHOOK_CXX_H_

#include "mhook.h"

namespace mhookcxx {

/* Example:
 *
 * #include <Windows.h>
 * #include "mhookcxx.h"
 *
 * auto QPC = mhookcxx::create(&QueryPerformanceCounter);
 *
 * BOOL WINAPI QPC_Hook(_Out_ LARGE_INTEGER* c) {
 *   printf("QPC_HOOK\n");
 *   return QPC.orig(c);
 * }
 *
 * int main() {
 *   if (QPC.install(&QPC_Hook) {
 *     LARGE_INTEGER c;
 *     QueryPerformance(&c);
 *   }
 *   QPC.uninstall();
 * }
 */

template <class FuncT> class hook {
    bool installed;
public:
    FuncT* orig;

    hook(FuncT* origFunc): installed(false), orig(origFunc) { }
    hook(void* origFunc): hook((FuncT*) origFunc) { }

    virtual ~hook() {
        uninstall();
    }

    bool install(FuncT* hookFunc) {
        uninstall();
        if (!orig || !hookFunc || !Mhook_SetHook((void**) &orig, hookFunc)) {
            return false;
        }
        installed = true;
        return true;
    }

    bool install(void* hookFunc) {
        install((FuncT*) hookFunc);
    }

    void uninstall() {
        if (installed && orig) {
            Mhook_Unhook((void**) &orig);
        }
        installed = false;
    }


    // Only move constructors, no copy.
    hook(hook&& other) {
        *this = other;
    }

    hook& operator=(hook&& other) {
        if (this == &other) {
            return *this;
        }
        installed = other.installed;
        orig = other.orig;
        other.installed = false;
        other.orig = nullptr;
        return *this;
    }

    hook(const hook&) = delete;
    hook& operator=(const hook& other) = delete;
};

template <class FuncT> hook<FuncT> create(FuncT* origFunc) {
    return hook<FuncT>(origFunc);
}

}

#endif // _MHOOK_CXX_H

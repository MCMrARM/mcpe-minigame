#pragma once

#include "util/Log.h"

namespace {
void patchCallInstruction(void* patchOff, void* func, bool jump) {
    unsigned char* data = (unsigned char*) patchOff;
    Log::trace("Hook", "Patching - original: %i %i %i %i %i", data[0], data[1], data[2], data[3], data[4]);
    data[0] = (unsigned char) (jump ? 0xe9 : 0xe8);
    int ptr = ((int) func) - (int) patchOff - 5;
    memcpy(&data[1], &ptr, sizeof(int));
    Log::trace("Hook", "Patching - result: %i %i %i %i %i", data[0], data[1], data[2], data[3], data[4]);
}
}
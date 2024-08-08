#pragma once

#if __has_include(<alloca.h>)
#include <alloca.h>
#else
#include <stdlib.h>
#include <malloc.h>
#ifndef alloca
#define alloca _alloca
#endif
#endif

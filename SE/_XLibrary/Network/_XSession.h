#pragma once

#ifdef WIN32
#include "XSessionWinsock.h"
#else
#include "XSessioniOSGK.h"
#include "XSessioniOSGKMatch.h"
#endif
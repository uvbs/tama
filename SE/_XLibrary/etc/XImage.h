/*
 *  XImage.h
 *
 *  Created by xuzhu on 11. 5. 19..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once

#ifdef WIN32
    #include "XImageD3D.h"
#else
    #include "XImageiOS.h"
#endif
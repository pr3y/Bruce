/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** mmclist.c
**
** list of all mapper interfaces
** $Id: mmclist.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "nes_mmc.h"

/* mapper interfaces */
extern mapintf_t map0_intf;
extern mapintf_t map1_intf;
extern mapintf_t map2_intf;
extern mapintf_t map3_intf;
extern mapintf_t map4_intf;
extern mapintf_t map5_intf;
extern mapintf_t map7_intf;
extern mapintf_t map8_intf;
extern mapintf_t map9_intf;
extern mapintf_t map10_intf;
extern mapintf_t map11_intf;
extern mapintf_t map15_intf;
extern mapintf_t map16_intf;
extern mapintf_t map18_intf;
extern mapintf_t map19_intf;
extern mapintf_t map21_intf;
extern mapintf_t map22_intf;
extern mapintf_t map23_intf;
extern mapintf_t map24_intf;
extern mapintf_t map25_intf;
extern mapintf_t map32_intf;
extern mapintf_t map33_intf;
extern mapintf_t map34_intf;
extern mapintf_t map40_intf;
extern mapintf_t map64_intf;
extern mapintf_t map65_intf;
extern mapintf_t map66_intf;
extern mapintf_t map70_intf;
extern mapintf_t map75_intf;
extern mapintf_t map78_intf;
extern mapintf_t map79_intf;
extern mapintf_t map85_intf;
extern mapintf_t map94_intf;
extern mapintf_t map99_intf;
extern mapintf_t map231_intf;

/* implemented mapper interfaces */
mapintf_t *mappers[] =
    {
        &map0_intf,
        &map1_intf,
        &map2_intf,
        &map3_intf,
        &map4_intf,
        &map5_intf,
        &map7_intf,
        &map8_intf,
        &map9_intf,
        &map11_intf,
        &map15_intf,
        &map16_intf,
        &map18_intf,
        &map19_intf,
        &map21_intf,
        &map22_intf,
        &map23_intf,
        &map24_intf,
        &map25_intf,
        &map32_intf,
        &map33_intf,
        &map34_intf,
        &map40_intf,
        &map64_intf,
        &map65_intf,
        &map66_intf,
        &map70_intf,
        &map75_intf,
        &map78_intf,
        &map79_intf,
        &map85_intf,
        &map94_intf,
        &map99_intf,
        &map231_intf,
        NULL};

/*
** $Log: mmclist.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.2  2000/10/10 13:05:30  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.1  2000/07/31 04:27:39  matt
** initial revision
**
*/

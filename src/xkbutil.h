#ifndef __XKBUTIL_H__
#define __XKBUTIL_H__

#include <ibus.h>

GList           *ibus_xkb_list_engines     (void);
IBusComponent   *ibus_xkb_get_component    (void);

#endif

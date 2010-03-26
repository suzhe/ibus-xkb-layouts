#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <ibus.h>

#define IBUS_TYPE_XKB_LAYOUT_ENGINE (ibus_xkb_layout_engine_get_type ())

GType   ibus_xkb_layout_engine_get_type    (void);

#endif

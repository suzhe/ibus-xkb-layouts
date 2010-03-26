#include "engine.h"

typedef struct _IBusXkbLayoutEngine IBusXkbLayoutEngine;
typedef struct _IBusXkbLayoutEngineClass IBusXkbLayoutEngineClass;

struct _IBusXkbLayoutEngine {
    IBusEngine parent;
};

struct _IBusXkbLayoutEngineClass {
    IBusEngineClass parent;
};

/* functions prototype */
static void     ibus_xkb_layout_engine_class_init       (IBusXkbLayoutEngineClass   *klass);
static void     ibus_xkb_layout_engine_init             (IBusXkbLayoutEngine        *engine);
static void     ibus_xkb_layout_engine_destroy          (IBusXkbLayoutEngine        *engine);
static gboolean ibus_xkb_layout_engine_process_key_event(IBusEngine                 *engine,
                                                         guint                       keyval,
                                                         guint                       keycode,
                                                         guint                       modifiers);

G_DEFINE_TYPE (IBusXkbLayoutEngine, ibus_xkb_layout_engine, IBUS_TYPE_ENGINE)

static void
ibus_xkb_layout_engine_class_init (IBusXkbLayoutEngineClass *klass)
{
    IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
    IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);

    ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_xkb_layout_engine_destroy;

    engine_class->process_key_event = ibus_xkb_layout_engine_process_key_event;
}

static void
ibus_xkb_layout_engine_init (IBusXkbLayoutEngine *xkb_layout)
{
}

static void
ibus_xkb_layout_engine_destroy (IBusXkbLayoutEngine *xkb_layout)
{
    ((IBusObjectClass *) ibus_xkb_layout_engine_parent_class)->destroy ((IBusObject *)xkb_layout);
}

static gboolean
ibus_xkb_layout_engine_process_key_event (IBusEngine *engine,
                                          guint       keyval,
                                          guint       keycode,
                                          guint       modifiers)
{
    /* TODO: Support Compose/Dead keys */
    return FALSE;
}

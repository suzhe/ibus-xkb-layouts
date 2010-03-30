#include <string.h>
#include <glib.h>
#include <X11/Xlib.h>
#include <libxklavier/xklavier.h>

#include "xkbutil.h"

static IBusEngineDesc *
ibus_xkb_engine_new (const gchar *layout,
                     const gchar *layout_desc,
                     const gchar *variant,
                     const gchar *variant_desc,
                     const gchar *language)
{
    IBusEngineDesc *engine;
    gchar *engine_name = NULL;
    gchar *engine_longname = NULL;
    gchar *engine_layout = NULL;

    engine_name = g_strdup_printf ("xkb:%s:%s:%s", layout, variant, language);
    if (variant_desc && *variant_desc) {
        engine_longname = g_strdup_printf ("%s - %s", layout_desc, variant_desc);
    }

    if (variant && *variant) {
        engine_layout = g_strdup_printf ("%s(%s)", layout, variant);
    }

    engine = ibus_engine_desc_new (engine_name,
                                   engine_longname ? engine_longname : layout_desc,
                                   "",
                                   language,
                                   "",
                                   "",
                                   "",
                                   engine_layout ? engine_layout : layout);

    /* It's a keyboard layout */
    engine->is_layout = TRUE;

    /* set default rank to 0 */
    engine->rank = 0;

    if (g_strcmp0(layout, "us") == 0 && (!variant || !*variant)) {
        engine->rank = 100;
    }

    g_free (engine_name);
    g_free (engine_longname);
    g_free (engine_layout);

    return engine;
}

struct ListEnginesCallbackData
{
    GList *engines;
    const gchar *language;
};

static void
ibus_xkb_layout_proc (XklConfigRegistry   *registry,
                      const XklConfigItem *item,
                      const XklConfigItem *subitem,
                      gpointer data)
{
    struct ListEnginesCallbackData *callback_data = (struct ListEnginesCallbackData *) data;

    if (subitem) {
        callback_data->engines = g_list_prepend (callback_data->engines, ibus_xkb_engine_new (
            item->name, item->description, subitem->name, subitem->description, callback_data->language));
    }
    else {
        callback_data->engines = g_list_prepend (callback_data->engines, ibus_xkb_engine_new (
            item->name, item->description, "", "", callback_data->language));
    }
}

static void
ibus_xkb_language_proc (XklConfigRegistry   *registry,
                        const XklConfigItem *item,
                        gpointer data)
{
    struct ListEnginesCallbackData *callback_data = (struct ListEnginesCallbackData *) data;

    callback_data->language = item->name;

    xkl_config_registry_foreach_language_variant (registry, item->name, ibus_xkb_layout_proc, data);
}

GList *
ibus_xkb_list_engines (void)
{
    XklEngine *xkl_engine = NULL;
    XklConfigRegistry *xkl_registry = NULL;
    Display *display = NULL;
    struct ListEnginesCallbackData callback_data = { NULL, NULL };

    display = XOpenDisplay (NULL);
    if (display == NULL) {
        return NULL;
    }

    xkl_engine = xkl_engine_get_instance (display);
    if (xkl_engine == NULL) {
        goto out;
    }

    xkl_registry = xkl_config_registry_get_instance (xkl_engine);
    if (!xkl_registry || !xkl_config_registry_load (xkl_registry, TRUE)) {
        goto out;
    }

    xkl_config_registry_foreach_language(xkl_registry, ibus_xkb_language_proc, &callback_data);

out:
    if (xkl_registry) {
        g_object_unref (xkl_registry);
    }
    if (xkl_engine) {
        g_object_unref (xkl_engine);
    }
    if (display) {
        XCloseDisplay (display);
    }

    return callback_data.engines ? g_list_reverse (callback_data.engines) : NULL;
}

IBusComponent *
ibus_xkb_get_component (void)
{
    GList *engines, *p;
    IBusComponent *component;

    component = ibus_component_new ("org.freedesktop.IBus.XKBLayouts",
                                    "XKB Layouts Component",
                                    "0.0.0",
                                    "",
                                    "James Su <james.su@gmail.com>",
                                    "http://github.com/suzhe/ibus-xkb-layouts",
                                    "",
                                    "ibus-xkb-layouts");

    engines = ibus_xkb_list_engines ();

    for (p = engines; p != NULL; p = p->next) {
        ibus_component_add_engine (component, (IBusEngineDesc *) p->data);
    }

    g_list_free (engines);
    return component;
}

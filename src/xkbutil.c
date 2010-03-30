#include <string.h>
#include <glib.h>
#include "xkbutil.h"

#ifndef XKB_RULES_XML_FILE
#define XKB_RULES_XML_FILE "/usr/share/X11/xkb/rules/evdev.xml"
#endif

static IBusEngineDesc *
ibus_xkb_engine_new (gchar    *layout,
                     gchar    *layout_desc,
                     gchar    *variant,
                     gchar    *variant_desc,
                     gchar    *language)
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

static GList *
ibus_xkb_create_engines (GList *engines,
                         gchar *layout,
                         gchar *layout_desc,
                         gchar *variant,
                         gchar *variant_desc,
                         GList *langs)
{
    GList *l;
    for (l = langs; l != NULL; l = l->next) {
        engines = g_list_prepend (engines, ibus_xkb_engine_new (layout, layout_desc, variant, variant_desc, (gchar *) l->data));
    }

    return engines;
}

static void
ibus_xkb_parse_config_item (XMLNode  *node,
                            gchar   **name,
                            gchar   **desc,
                            GList   **langs)
{
    GList *p = NULL;
    for (p = node->sub_nodes; p != NULL; p = p->next) {
        XMLNode *sub_node = (XMLNode *) p->data;
        if (g_strcmp0 (sub_node->name, "name") == 0) {
            *name = sub_node->text;
        }
        else if (g_strcmp0 (sub_node->name, "description") == 0) {
            *desc = sub_node->text;
        }
        else if (g_strcmp0 (sub_node->name, "languageList") == 0) {
            GList *l = NULL;
            for (l = sub_node->sub_nodes; l != NULL; l = l->next) {
                XMLNode *lang_node = (XMLNode *) l->data;
                if (g_strcmp0 (lang_node->name, "iso639Id") == 0) {
                    *langs = g_list_prepend (*langs, lang_node->text);
                }
            }
        }
    }
    if (*langs) {
        *langs = g_list_reverse (*langs);
    }
}

static GList *
ibus_xkb_parse_variant_list (GList *engines,
                             XMLNode *node,
                             gchar *layout_name,
                             gchar *layout_desc,
                             GList *layout_langs)
{
    GList *p = NULL;
    for (p = node->sub_nodes; p != NULL; p = p->next) {
        XMLNode *variant = (XMLNode *) p->data;
        if (g_strcmp0 (variant->name, "variant") == 0 && variant->sub_nodes) {
            XMLNode *config = (XMLNode *) variant->sub_nodes->data;
            if (g_strcmp0 (config->name, "configItem") == 0) {
                gchar *name = NULL;
                gchar *desc = NULL;
                GList *langs = NULL;

                ibus_xkb_parse_config_item (config, &name, &desc, &langs);

                if (name && desc && (langs || layout_langs)) {
                    engines = ibus_xkb_create_engines (engines, layout_name, layout_desc, name, desc,
                                                       langs ? langs : layout_langs);
                }

                g_list_free (langs);
            }
        }
    }

    return engines;
}

static GList *
ibus_xkb_parse_layout (GList   *engines,
                       XMLNode *layout)
{
    gchar *name = NULL;
    gchar *desc = NULL;
    GList *langs = NULL;
    GList *p = NULL;

    for (p = layout->sub_nodes; p != NULL; p = p->next) {
        XMLNode *sub_node = (XMLNode *) p->data;
        if (g_strcmp0 (sub_node->name, "configItem") == 0 && !name) {
            ibus_xkb_parse_config_item (sub_node, &name, &desc, &langs);

            if (name && desc && langs) {
                engines = ibus_xkb_create_engines (engines, name, desc, "", "", langs);
            }
        }
        else if (g_strcmp0 (sub_node->name, "variantList") == 0 && name) {
            engines = ibus_xkb_parse_variant_list (engines, sub_node, name, desc, langs);
        }
    }

    g_list_free (langs);

    return engines;
}

static GList *
ibus_xkb_parse_layout_list (GList   *engines,
                            XMLNode *layout_list)
{
    GList *p = NULL;
    for (p = layout_list->sub_nodes; p != NULL; p = p->next) {
        XMLNode *layout = (XMLNode *) p->data;

        if (g_strcmp0 (layout->name, "layout") == 0) {
           engines = ibus_xkb_parse_layout(engines, layout);
        }
    }

    return engines;
}

GList *
ibus_xkb_list_engines (void)
{
    GList *engines = NULL;
    GList *p = NULL;
    XMLNode *xkb_rules_xml = ibus_xml_parse_file (XKB_RULES_XML_FILE);

    if (!xkb_rules_xml) {
        return NULL;
    }

    if (g_strcmp0 (xkb_rules_xml->name, "xkbConfigRegistry") != 0) {
        g_warning ("File %s is not a valid XKB rules xml file.", XKB_RULES_XML_FILE);
        ibus_xml_free (xkb_rules_xml);
        return NULL;
    }

    for (p = xkb_rules_xml->sub_nodes; p != NULL; p = p->next) {
        XMLNode *sub_node = (XMLNode *) p->data;

        if (g_strcmp0 (sub_node->name, "layoutList") == 0) {
           engines = ibus_xkb_parse_layout_list (engines, sub_node);
        }
    }

    ibus_xml_free (xkb_rules_xml);
    return g_list_reverse (engines);
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

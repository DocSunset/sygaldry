#include "m_pd.h"

static t_class *edge_detect_class;

typedef struct _edge_detect {
    t_object x_obj;
    int previous_button_state;
    t_outlet *any_edge, *falling_edge, *rising_edge;
} t_edge_detect;

void edge_detect_float_method(t_edge_detect *ed, t_floatarg f)
{
    int rising_edge = 0;
    int falling_edge = 0;
    int any_edge;
    int button_state = f != 0;

    if (ed->previous_button_state != button_state)
    {
        ed->previous_button_state = button_state;
        rising_edge = button_state;
        falling_edge = button_state == 0;
    }
    any_edge = rising_edge || falling_edge;

    if (rising_edge) outlet_float(ed->rising_edge, 1);
    else outlet_float(ed->rising_edge, 0);

    if (falling_edge) outlet_float(ed->falling_edge, 1);
    else outlet_float(ed->falling_edge, 0);

    if (any_edge) outlet_float(ed->any_edge, 1);
    else outlet_float(ed->any_edge, 0);
}

void *edge_detect_new(t_symbol *s, int argc, t_atom *argv)
{
    t_edge_detect *ed = (t_edge_detect *)pd_new(edge_detect_class);
    ed->previous_button_state = atom_getfloatarg(0, argc, argv) != 0;
    ed->any_edge = outlet_new(&ed->x_obj, &s_float);
    ed->falling_edge = outlet_new(&ed->x_obj, &s_float);
    ed->rising_edge = outlet_new(&ed->x_obj, &s_float);
    return (void *)ed;
}

void edge_detect_setup(void)
{
    edge_detect_class = class_new(gensym("edge_detect"),
        (t_newmethod)edge_detect_new,
        0, sizeof(t_edge_detect),
        CLASS_DEFAULT,
        A_GIMME, 0);

    class_addfloat(edge_detect_class, (t_method)edge_detect_float_method);
}

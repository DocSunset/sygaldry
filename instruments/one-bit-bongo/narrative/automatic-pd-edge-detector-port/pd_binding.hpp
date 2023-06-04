#include <type_traits>
#include "m_pd.h"
#include <boost/pfr.hpp>
#include "utilities/spelling.hpp"


template<typename Processor>
static t_class *processor_class;

template<typename Processor>
struct t_processor
{
    t_object x_obj;
    t_atom output;
    Processor processor;
};

template<typename Endpoint>
void set_value_from_float(Endpoint& ep, t_float f)
{
    if constexpr (std::is_same_v<decltype(ep.value), bool>)
    {
        ep.value = f != 0.0f;
    }
    else if constexpr (std::is_floating_point_v<decltype(ep.value)> || std::is_integral_v<decltype(ep.value)>)
    {
        ep.value = f;
    }
}

template<typename Processor, typename Input>
void setter_method(t_processor<Processor> *obj, t_float f)
{
    boost::pfr::for_each_field(obj->processor.inputs, [&f](auto& ep)
    {
        if constexpr (std::is_same_v<Input, std::remove_cvref_t<decltype(ep)>>)
        {
            set_value_from_float(ep, f);
        }
    });
}

template<typename Processor>
void bang_method(t_processor<Processor> *obj)
{
    using sygaldry::spelling::lower_kebab_case;
    obj->processor();
    boost::pfr::for_each_field(obj->processor.outputs, [&](auto endpoint)
    {
        SETFLOAT(&obj->output, static_cast<t_float>(endpoint.value));
        outlet_anything(obj->x_obj.ob_outlet, gensym(lower_kebab_case(endpoint)), 1, &obj->output);
    });
};

template<typename Processor>
void * processor_new(t_symbol, int argc, t_atom *argv)
{
    t_processor<Processor> *obj = (t_processor<Processor> *)pd_new(processor_class<Processor>);

    int i = 0;
    boost::pfr::for_each_field(obj->processor.inputs, [&](auto& ep)
    {
        if (i >= argc) return;
        set_value_from_float(ep, atom_getfloatarg(i, argc, argv));
        ++i;
    });
    obj->processor();

    outlet_new(&obj->x_obj, 0);
    return (void *)obj;
}

template<typename Processor>
void processor_setup_internal()
{
    processor_class<Processor> = class_new(gensym(sygaldry::spelling::lower_snake_case_v<Processor>),
        (t_newmethod)processor_new<Processor>,
        0, sizeof(t_processor<Processor>),
        CLASS_DEFAULT,
        A_GIMME, 0);

    boost::pfr::for_each_field(Processor{}.inputs, [](auto endpoint)
    {
        class_addmethod(processor_class<Processor>,
                        (t_method)setter_method<Processor, decltype(endpoint)>,
                        gensym(sygaldry::spelling::lower_kebab_case(endpoint)),
                        A_FLOAT, 0);
    });
    class_addbang(processor_class<Processor>, bang_method<Processor>);
}

#define MAKE_PD_BINDING(LIBNAME, Processor) \
extern "C" void LIBNAME##_setup(void) \
{ \
    processor_setup_internal<Processor>(); \
}

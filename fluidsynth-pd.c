/*
 * HOWTO write an External for Pure data
 * (c) 2001-2006 IOhannes m zmölnig zmoelnig[AT]iem.at
 *
 * this is the source-code for the fourth example in the HOWTO
 * it creates a simple dsp-object:
 * 2 input signals are mixed into 1 output signal
 * the mixing-factor can be set via the 3rd inlet
 *
 * for legal issues please see the file LICENSE.txt
 */


/**
 * include the interface to Pd
 */
#include "m_pd.h"
#include "fluidsynth.h"

/**
 * define a new "class"
 */
static t_class *fluidsynth_tilde_class;


/**
 * this is the dataspace of our new object
 * the first element is the mandatory "t_object"
 
 */
typedef struct _fluidsynth_tilde {
    t_object  x_obj;
    fluid_synth_t *synth;
    fluid_settings_t *settings;
    t_inlet *x_note_in,*x_pgm_change_in;
    t_outlet *x_outleft;
    t_outlet *x_outright;
    
} t_fluidsynth_tilde;


/**
 * this is the core of the object
 * this perform-routine is called for each signal block
 * the name of this function is arbitrary and is registered to Pd in the
 * pan_tilde_dsp() function, each time the DSP is turned on
 *
 * the argument to this function is just a pointer within an array
 * we have to know for ourselves how many elements inthis array are
 * reserved for us (hint: we declare the number of used elements in the
 * pan_tilde_dsp() at registration
 *
 * since all elements are of type "t_int" we have to cast them to whatever
 * we think is apropriate; "apropriate" is how we registered this function
 * in pan_tilde_dsp()
 */
t_int *fluidsynth_tilde_perform(t_int *w)
{
    /* the first element is a pointer to the dataspace of this object */
    t_fluidsynth_tilde *x = (t_fluidsynth_tilde *)(w[1]);
    /* here is a pointer to the t_sample arrays that hold the 2 input signals */
    t_sample  *outleft =    (t_sample *)(w[2]);
    t_sample  *outright =    (t_sample *)(w[3]);
    /* all signalblocks are of the same length */
    int          n =           (int)(w[4]);
 
    /* just a counter */
    int i;

    /* this is the main routine:
     * mix the 2 input signals into the output signal
     */
    float * bufs[2] = {outleft,outright};
    
    for(i=0; i<n; i++)
    {
    
        outleft[i]=outright[i] =0.0f;
    }

    if (x->synth != NULL) {
        fluid_synth_process(x->synth, n, 0, NULL, 2, bufs);
    }
    
    /* return a pointer to the dataspace for the next dsp-object */
    return (w+5);
}


/**
 * register a special perform-routine at the dsp-engine
 * this function gets called whenever the DSP is turned ON
 * the name of this function is registered in pan_tilde_setup()
 */
void fluidsynth_tilde_dsp(t_fluidsynth_tilde *x, t_signal **sp)
{
    /* add pan_tilde_perform() to the DSP-tree;
     * the pan_tilde_perform() will expect "4" arguments (packed into an
     * t_int-array), which are:
     * the objects data-space, 2 signal vectors (which happen to be
     *  2 output signals) and the length of the
     * signal vectors (all vectors are of the same length)
     */
    dsp_add(fluidsynth_tilde_perform, 4, x,
            sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

/**
 * this is the "destructor" of the class;
 * it allows us to free dynamically allocated ressources
 */
void fluidsynth_tilde_free(t_fluidsynth_tilde *x)
{

    /* free any ressources associated with the given outlet */
    outlet_free(x->x_outleft);
    outlet_free(x->x_outright);
    inlet_free(x->x_note_in);
    inlet_free(x->x_pgm_change_in);
    
    /* delete fluid synth instance */
    delete_fluid_synth(x->synth);
    delete_fluid_settings(x->settings);
    
}

/**
 * this is the "constructor" of the class
 * the argument is the initial mixing-factor
 */
void *fluidsynth_tilde_new(void)
{
    t_fluidsynth_tilde *x = (t_fluidsynth_tilde *)pd_new(fluidsynth_tilde_class);
    sys_getsr();
    x->settings = new_fluid_settings();
    fluid_settings_setint(x->settings,"synth.polyphony", 64);
    fluid_settings_setnum(x->settings,"synth.sample-rate",(double)sys_getsr());
    fluid_settings_setint(x->settings,"synth.threadsafe-api",1);
    fluid_settings_setint(x->settings,"synth.verbose",1);

    

    x->synth = new_fluid_synth(x->settings);

    if (x->synth == NULL) {
        poststring("Error launching fluidsynth");
    } else {
        post("fluidsynth is launched !!! sr = %f",(double)sys_getsr());
    }
    /*create inlets*/
    x->x_note_in = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_list, gensym("note_in"));
    x->x_pgm_change_in = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_list, gensym("pgm_change_in"));

    /* create a new signal-outlet */
    x->x_outleft = outlet_new(&x->x_obj, &s_signal);
    x->x_outright = outlet_new(&x->x_obj, &s_signal);

    return (void *)x;
}

void fluidsynth_tilde_load_font(t_fluidsynth_tilde *x,t_symbol *sym) {
    char buf[MAXPDSTRING];

    t_atom at;
    SETSYMBOL(&at,sym);
    atom_string(&at, buf, MAXPDSTRING);
 
    FILE * testit = fopen(buf,"rb");
    
    if(testit == NULL) {
        post ("could not open %s ",buf);
        return;
    }
    
    fclose(testit);
    if( x->synth != NULL) {
        int rval = fluid_synth_sfload(x->synth, buf, 1);
        if (rval == FLUID_FAILED) {
            post ("fluidsynth could not open sound font %s",buf);
        } else {
            post ("sound font loaded %s",buf);
        }
    }
}

void fluid_note_in(t_fluidsynth_tilde *x,int note,int velocity,int channel) {

    if (x->synth == NULL) {return;}
    if (note < 1 || note > 128) {return;}
    if (velocity < 0 || velocity > 127) {return;}
    if (channel < 1 || channel > 16) {return;}

    if (velocity != 0) {
        fluid_synth_noteon(x->synth, channel, note, velocity);
    } else {
        fluid_synth_noteoff(x->synth, channel, note);
    }
}

void fluid_pgm_change_in(t_fluidsynth_tilde *x,int channel,int pgm) {
    if (x->synth == NULL) {return;}
    if (channel < 1 || channel > 16) {return;}
    if (pgm < 1 || pgm >128) {return;}

    fluid_synth_program_change(x->synth, channel, pgm);
    post("pgm change chnl : %i  pgm : %i",channel,pgm);
}

void fluidsynth_tilde_note_in(t_fluidsynth_tilde *x,t_symbol *sym,t_int argc, t_atom *argv){
switch(argc){
        //lists must have 3 ints
    case 3:
        
        fluid_note_in(x, atom_getint(argv), atom_getint(argv+1), atom_getint(argv+2));
        
        break;
    default:
        error("error: three arguments are needed for note-in");
    }
}

void fluidsynth_tilde_pgm_change_in(t_fluidsynth_tilde *x,t_symbol *sym,t_int argc, t_atom *argv){
switch(argc){
        //lists must have 2 ints
    case 2:
        
        fluid_pgm_change_in(x, atom_getint(argv+1), atom_getint(argv));
        
        break;
    default:
        error("error: two arguments are needed for pgm change");
    }
}


/**
 * define the function-space of the class
 * within a single-object external the name of this function is very special
 */
void fluidsynth_tilde_setup(void) {
    fluidsynth_tilde_class = class_new(gensym("fluidsynth~"),
                                (t_newmethod)fluidsynth_tilde_new,
                                (t_method)fluidsynth_tilde_free,
                                sizeof(t_fluidsynth_tilde),
                                CLASS_DEFAULT,
                                 0);

    /* whenever the audio-engine is turned on, the "pan_tilde_dsp()"
     * function will get called
     */
    class_addmethod(fluidsynth_tilde_class,
                    (t_method)fluidsynth_tilde_dsp, gensym("dsp"), 0);
    
    class_addmethod(fluidsynth_tilde_class,
                    (t_method)fluidsynth_tilde_load_font, gensym("load"), A_SYMBOL, 0);
                    
    class_addmethod(fluidsynth_tilde_class, (t_method)fluidsynth_tilde_note_in, gensym("note_in"), A_GIMME,0);
    class_addmethod(fluidsynth_tilde_class, (t_method)fluidsynth_tilde_pgm_change_in, gensym("pgm_change_in"), A_GIMME,0);

    
}

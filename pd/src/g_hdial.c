/* Copyright (c) 1997-1999 Miller Puckette.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution. */

/* g_7_guis.c written by Thomas Musil (c) IEM KUG Graz Austria 2000-2001 */
/* thanks to Miller Puckette, Guenther Geiger and Krzystof Czaja */

/* name change to hradio by MSP and changed to
put out a "float" as in sliders, toggles, etc. */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "m_pd.h"
#include "g_canvas.h"
#include "t_tk.h"
#include "g_all_guis.h"
#include <math.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_IO_H
#include <io.h>
#endif

extern int gfxstub_haveproperties(void *key);
void hradio_draw_select(t_hradio* x, t_glist* glist);

/* ------------- hdl     gui-horicontal dial ---------------------- */

t_widgetbehavior hradio_widgetbehavior;
static t_class *hradio_class, *hradio_old_class;

/* widget helper functions */

void hradio_draw_update(t_gobj *client, t_glist *glist)
{
    t_hradio *x = (t_hradio *)client;
    if(glist_isvisible(glist))
    {
        t_canvas *canvas=glist_getcanvas(glist);

        sys_vgui(".x%lx.c itemconfigure %lxBUT%d -fill #%6.6x -stroke #%6.6x\n",
                 canvas, x, x->x_drawn,
                 x->x_gui.x_bcol, x->x_gui.x_bcol);
        sys_vgui(".x%lx.c itemconfigure %lxBUT%d -fill #%6.6x -stroke #%6.6x\n",
                 canvas, x, x->x_on,
                 x->x_gui.x_fcol, x->x_gui.x_fcol);
        x->x_drawn = x->x_on;
    }
}
void hradio_draw_io(t_hradio* x, t_glist* glist, int old_snd_rcv_flags)
{
    t_canvas *canvas=glist_getcanvas(glist);
    iemgui_io_draw(&x->x_gui,canvas,old_snd_rcv_flags,"HRDO");
}
void hradio_draw_new(t_hradio *x, t_glist *glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int n=x->x_number, i, dx=x->x_gui.x_w, s4=dx/4;
    int yy11=text_ypix(&x->x_gui.x_obj, glist), yy12=yy11+dx;
    int yy21=yy11+s4, yy22=yy12-s4;
    int xx11b=text_xpix(&x->x_gui.x_obj, glist), xx11=xx11b, xx21=xx11b+s4;
    int xx22=xx11b+dx-s4;

    scalehandle_draw_new(x->x_gui. x_handle,canvas);
    scalehandle_draw_new(x->x_gui.x_lhandle,canvas);

        char *nlet_tag = iem_get_tag(glist, (t_iemgui *)x);

        for(i=0; i<n; i++)
        {
            sys_vgui(".x%lx.c create prect %d %d %d %d "
                     "-stroke $pd_colors(iemgui_border) -fill #%6.6x "
                     "-tags {%lxBASE%d %lxBASE %lxHRDO %s text iemgui border}\n",
                 canvas, xx11, yy11, xx11+dx, yy12,
                 x->x_gui.x_bcol, x, i, x, x, nlet_tag);
            sys_vgui(".x%lx.c create prect %d %d %d %d -fill #%6.6x "
                     "-stroke #%6.6x -tags {%lxBUT%d %lxHRDO %s text iemgui}\n",
                 canvas, xx21, yy21, xx22, yy22,
                 (x->x_on==i)?x->x_gui.x_fcol:x->x_gui.x_bcol,
                 (x->x_on==i)?x->x_gui.x_fcol:x->x_gui.x_bcol,
                 x, i, x, nlet_tag);
            xx11 += dx;
            xx21 += dx;
            xx22 += dx;
            x->x_drawn = x->x_on;
        }
    iemgui_label_draw_new(&x->x_gui,canvas,xx11b,yy11,nlet_tag,"HRDO");
    hradio_draw_io(x,glist,7);
}

void hradio_draw_move(t_hradio *x, t_glist *glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int n=x->x_number, i, dx=x->x_gui.x_w, s4=dx/4;
    int yy11=text_ypix(&x->x_gui.x_obj, glist), yy12=yy11+dx;
    int yy21=yy11+s4, yy22=yy12-s4;
    int xx11b=text_xpix(&x->x_gui.x_obj, glist), xx11=xx11b, xx21=xx11b+s4;
    int xx22=xx11b+dx-s4;

    if (glist_isvisible(canvas))
    {

        char *nlet_tag = iem_get_tag(glist, (t_iemgui *)x);

        xx11 = xx11b;
        xx21=xx11b+s4;
        xx22=xx11b+dx-s4;
        for(i=0; i<n; i++)
        {
            sys_vgui(".x%lx.c coords %lxBASE%d %d %d %d %d\n",
                     canvas, x, i, xx11, yy11, xx11+dx, yy12);
            sys_vgui(".x%lx.c coords %lxBUT%d %d %d %d %d\n",
                     canvas, x, i, xx21, yy21, xx22, yy22);
            xx11 += dx;
            xx21 += dx;
            xx22 += dx;
        }
        iemgui_label_draw_move(&x->x_gui,canvas,xx11b,yy11);
        if(!iemgui_has_snd(&x->x_gui) && canvas == x->x_gui.x_glist)
            sys_vgui(".x%lx.c coords %lxHRDO%so%d %d %d %d %d\n",
                 canvas, x, nlet_tag, 0, xx11b, yy12-1, xx11b + IOWIDTH, yy12);
        if(!iemgui_has_rcv(&x->x_gui) && canvas == x->x_gui.x_glist)
            sys_vgui(".x%lx.c coords %lxHRDO%si%d %d %d %d %d\n",
                 canvas, x, nlet_tag, 0, xx11b, yy11, xx11b + IOWIDTH, yy11+1);
        /* redraw scale handle rectangle if selected */
        if (x->x_gui.x_selected)
            hradio_draw_select(x, x->x_gui.x_glist);
    }
}

void hradio_draw_config(t_hradio* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int n=x->x_number, i;
    iemgui_label_draw_config(&x->x_gui,canvas);
    for(i=0; i<n; i++)
    {
        sys_vgui(".x%lx.c itemconfigure %lxBASE%d "
                 "-fill #%6.6x\n "
                 ".x%lx.c itemconfigure %lxBUT%d -fill #%6.6x -stroke #%6.6x\n",
             canvas, x, i, x->x_gui.x_bcol, canvas, x, i,
             (x->x_on==i)?x->x_gui.x_fcol:x->x_gui.x_bcol,
             (x->x_on==i)?x->x_gui.x_fcol:x->x_gui.x_bcol);
        /*sys_vgui(".x%lx.c itemconfigure %lxBUT%d -fill #%6.6x "
                   "-outline #%6.6x\n", canvas, x, i,
                 (x->x_on==i)?x->x_gui.x_fcol:x->x_gui.x_bcol,
                 (x->x_on==i)?x->x_gui.x_fcol:x->x_gui.x_bcol);*/
    }
}

void hradio_draw_select(t_hradio* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    if(x->x_gui.x_selected)
    {
        // check if we are drawing inside a gop abstraction
        // visible on parent canvas.  If so, disable highlighting
        if (x->x_gui.x_glist == glist_getcanvas(glist))
        {
            sys_vgui(".x%lx.c itemconfigure %lxBASE "
                "-stroke $pd_colors(selection)\n", canvas, x);
            scalehandle_draw_select2(&x->x_gui,glist,"HRDO",
                x->x_gui.x_w*x->x_number-1,x->x_gui.x_h-1);
        }
    }
    else
    {
        sys_vgui(".x%lx.c itemconfigure %lxBASE -stroke %s\n",
            canvas, x, IEM_GUI_COLOR_NORMAL);
        scalehandle_draw_erase2(&x->x_gui,glist);
    }
    iemgui_label_draw_select(&x->x_gui,canvas);
    iemgui_tag_selected(&x->x_gui,canvas,"HRDO");
}

static void hradio__clickhook(t_scalehandle *sh, t_floatarg f, t_floatarg xxx,
    t_floatarg yyy)
{
    t_hradio *x = (t_hradio *)(sh->h_master);
    int newstate = (int)f;
    if (sh->h_dragon && newstate == 0 && sh->h_scale)
    {
        canvas_apply_setundo(x->x_gui.x_glist, (t_gobj *)x);
        if (sh->h_dragx || sh->h_dragy)
        {
            x->x_gui.x_w += sh->h_dragy;
            x->x_gui.x_h += sh->h_dragy;
            canvas_dirty(x->x_gui.x_glist, 1);
        }
        if (glist_isvisible(x->x_gui.x_glist))
        {
            hradio_draw_move(x, x->x_gui.x_glist);
            scalehandle_unclick_scale(sh);
        }
    }
    else if (!sh->h_dragon && newstate && sh->h_scale)
    {
        scalehandle_click_scale(sh);
    }
    else if (sh->h_dragon && newstate == 0 && !sh->h_scale)
    {
        scalehandle_unclick_label(sh);
    }
    else if (!sh->h_dragon && newstate && !sh->h_scale)
    {
        scalehandle_click_label(sh);
    }
    sh->h_dragon = newstate;
}

static void hradio__motionhook(t_scalehandle *sh, t_floatarg f1, t_floatarg f2)
{
    if (sh->h_dragon && sh->h_scale)
    {
        t_hradio *x = (t_hradio *)(sh->h_master);
        int dx = (int)f1, dy = (int)f2;
        dx = maxi(dx,(IEM_GUI_MINSIZE-x->x_gui.x_w)*x->x_number);
        dy = dx/x->x_number;
        sh->h_dragx = dx;
        sh->h_dragy = dy;
        scalehandle_drag_scale(sh);

        int properties = gfxstub_haveproperties((void *)x);
        if (properties)
        {
            int new_w = x->x_gui.x_w + sh->h_dragx;
            properties_set_field_int(properties,"dim.w_ent",new_w);
        }
    }
    scalehandle_dragon_label(sh,f1,f2);
}

void hradio_draw(t_hradio *x, t_glist *glist, int mode)
{
    if(mode == IEM_GUI_DRAW_MODE_UPDATE)
        sys_queuegui(x, glist, hradio_draw_update);
    else if(mode == IEM_GUI_DRAW_MODE_MOVE)
        hradio_draw_move(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_NEW)
    {
        hradio_draw_new(x, glist);
        sys_vgui(".x%lx.c raise all_cords\n", glist_getcanvas(glist));
    }
    else if(mode == IEM_GUI_DRAW_MODE_SELECT)
        hradio_draw_select(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_ERASE)
        iemgui_draw_erase(&x->x_gui, glist, "HRDO");
    else if(mode == IEM_GUI_DRAW_MODE_CONFIG)
        hradio_draw_config(x, glist);
    else if(mode >= IEM_GUI_DRAW_MODE_IO)
        hradio_draw_io(x, glist, mode - IEM_GUI_DRAW_MODE_IO);
}

/* ------------------------ hdl widgetbehaviour----------------------------- */

static void hradio_getrect(t_gobj *z, t_glist *glist, int *xp1, int *yp1,
    int *xp2, int *yp2)
{
    t_hradio *x = (t_hradio *)z;

    *xp1 = text_xpix(&x->x_gui.x_obj, glist);
    *yp1 = text_ypix(&x->x_gui.x_obj, glist);
    *xp2 = *xp1 + x->x_gui.x_w*x->x_number;
    *yp2 = *yp1 + x->x_gui.x_h;

    iemgui_label_getrect(x->x_gui, glist, xp1, yp1, xp2, yp2);
}

static void hradio_save(t_gobj *z, t_binbuf *b)
{
    t_hradio *x = (t_hradio *)z;
    int bflcol[3];
    t_symbol *srl[3];

    iemgui_save(&x->x_gui, srl, bflcol);
    binbuf_addv(b, "ssiisiiiisssiiiiiiii", gensym("#X"),gensym("obj"),
                (int)x->x_gui.x_obj.te_xpix, (int)x->x_gui.x_obj.te_ypix,
                (pd_class(&x->x_gui.x_obj.ob_pd) == hradio_old_class ?
                    gensym("hdl") : gensym("hradio")),
                x->x_gui.x_w,
                x->x_change, iem_symargstoint(&x->x_gui), x->x_number,
                srl[0], srl[1], srl[2],
                x->x_gui.x_ldx, x->x_gui.x_ldy,
                iem_fstyletoint(&x->x_gui), x->x_gui.x_fontsize,
                bflcol[0], bflcol[1], bflcol[2], x->x_on);
    binbuf_addv(b, ";");
}

static void hradio_properties(t_gobj *z, t_glist *owner)
{
    t_hradio *x = (t_hradio *)z;
    char buf[800];
    t_symbol *srl[3];
    int hchange=-1;

    iemgui_properties(&x->x_gui, srl);
    if (pd_class(&x->x_gui.x_obj.ob_pd) == hradio_old_class)
        hchange = x->x_change;
    sprintf(buf, "pdtk_iemgui_dialog %%s |hradio| \
            ----------dimensions(pix):----------- %d %d size: 0 0 empty \
            empty 0.0 empty 0.0 empty %d \
            %d new-only new&old %d %d number: %d \
            {%s} {%s} \
            {%s} %d %d \
            %d %d \
            %d %d %d\n",
            x->x_gui.x_w, IEM_GUI_MINSIZE,
            0,/*no_schedule*/
            hchange, x->x_gui.x_loadinit, -1, x->x_number,
            srl[0]->s_name, srl[1]->s_name,
            srl[2]->s_name, x->x_gui.x_ldx, x->x_gui.x_ldy,
            x->x_gui.x_font_style, x->x_gui.x_fontsize,
            0xffffff & x->x_gui.x_bcol, 0xffffff & x->x_gui.x_fcol,
            0xffffff & x->x_gui.x_lcol);
    gfxstub_new(&x->x_gui.x_obj.ob_pd, x, buf);
}

static void hradio_dialog(t_hradio *x, t_symbol *s, int argc, t_atom *argv)
{
    canvas_apply_setundo(x->x_gui.x_glist, (t_gobj *)x);

    t_symbol *srl[3];
    int a = (int)atom_getintarg(0, argc, argv);
    int chg = (int)atom_getintarg(4, argc, argv);
    int num = (int)atom_getintarg(6, argc, argv);
    int sr_flags;

    if(chg != 0) chg = 1;
    x->x_change = chg;
    sr_flags = iemgui_dialog(&x->x_gui, srl, argc, argv);
    x->x_gui.x_w = iemgui_clip_size(a);
    x->x_gui.x_h = x->x_gui.x_w;
    if(x->x_number != num)
    {
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
        x->x_number = num;
        if(x->x_on >= x->x_number)
        {
            x->x_on = x->x_number - 1;
            x->x_on_old = x->x_on;
        }
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
    }
    else
    {
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_CONFIG);
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_IO + sr_flags);
        //(*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_MOVE);
        //canvas_fixlinesfor(glist_getcanvas(x->x_gui.x_glist), (t_text*)x);
        iemgui_shouldvis(&x->x_gui, IEM_GUI_DRAW_MODE_MOVE);
    }

    /* forcing redraw of the scale handle */
    if (x->x_gui.x_selected)
    {
        hradio_draw_select(x, x->x_gui.x_glist);
    }

    // ico@bukvic.net 100518
    // update scrollbars when object potentially exceeds window size
    t_canvas *canvas=(t_canvas *)glist_getcanvas(x->x_gui.x_glist);
    sys_vgui("pdtk_canvas_getscroll .x%lx.c\n", (long unsigned int)canvas);
}

static void hradio_set(t_hradio *x, t_floatarg f)
{
    int i=(int)f;
    int old=x->x_on_old;

    if(i < 0)
        i = 0;
    if(i >= x->x_number)
        i = x->x_number-1;
    if(x->x_on != x->x_on_old)
    {
        old = x->x_on_old;
        x->x_on_old = x->x_on;
        x->x_on = i;
        if (x->x_on != x->x_on_old)
            (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
        x->x_on_old = old;
    }
    else
    {
        x->x_on = i;
        if (x->x_on != x->x_on_old)
            (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
    }
}

static void hradio_bang(t_hradio *x)
{
        /* compatibility with earlier  "hdial" behavior */
    if (pd_class(&x->x_gui.x_obj.ob_pd) == hradio_old_class)
    {
        if((x->x_change)&&(x->x_on != x->x_on_old))
        {
            SETFLOAT(x->x_at, (t_float)x->x_on_old);
            SETFLOAT(x->x_at+1, 0.0);
            outlet_list(x->x_gui.x_obj.ob_outlet, &s_list, 2, x->x_at);
            if(iemgui_has_snd(&x->x_gui) && x->x_gui.x_snd->s_thing)
                pd_list(x->x_gui.x_snd->s_thing, &s_list, 2, x->x_at);
        }
        x->x_on_old = x->x_on;
        SETFLOAT(x->x_at, (t_float)x->x_on);
        SETFLOAT(x->x_at+1, 1.0);
        outlet_list(x->x_gui.x_obj.ob_outlet, &s_list, 2, x->x_at);
        if(iemgui_has_snd(&x->x_gui) && x->x_gui.x_snd->s_thing)
            pd_list(x->x_gui.x_snd->s_thing, &s_list, 2, x->x_at);
    }
    else
    {
        outlet_float(x->x_gui.x_obj.ob_outlet, x->x_on);
        if(iemgui_has_snd(&x->x_gui) && x->x_gui.x_snd->s_thing)
            pd_float(x->x_gui.x_snd->s_thing, x->x_on);
    }
}

static void hradio_fout(t_hradio *x, t_floatarg f)
{
    int i=(int)f;

    if(i < 0)
        i = 0;
    if(i >= x->x_number)
        i = x->x_number-1;

    if (pd_class(&x->x_gui.x_obj.ob_pd) == hradio_old_class)
    {
        if((x->x_change)&&(i != x->x_on_old))
        {
            SETFLOAT(x->x_at, (t_float)x->x_on_old);
            SETFLOAT(x->x_at+1, 0.0);
            outlet_list(x->x_gui.x_obj.ob_outlet, &s_list, 2, x->x_at);
            if(iemgui_has_snd(&x->x_gui) && x->x_gui.x_snd->s_thing)
                pd_list(x->x_gui.x_snd->s_thing, &s_list, 2, x->x_at);
        }
        if(x->x_on != x->x_on_old)
            x->x_on_old = x->x_on;
        x->x_on = i;
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
        x->x_on_old = x->x_on;
        SETFLOAT(x->x_at, (t_float)x->x_on);
        SETFLOAT(x->x_at+1, 1.0);
        outlet_list(x->x_gui.x_obj.ob_outlet, &s_list, 2, x->x_at);
        if(iemgui_has_snd(&x->x_gui) && x->x_gui.x_snd->s_thing)
            pd_list(x->x_gui.x_snd->s_thing, &s_list, 2, x->x_at);
    }
    else
    {
        x->x_on_old = x->x_on;
        x->x_on = i;
        if (i != x->x_on_old)
            (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
        outlet_float(x->x_gui.x_obj.ob_outlet, x->x_on);
        if(iemgui_has_snd(&x->x_gui) && x->x_gui.x_snd->s_thing)
            pd_float(x->x_gui.x_snd->s_thing, x->x_on);
    }
}

static void hradio_float(t_hradio *x, t_floatarg f)
{
    int i=(int)f;

    if(i < 0)
        i = 0;
    if(i >= x->x_number)
        i = x->x_number-1;

    if (pd_class(&x->x_gui.x_obj.ob_pd) == hradio_old_class)
    {
        /* compatibility with earlier  "vdial" behavior */
        if((x->x_change)&&(i != x->x_on_old))
        {
            if(x->x_gui.x_put_in2out)
            {
                SETFLOAT(x->x_at, (t_float)x->x_on_old);
                SETFLOAT(x->x_at+1, 0.0);
                outlet_list(x->x_gui.x_obj.ob_outlet, &s_list, 2, x->x_at);
                if(iemgui_has_snd(&x->x_gui) && x->x_gui.x_snd->s_thing)
                    pd_list(x->x_gui.x_snd->s_thing, &s_list, 2, x->x_at);
            }
        }
        if(x->x_on != x->x_on_old)
            x->x_on_old = x->x_on;
        x->x_on = i;
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
        x->x_on_old = x->x_on;
        if(x->x_gui.x_put_in2out)
        {
            SETFLOAT(x->x_at, (t_float)x->x_on);
            SETFLOAT(x->x_at+1, 1.0);
            outlet_list(x->x_gui.x_obj.ob_outlet, &s_list, 2, x->x_at);
            if(iemgui_has_snd(&x->x_gui) && x->x_gui.x_snd->s_thing)
                pd_list(x->x_gui.x_snd->s_thing, &s_list, 2, x->x_at);
        }
    }
    else
    {
        x->x_on_old = x->x_on;
        x->x_on = i;
        if (i != x->x_on_old)
            (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
        if (x->x_gui.x_put_in2out)
        {
            outlet_float(x->x_gui.x_obj.ob_outlet, x->x_on);
            if(iemgui_has_snd(&x->x_gui) && x->x_gui.x_snd->s_thing)
                pd_float(x->x_gui.x_snd->s_thing, x->x_on);
        }
    }
}

static void hradio_click(t_hradio *x, t_floatarg xpos, t_floatarg ypos,
    t_floatarg shift, t_floatarg ctrl, t_floatarg alt)
{
    int xx = (int)xpos - (int)text_xpix(&x->x_gui.x_obj, x->x_gui.x_glist);

    hradio_fout(x, (t_float)(xx / x->x_gui.x_w));
}

static int hradio_newclick(t_gobj *z, struct _glist *glist, int xpix, int ypix,
    int shift, int alt, int dbl, int doit)
{
    if(doit)
    {
        hradio_click((t_hradio *)z, (t_floatarg)xpix, (t_floatarg)ypix,
            (t_floatarg)shift, 0, (t_floatarg)alt);
    }
    return (1);
}

static void hradio_loadbang(t_hradio *x)
{
    if(!sys_noloadbang && x->x_gui.x_loadinit)
        hradio_bang(x);
}

static void hradio_number(t_hradio *x, t_floatarg num)
{
    int n=(int)num;

    if(n < 1)
        n = 1;
    if(n > IEM_RADIO_MAX)
        n = IEM_RADIO_MAX;
    if(n != x->x_number)
    {
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
        x->x_number = n;
        if(x->x_on >= x->x_number)
            x->x_on = x->x_number - 1;
        x->x_on_old = x->x_on;
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
    }
}

static void hradio_size(t_hradio *x, t_symbol *s, int ac, t_atom *av)
{
    x->x_gui.x_w = iemgui_clip_size((int)atom_getintarg(0, ac, av));
    x->x_gui.x_h = x->x_gui.x_w;
    iemgui_size(&x->x_gui);
}

static void hradio_init(t_hradio *x, t_floatarg f)
{
    x->x_gui.x_loadinit = (f==0.0)?0:1;
}

static void hradio_double_change(t_hradio *x)
{x->x_change = 1;}

static void hradio_single_change(t_hradio *x)
{x->x_change = 0;}

static void *hradio_donew(t_symbol *s, int argc, t_atom *argv, int old)
{
    t_hradio *x = (t_hradio *)pd_new(old? hradio_old_class : hradio_class);
    int bflcol[]={-262144, -1, -1};
    int a=IEM_GUI_DEFAULTSIZE, on=0;
    int ldx=0, ldy=-8, chg=1, num=8;
    int fs=10;

    iem_inttosymargs(&x->x_gui, 0);
    iem_inttofstyle(&x->x_gui, 0);

    if((argc == 15)&&IS_A_FLOAT(argv,0)&&IS_A_FLOAT(argv,1)&&IS_A_FLOAT(argv,2)
       &&IS_A_FLOAT(argv,3)
       &&(IS_A_SYMBOL(argv,4)||IS_A_FLOAT(argv,4))
       &&(IS_A_SYMBOL(argv,5)||IS_A_FLOAT(argv,5))
       &&(IS_A_SYMBOL(argv,6)||IS_A_FLOAT(argv,6))
       &&IS_A_FLOAT(argv,7)&&IS_A_FLOAT(argv,8)
       &&IS_A_FLOAT(argv,9)&&IS_A_FLOAT(argv,10)&&IS_A_FLOAT(argv,11)
       &&IS_A_FLOAT(argv,12)&&IS_A_FLOAT(argv,13)&&IS_A_FLOAT(argv,14))
    {
        a = (int)atom_getintarg(0, argc, argv);
        chg = (int)atom_getintarg(1, argc, argv);
        iem_inttosymargs(&x->x_gui, atom_getintarg(2, argc, argv));
        num = (int)atom_getintarg(3, argc, argv);
        iemgui_new_getnames(&x->x_gui, 4, argv);
        ldx = (int)atom_getintarg(7, argc, argv);
        ldy = (int)atom_getintarg(8, argc, argv);
        iem_inttofstyle(&x->x_gui, atom_getintarg(9, argc, argv));
        fs = (int)atom_getintarg(10, argc, argv);
        bflcol[0] = (int)atom_getintarg(11, argc, argv);
        bflcol[1] = (int)atom_getintarg(12, argc, argv);
        bflcol[2] = (int)atom_getintarg(13, argc, argv);
        on = (int)atom_getintarg(14, argc, argv);
    }
    else iemgui_new_getnames(&x->x_gui, 4, 0);
    x->x_gui.x_draw = (t_iemfunptr)hradio_draw;
    x->x_gui.x_glist = (t_glist *)canvas_getcurrent();
    if (x->x_gui.x_font_style<0 || x->x_gui.x_font_style>2) x->x_gui.x_font_style=0;
    if(num < 1)
        num = 1;
    if(num > IEM_RADIO_MAX)
        num = IEM_RADIO_MAX;
    x->x_number = num;
    if(on < 0)
        on = 0;
    if(on >= x->x_number)
        on = x->x_number - 1;
    if(x->x_gui.x_loadinit)
        x->x_on = on;
    else
        x->x_on = 0;
    x->x_on_old = x->x_on;
    x->x_change = (chg==0)?0:1;
    if (iemgui_has_rcv(&x->x_gui))
        pd_bind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
    x->x_gui.x_ldx = ldx;
    x->x_gui.x_ldy = ldy;
    if(fs < 4)
        fs = 4;
    x->x_gui.x_fontsize = fs;
    x->x_gui.x_w = iemgui_clip_size(a);
    x->x_gui.x_h = x->x_gui.x_w;
    iemgui_verify_snd_ne_rcv(&x->x_gui);
    iemgui_all_colfromload(&x->x_gui, bflcol);
    outlet_new(&x->x_gui.x_obj, &s_list);

    x->x_gui. x_handle = scalehandle_new(scalehandle_class,(t_iemgui *)x,1);
    x->x_gui.x_lhandle = scalehandle_new(scalehandle_class,(t_iemgui *)x,0);
    x->x_gui.x_obj.te_iemgui = 1;

    return (x);
}

static void *hradio_new(t_symbol *s, int argc, t_atom *argv)
{
    return (hradio_donew(s, argc, argv, 0));
}

static void *hdial_new(t_symbol *s, int argc, t_atom *argv)
{
    return (hradio_donew(s, argc, argv, 1));
}

static void hradio_ff(t_hradio *x)
{
    if(iemgui_has_rcv(&x->x_gui))
        pd_unbind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
    gfxstub_deleteforkey(x);

    if (x->x_gui. x_handle) scalehandle_free(x->x_gui. x_handle);
    if (x->x_gui.x_lhandle) scalehandle_free(x->x_gui.x_lhandle);
}

void g_hradio_setup(void)
{
    hradio_class = class_new(gensym("hradio"), (t_newmethod)hradio_new,
        (t_method)hradio_ff, sizeof(t_hradio), 0, A_GIMME, 0);
    class_addbang(hradio_class, hradio_bang);
    class_addfloat(hradio_class, hradio_float);
    class_addmethod(hradio_class, (t_method)hradio_click, gensym("click"),
                    A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(hradio_class, (t_method)hradio_dialog, gensym("dialog"),
                    A_GIMME, 0);
    class_addmethod(hradio_class, (t_method)hradio_loadbang,
        gensym("loadbang"), 0);
    class_addmethod(hradio_class, (t_method)hradio_set,
        gensym("set"), A_FLOAT, 0);
    class_addmethod(hradio_class, (t_method)hradio_size,
        gensym("size"), A_GIMME, 0);
    iemgui_class_addmethods(hradio_class);
    class_addmethod(hradio_class, (t_method)hradio_init,
        gensym("init"), A_FLOAT, 0);
    class_addmethod(hradio_class, (t_method)hradio_number,
        gensym("number"), A_FLOAT, 0);
    class_addmethod(hradio_class, (t_method)hradio_single_change,
        gensym("single_change"), 0);
    class_addmethod(hradio_class, (t_method)hradio_double_change,
        gensym("double_change"), 0);
 
    scalehandle_class = class_new(gensym("_scalehandle"), 0, 0,
                  sizeof(t_scalehandle), CLASS_PD, 0);
    class_addmethod(scalehandle_class, (t_method)hradio__clickhook,
            gensym("_click"), A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(scalehandle_class, (t_method)hradio__motionhook,
            gensym("_motion"), A_FLOAT, A_FLOAT, 0);

    wb_init(&hradio_widgetbehavior,hradio_getrect,hradio_newclick);
    class_setwidget(hradio_class, &hradio_widgetbehavior);
    class_sethelpsymbol(hradio_class, gensym("hradio"));
    class_setsavefn(hradio_class, hradio_save);
    class_setpropertiesfn(hradio_class, hradio_properties);

        /*obsolete version (0.34-0.35) */
    hradio_old_class = class_new(gensym("hdl"), (t_newmethod)hdial_new,
        (t_method)hradio_ff, sizeof(t_hradio), 0, A_GIMME, 0);
    class_addcreator((t_newmethod)hradio_new, gensym("rdb"), A_GIMME, 0);
    class_addcreator((t_newmethod)hradio_new, gensym("radiobut"), A_GIMME, 0);
    class_addcreator((t_newmethod)hradio_new, gensym("radiobutton"),
        A_GIMME, 0);
    class_addbang(hradio_old_class, hradio_bang);
    class_addfloat(hradio_old_class, hradio_float);
    class_addmethod(hradio_old_class, (t_method)hradio_click, gensym("click"),
                    A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(hradio_old_class, (t_method)hradio_dialog, gensym("dialog"),
                    A_GIMME, 0);
    class_addmethod(hradio_old_class, (t_method)hradio_loadbang,
        gensym("loadbang"), 0);
    class_addmethod(hradio_old_class, (t_method)hradio_set,
        gensym("set"), A_FLOAT, 0);
    class_addmethod(hradio_old_class, (t_method)hradio_size,
        gensym("size"), A_GIMME, 0);
    iemgui_class_addmethods(hradio_old_class);
    class_addmethod(hradio_old_class, (t_method)hradio_init,
        gensym("init"), A_FLOAT, 0);
    class_addmethod(hradio_old_class, (t_method)hradio_number,
        gensym("number"), A_FLOAT, 0);
    class_addmethod(hradio_old_class, (t_method)hradio_single_change,
        gensym("single_change"), 0);
    class_addmethod(hradio_old_class, (t_method)hradio_double_change,
        gensym("double_change"), 0);
    class_setwidget(hradio_old_class, &hradio_widgetbehavior);
    class_sethelpsymbol(hradio_old_class, gensym("hradio"));
}

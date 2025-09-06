/*
  COPYRIGHT (C) 2025  Stepan Pressl (pressl.stepan@gmail.com)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

/* This file defines all the function needed to be defined by each platform.
 *
 * The functions are divided into categories, to allow the launching
 * of multiple models and for the generated code to be as generic
 * as possible.
 * 
 * The function prototypes using the NAME macro apply to one model
 * instance, only. The implementation of the "NAME" functions is provided
 * BY THE GENERATED CODE.
 *
 * The functions prototypes NOT using the NAME macro apply are DEFINED
 * by the platform and but are used in the generated code.
 * They are not model dependant.
 *
 * REVISIT: devise a proper FSM that encapsulates model's runtime and
 *          its state.
 */

#ifndef _PLATFORM_H
#define _PLATFORM_H

#ifdef CONF_SHV_USED
#include <shv/tree/shv_com.h>
#include <shv/tree/shv_tree.h>
#ifdef CONF_SHV_UPDATES_USED
#include <shv/tree/shv_file_node.h>
#include <shv/tree/shv_clayer_posix.h>
#include <shv_fwstable_node.h>
#endif
#endif

/* The concat macro that makes everything work */
#define XNAME(x,y)  x##y
#define NAME(x,y)   XNAME(x,y)

/* Forward declaration of platform dependant model context */
struct pysim_platform_model_ctx;

/* Model dependant functions */
int NAME(MODEL, _init)(void);          /* init the model */
int NAME(MODEL, _isr)(double);         /* run the control computation */
int NAME(MODEL, _end)(void);           /* deinit the model */
double NAME(MODEL, _get_tsamp)(void);  /* get model's sampling period */

double NAME(MODEL, _runtime)(struct pysim_platform_model_ctx *ctx); /* get model's runtime */

/* Pauses the execution of the model - stops the loop and deinits the model.
 * But does not close the connection.
 *
 * If communication is established, this function is mapped to the SHV manager node.
 */
void NAME(MODEL, _pausectrl)(struct pysim_platform_model_ctx *ctx);

/* Resumes the exectuion of the model - inits the model again and starts the loop.
 * Does not initialize communication, as it's already ongoing.
 *
 * If communication is established, this function is mapped to the SHV manager node.
 */
void NAME(MODEL, _resumectrl)(struct pysim_platform_model_ctx *ctx);

/* Gets model's current state, the states are defined in pyblock.h
 *
 * If communication is established, this function is mapped to the SHV manager node.
 */
int NAME(MODEL, _getctrlstate)(struct pysim_platform_model_ctx *ctx);

#ifdef CONF_SHV_USED
int NAME(MODEL, _comprio)(struct pysim_platform_model_ctx *ctx); /* prio for the com thread */
int NAME(MODEL, _com_init)(shv_attention_signaller at_signlr);   /* init the com for the model */
int NAME(MODEL, _com_end)(void);                                 /* deinit the com for the model */

#ifdef CONF_SHV_UPDATES_USED
int shv_init_fwupdate(struct pysim_platform_model_ctx *ctx,
                      struct shv_file_node *file_node);         /* init the fwupdate node */
int shv_init_fwstable(struct pysim_platform_model_ctx *ctx,
                      struct shv_fwstable_node *fwstable_node); /* init the fwstable node */
#endif /* CONF_SHV_UPDATES_USED */
#endif /* CONF_SHV_USED */

#endif /* _PLATFORM_H */

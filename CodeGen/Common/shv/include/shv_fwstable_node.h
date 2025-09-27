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

#ifndef _SHV_FWSTABLE_NODE_H
#define _SHV_FWSTABLE_NODE_H

#include <shv/tree/shv_tree.h>

struct shv_fwstable_node
{
    struct shv_node shv_node; /* Base node */
    struct {
        int (*confirm)(void); /* A callback to confirm the newly updated image */
        int (*get)(void);     /* A callback to get image's current state (confirmed/unconfirmed) */
    } ops;
};

extern const struct shv_dmap shv_fwstable_dmap;

struct shv_fwstable_node *shv_fwstable_node_new(const struct shv_dmap *dir, int mode);

#endif /* _SHV_FWSTABLE_NODE_H */

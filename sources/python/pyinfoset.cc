//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Python extension type for infoset objects
//
// This file is part of Gambit
// Copyright (c) 2003, The Gambit Project
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include <Python.h>
#include "pygambit.h"
#include "game/efg.h"

// This file provides implementations for both action and information set,
// since they're closely related

/*************************************************************************
 * ACTION: TYPE DESCRIPTOR
 *************************************************************************/

staticforward void action_dealloc(actionobject *);
staticforward PyObject *action_getattr(actionobject *, char *);
staticforward int action_compare(actionobject *, actionobject *); 
staticforward PyObject *action_str(actionobject *);

PyTypeObject Actiontype = {      /* main python type-descriptor */
  /* type header */                    /* shared by all instances */
  PyObject_HEAD_INIT(0)
  0,                               /* ob_size */
  "action",                           /* tp_name */
  sizeof(actionobject),               /* tp_basicsize */
  0,                               /* tp_itemsize */

  /* standard methods */
  (destructor)  action_dealloc,       /* tp_dealloc  ref-count==0  */
  (printfunc)   0,         /* tp_print    "print x"     */
  (getattrfunc) action_getattr,       /* tp_getattr  "x.attr"      */
  (setattrfunc) 0,                 /* tp_setattr  "x.attr=v"    */
  (cmpfunc)     action_compare,      /* tp_compare  "x > y"       */
  (reprfunc)    0,                 /* tp_repr     `x`, print x  */

  /* type categories */
  0,                               /* tp_as_number   +,-,*,/,%,&,>>,pow...*/
  0,                               /* tp_as_sequence +,[i],[i:j],len, ...*/
  0,                               /* tp_as_mapping  [key], len, ...*/

  /* more methods */
  (hashfunc)     0,                /* tp_hash    "dict[x]" */
  (ternaryfunc)  0,                /* tp_call    "x()"     */
  (reprfunc)     action_str,                /* tp_str     "str(x)"  */
};  /* plus others: see Include/object.h */


/*****************************************************************************
 * INSTANCE METHODS
 *****************************************************************************/

static PyObject *
action_deleteaction(actionobject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  
  self->m_action->DeleteAction();
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
action_getchanceprob(actionobject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  if (!self->m_action->GetInfoset().IsChanceInfoset()) {
    return NULL;
  }

  return Py_BuildValue("d", (double) self->m_action->GetChanceProb());
}

static PyObject *
action_getinfoset(actionobject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  infosetobject *infoset = newinfosetobject();
  infoset->m_infoset = new gbtEfgInfoset(self->m_action->GetInfoset());
  return (PyObject *) infoset;
}

static PyObject *
action_getlabel(actionobject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  return Py_BuildValue("s", (char *) self->m_action->GetLabel());
}

static PyObject *
action_setchanceprob(actionobject *self, PyObject *args)
{
  double prob;

  if (!PyArg_ParseTuple(args, "d", &prob)) {
    return NULL;
  }

  if (!self->m_action->GetInfoset().IsChanceInfoset()) {
    return NULL;
  }

  self->m_action->GetInfoset().SetChanceProb(self->m_action->GetId(), prob);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
action_setlabel(actionobject *self, PyObject *args)
{
  char *label;

  if (!PyArg_ParseTuple(args, "s", &label)) {
    return NULL;
  }

  self->m_action->SetLabel(label);
  Py_INCREF(Py_None);
  return Py_None;
}

static struct PyMethodDef action_methods[] = {
  { "DeleteAction", (PyCFunction) action_deleteaction, 1 },
  { "GetChanceProb", (PyCFunction) action_getchanceprob, 1 },
  { "GetInfoset", (PyCFunction) action_getinfoset, 1 },
  { "GetLabel", (PyCFunction) action_getlabel, 1 },
  { "SetLabel", (PyCFunction) action_setlabel, 1 },
  { "SetChanceProb", (PyCFunction) action_setchanceprob, 1 },
  { NULL, NULL }
};

/*****************************************************************************
 * BASIC TYPE-OPERATIONS
 *****************************************************************************/

actionobject *
newactionobject(void)
{
  actionobject *self;
  self = PyObject_NEW(actionobject, &Actiontype);
  if (self == NULL) {
    return NULL;
  }
  self->m_action = new gbtEfgAction();
  return self;
}

static void                   
action_dealloc(actionobject *self) 
{                            
  PyMem_DEL(self);           
}

static PyObject *
action_getattr(actionobject *self, char *name)
{
  return Py_FindMethod(action_methods, (PyObject *) self, name);
}

static int
action_compare(actionobject *obj1, actionobject *obj2)
{
  // Implementation: If outcomes are the game underlying object, return
  // equal; otherwise, order by their Python pointer addresses.
  if (*obj1->m_action == *obj2->m_action) {
    return 0;
  }
  else if (obj1->m_action < obj2->m_action) {
    return -1;
  }
  else {
    return 1;
  }
}

static PyObject *
action_str(actionobject *self)
{
  return PyString_FromFormat("<{action} \"%s\">",
			     (char *) self->m_action->GetLabel());
}

/*************************************************************************
 * INFOSET: TYPE DESCRIPTOR
 *************************************************************************/

staticforward void infoset_dealloc(infosetobject *);
staticforward PyObject *infoset_getattr(infosetobject *, char *);
staticforward int infoset_compare(infosetobject *, infosetobject *); 
staticforward PyObject *infoset_str(infosetobject *);

PyTypeObject Infosettype = {      /* main python type-descriptor */
  /* type header */                    /* shared by all instances */
  PyObject_HEAD_INIT(0)
  0,                               /* ob_size */
  "infoset",                           /* tp_name */
  sizeof(infosetobject),               /* tp_basicsize */
  0,                               /* tp_itemsize */

  /* standard methods */
  (destructor)  infoset_dealloc,       /* tp_dealloc  ref-count==0  */
  (printfunc)   0,         /* tp_print    "print x"     */
  (getattrfunc) infoset_getattr,       /* tp_getattr  "x.attr"      */
  (setattrfunc) 0,                 /* tp_setattr  "x.attr=v"    */
  (cmpfunc)     infoset_compare,      /* tp_compare  "x > y"       */
  (reprfunc)    0,                 /* tp_repr     `x`, print x  */

  /* type categories */
  0,                               /* tp_as_number   +,-,*,/,%,&,>>,pow...*/
  0,                               /* tp_as_sequence +,[i],[i:j],len, ...*/
  0,                               /* tp_as_mapping  [key], len, ...*/

  /* more methods */
  (hashfunc)     0,                /* tp_hash    "dict[x]" */
  (ternaryfunc)  0,                /* tp_call    "x()"     */
  (reprfunc)     infoset_str,                /* tp_str     "str(x)"  */
};  /* plus others: see Include/object.h */


/*****************************************************************************
 * INSTANCE METHODS
 *****************************************************************************/

static PyObject *
infoset_deleteinfoset(infosetobject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  if (self->m_infoset->NumMembers() > 0) {
    return NULL;
  }

  self->m_infoset->DeleteInfoset();
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
infoset_getaction(infosetobject *self, PyObject *args)
{
  int index;

  if (!PyArg_ParseTuple(args, "i", &index)) {
    return NULL;
  }

  if (index < 1 || index > self->m_infoset->NumActions()) {
    return NULL;
  }
  
  actionobject *action = newactionobject();
  *action->m_action = self->m_infoset->GetAction(index);
  return (PyObject *) action;
}

static PyObject *
infoset_getlabel(infosetobject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  return Py_BuildValue("s", (char *) self->m_infoset->GetLabel());
}

static PyObject *
infoset_getmember(infosetobject *self, PyObject *args)
{
  int index;

  if (!PyArg_ParseTuple(args, "i", &index)) {
    return NULL;
  }

  if (index < 1 || index > self->m_infoset->NumMembers()) {
    return NULL;
  }
  
  nodeobject *node = newnodeobject();
  *node->m_node = self->m_infoset->GetMember(index);
  return (PyObject *) node;
}

static PyObject *
infoset_getplayer(infosetobject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  efplayerobject *player = newefplayerobject();
  player->m_efplayer = new gbtEfgPlayer(self->m_infoset->GetPlayer());
  return (PyObject *) player;
}

static PyObject *
infoset_reveal(infosetobject *self, PyObject *args)
{
  PyObject *player;

  if (!PyArg_ParseTuple(args, "O", &player)) {
    return NULL;
  }

  if (!is_efplayerobject(player)) {
    return NULL;
  }

  self->m_infoset->Reveal(*((efplayerobject *) player)->m_efplayer);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
infoset_setlabel(infosetobject *self, PyObject *args)
{
  char *label;

  if (!PyArg_ParseTuple(args, "s", &label)) {
    return NULL;
  }

  self->m_infoset->SetLabel(label);
  Py_INCREF(Py_None);
  return Py_None;
}

static struct PyMethodDef infoset_methods[] = {
  { "GetAction", (PyCFunction) infoset_getaction, 1 },
  { "GetLabel", (PyCFunction) infoset_getlabel, 1 },
  { "GetMember", (PyCFunction) infoset_getmember, 1 },
  { "GetPlayer", (PyCFunction) infoset_getplayer, 1 }, 
  { "Reveal", (PyCFunction) infoset_reveal, 1 }, 
  { "SetLabel", (PyCFunction) infoset_setlabel, 1 },
  { NULL, NULL }
};

/*****************************************************************************
 * BASIC TYPE-OPERATIONS
 *****************************************************************************/

infosetobject *
newinfosetobject(void)
{
  infosetobject *self;
  self = PyObject_NEW(infosetobject, &Infosettype);
  if (self == NULL) {
    return NULL;
  }
  self->m_infoset = new gbtEfgInfoset();
  return self;
}

static void                   
infoset_dealloc(infosetobject *self) 
{                            
  PyMem_DEL(self);           
}

static PyObject *
infoset_getattr(infosetobject *self, char *name)
{
  return Py_FindMethod(infoset_methods, (PyObject *) self, name);
}

static int
infoset_compare(infosetobject *obj1, infosetobject *obj2)
{
  // Implementation: If outcomes are the game underlying object, return
  // equal; otherwise, order by their Python pointer addresses.
  if (*obj1->m_infoset == *obj2->m_infoset) {
    return 0;
  }
  else if (obj1->m_infoset < obj2->m_infoset) {
    return -1;
  }
  else {
    return 1;
  }
}

static PyObject *
infoset_str(infosetobject *self)
{
  return PyString_FromFormat("<{infoset} \"%s\">",
			     (char *) self->m_infoset->GetLabel());
}

/************************************************************************
 * MODULE METHODS
 ************************************************************************/

void
initinfoset(void)
{
  Actiontype.ob_type = &PyType_Type;
  Infosettype.ob_type = &PyType_Type;
}

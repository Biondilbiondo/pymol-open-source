
/* 
A* -------------------------------------------------------------------
B* This file contains source code for the PyMOL computer program
C* copyright 1998-2000 by Warren Lyford Delano of DeLano Scientific. 
D* -------------------------------------------------------------------
E* It is unlawful to modify or remove this copyright notice.
F* -------------------------------------------------------------------
G* Please see the accompanying LICENSE file for further information. 
H* -------------------------------------------------------------------
I* Additional authors of this source file include:
-* 
-* 
-*
Z* -------------------------------------------------------------------
*/
#ifndef _H_ObjectSlice
#define _H_ObjectSlice

#include"ObjectMap.h"

typedef struct {
  PyMOLGlobals *G;
  /* stored in a session */

  int Active;
  ObjectNameType MapName;
  int MapState;
  float MapMean;
  float MapStdev;

  float ExtentMin[3];
  float ExtentMax[3];
  int ExtentFlag;

  float origin[3];              /* the origin of the plane */
  float system[9];              /* x, y, and z of the system */

  /* not stored in session */

  int RefreshFlag;
  int min[2], max[2];           /* extents of the arrays */
  float last_scale;

  /* the data is normalized for easier ploting */
  int n_points;

  float *values;
  float *points;
  int *flags;
  float *colors;
  float *normals;

  int n_strips;
  int *strips;

  CGO *shaderCGO;
  float Corner[24];

  float outline_points[36];
  int outline_n_points;
  float outline_zaxis[3];
} ObjectSliceState;

struct ObjectSlice : public CObject {
  ObjectSliceState *State;
  PickContext context{};
  int NState = 0;
  ObjectSlice(PyMOLGlobals* G);
  ~ObjectSlice();
};

ObjectSlice *ObjectSliceFromMap(PyMOLGlobals * G, ObjectSlice * obj, ObjectMap * map,
                                int state, int map_state);

PyObject *ObjectSliceAsPyList(ObjectSlice * I);
int ObjectSliceNewFromPyList(PyMOLGlobals * G, PyObject * list, ObjectSlice ** result);

ObjectSliceState *ObjectSliceStateGetActive(ObjectSlice * I, int state);
void ObjectSliceDrag(ObjectSlice * I, int state, int mode, float *pt, float *mov,
                     float *z_dir);
int ObjectSliceGetVertex(ObjectSlice * I, int index, int base, float *v);

#endif

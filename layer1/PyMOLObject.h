
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
#ifndef _H_PyMOLObject
#define _H_PyMOLObject


/* literally a 3-D object...also an object object */

#include"Base.h"
#include"Ray.h"
#include"Rep.h"
#include"Setting.h"
#include"PyMOLGlobals.h"
#include"View.h"
#include"Word.h"
#include"vla.h"

typedef char ObjectNameType[WordLength];

enum cObject_t : int {
  cObjectMolecule = 1,
  cObjectMap = 2,
  cObjectMesh = 3,
  cObjectMeasurement = 4,
  cObjectCallback = 5,
  cObjectCGO = 6,
  cObjectSurface = 7,
  cObjectGadget = 8,
  cObjectCalculator = 9,
  cObjectSlice = 10,
  cObjectAlignment = 11,
  cObjectGroup = 12,
  cObjectVolume = 13,
};

/* 
   the object base class is in the process of being converted to support
   states explicitly (an unfortunate early omission), which will allow
   for simplified implementation of future multi-state objects.
 */

struct CObjectState {
  PyMOLGlobals *G;
  std::vector<double> Matrix;
  std::vector<double> InvMatrix;
  CObjectState() : G(nullptr){};
  CObjectState(PyMOLGlobals * G): G(G) {};
};

void ObjectUpdate(CObject*);
void ObjectRenderUnitBox(CObject*, RenderInfo*);
int ObjectGetNFrames(CObject*);
void ObjectDescribeElement(CObject*, int, char*);
void ObjectInvalidate(CObject*, int, int, int);
CSetting** ObjectGetSettingHandle(CObject*, int);

struct CObject {
  PyMOLGlobals *G = nullptr;
  void (*fUpdate) (CObject * I) = ObjectUpdate;        /* update representations */
  void (*fRender) (CObject * I, RenderInfo * info) = ObjectRenderUnitBox;
  int (*fGetNFrame) (CObject * I) = ObjectGetNFrames;
  void (*fDescribeElement) (CObject * I, int index, char *buffer) = ObjectDescribeElement;
  void (*fInvalidate) (CObject * I, int rep, int level, int state) = ObjectInvalidate;
  CSetting **(*fGetSettingHandle) (CObject * I, int state) = ObjectGetSettingHandle;
  char *(*fGetCaption) (CObject * I, char * ch, int len) = nullptr;
  CObjectState *(*fGetObjectState) (CObject * I, int state) = nullptr;
  cObject_t type;
  ObjectNameType Name {};
  int Color = 0;
  int visRep = 0;
  float ExtentMin[3]{}, ExtentMax[3]{};
  int ExtentFlag = false, TTTFlag = false;
  float TTT[16]{};                /* translate, transform, translate matrix (to apply when rendering) */
  CSetting *Setting = nullptr;
  int Enabled = 0;                  /* read-only... maintained by Scene */
  int Context = 0;                  /* 0 = Camera, 1 = Unit Window, 2 = Scaled Window */
  pymol::vla<CViewElem> ViewElem;          /* for animating objects via the TTT */

  /* not pickled */
  int grid_slot = 0;
  CGO *gridSlotSelIndicatorsCGO = nullptr;
  int Grabbed = 0;

  // methods

protected:
  CObject(PyMOLGlobals* G);

public:
  virtual ~CObject();

  void update() {
    if (fUpdate)
      fUpdate(this);
  }

  void render(RenderInfo * info) {
    if (fRender)
      fRender(this, info);
  }

  void invalidate(int rep, int level, int state) {
    if (fInvalidate)
      fInvalidate(this, rep, level, state);
  }
  int getNFrame() {
    if (fGetNFrame)
      return fGetNFrame(this);
    return 0;
  }
};

int ObjectCopyHeader(CObject * I, const CObject * src);
void ObjectSetName(CObject * I, const char *name);
bool ObjectMakeValidName(char *name);
void ObjectMakeValidName(PyMOLGlobals * G, char *name);
void ObjectPurgeSettings(CObject * I);
void ObjectUseColor(CObject * I);
void ObjectUseColorCGO(CGO *cgo, CObject * I);
void ObjectSetRepVisMask(CObject * I, int repmask, int value);
void ObjectToggleRepVis(CObject * I, int rep);
void ObjectPrepareContext(CObject * I, RenderInfo * info);
void ObjectSetTTT(CObject * I, const float *ttt, int state,int store);
int ObjectGetTTT(CObject * I, const float **ttt, int state);
int ObjectGetTotalMatrix(CObject * I, int state, int history, double *matrix);
void ObjectCombineTTT(CObject * I, const float *ttt, int reverse_order, int store);
void ObjectTranslateTTT(CObject * T, float *v,int store);
void ObjectSetTTTOrigin(CObject * I, float *origin);
void ObjectResetTTT(CObject * I,int store);
PyObject *ObjectAsPyList(CObject * I);
int ObjectFromPyList(PyMOLGlobals * G, PyObject * list, CObject * I);
int ObjectGetCurrentState(CObject * I, int ignore_all_states);
void ObjectAdjustStateRebuildRange(CObject * I, int *start, int *stop);
int ObjectMotion(CObject * I, int action, int first,
                 int last, float power, float bias,
                 int simple, float linear, int wrap,
                 int hand, int window, int cycles, int state, int quiet);
int ObjectGetSpecLevel(CObject * I, int frame);
void ObjectMotionTrim(CObject *I, int n_frame);
void ObjectDrawViewElem(CObject *I, BlockRect *rect, int frames, CGO *orthoCGO);
void ObjectStateInit(PyMOLGlobals * G, CObjectState * I);
void ObjectStatePurge(CObjectState * I);
int ObjectStateSetMatrix(CObjectState * I, double *matrix);
double *ObjectStateGetMatrix(CObjectState * I);
double *ObjectStateGetInvMatrix(CObjectState * I);
void ObjectStateTransformMatrix(CObjectState * I, double *matrix);
void ObjectStateResetMatrix(CObjectState * I);
PyObject *ObjectStateAsPyList(CObjectState * I);
int ObjectStateFromPyList(PyMOLGlobals * G, PyObject * list, CObjectState * I);
int ObjectStatePushAndApplyMatrix(CObjectState * I, RenderInfo * info);
void ObjectStatePopMatrix(CObjectState * I, RenderInfo * info);
void ObjectStateRightCombineMatrixR44d(CObjectState * I, double *matrix);
void ObjectStateLeftCombineMatrixR44d(CObjectState * I, double *matrix);
void ObjectStateCombineMatrixTTT(CObjectState * I, float *matrix);
int ObjectMotionModify(CObject *I,int action, int index, int count,int target, int freeze, int localize);
void ObjectMotionReinterpolate(CObject *I);
int ObjectMotionGetLength(CObject *I);

typedef struct _CObjectUpdateThreadInfo CObjectUpdateThreadInfo;

#define cObjectTypeAll                    0
#define cObjectTypeObjects                1
#define cObjectTypeSelections             2
#define cObjectTypePublic                 3
#define cObjectTypePublicObjects          4
#define cObjectTypePublicSelections       5
#define cObjectTypePublicNonGroupObjects  6
#define cObjectTypePublicGroupObjects     7
#define cObjectTypeNonGroupObjects        8
#define cObjectTypeGroupObjects           9
/* Note: public objects are ones that do not start with "_" */

// object and object-state level setting
template <typename V> void SettingSet(int index, V value, CObject * obj, int state=-1) {
  if (obj->fGetSettingHandle) {
    auto handle = obj->fGetSettingHandle(obj, state);
    if (handle)
      SettingSet(obj->G, handle, index, value);
  }
}

#endif

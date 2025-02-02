
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
#ifndef _H_CoordSet
#define _H_CoordSet

#include"os_python.h"
#include"Rep.h"
#include"Symmetry.h"
#include"Word.h"
#include"Setting.h"
#include"ObjectMolecule.h"
#include"vla.h"

#define COORD_SET_HAS_ANISOU 0x01

enum mmpymolx_prop_state_t {
  MMPYMOLX_PROP_STATE_NULL = 0, // invalidated
  MMPYMOLX_PROP_STATE_AUTO,     // auto-assigned (libmmpymolx)
  MMPYMOLX_PROP_STATE_USER,     // user-assigned (cmd.alter)
};

struct CoordSet {
  // methods (not fully refactored yet)
  void fFree();

  // methods
  void update(int state);
  void render(RenderInfo * info);
  void enumIndices();
  void appendIndices(int offset);
  int extendIndices(int nAtom);
  void invalidateRep(int type, int level);
  int atmToIdx(int atm) const;

  // read/write pointer to coordinate
  float * coordPtr(int idx) {
    return Coord + idx * 3;
  }

  // read pointer to coordinate
  const float * coordPtr(int idx) const {
    return Coord + idx * 3;
  }

  AtomInfoType * getAtomInfo(int idx) {
    return Obj->AtomInfo + IdxToAtm[idx];
  }

  // true if any atom in this coord set has any of the reps in "bitmask" shown
  bool hasRep(int bitmask) {
    if (Obj->RepVisCache & bitmask)
      for (int idx = 0; idx < NIndex; idx++)
        if (getAtomInfo(idx)->visRep & bitmask)
          return true;
    return false;
  }

  CObjectState State;
  ObjectMolecule *Obj = nullptr;
  pymol::vla<float> Coord;
  pymol::vla<int> IdxToAtm;
  pymol::vla<int> AtmToIdx;
  int NIndex = 0, NAtIndex = 0, prevNIndex = 0, prevNAtIndex = 0;
  ::Rep *Rep[cRepCnt] = {0};            /* an array of pointers to representations */
  int Active[cRepCnt] = {0};          /* active flags */
  int NTmpBond = 0;                 /* optional, temporary (for coord set transfers) */
  pymol::vla<BondType> TmpBond;            /* actual bond info is stored in ObjectMolecule */
  int NTmpLinkBond = 0;             /* optional, temporary storage of linkage  info. */
  pymol::vla<BondType> TmpLinkBond;        /* first atom is in obj, second is in cset */
  std::unique_ptr<CSymmetry> Symmetry;
  WordType Name = {0};
  std::vector<float> Spheroid;
  std::vector<float> SpheroidNormal;
  CSetting *Setting = nullptr;
  /* for periodic MD boxes -- may be merge into symmetry lattice later... */
  std::unique_ptr<CCrystal> PeriodicBox;
  int PeriodicBoxType = 0;
  int tmp_index = 0;                /* for saving */

  pymol::vla<LabPosType> LabPos;

  /* not saved in state */

  pymol::vla<RefPosType> RefPos;

  /* idea:  
     int start_atix, stop_atix <-- for discrete objects, we need
     something like this that would enable pymol to skip atoms not in the
     discrete state...question is: are these atoms sorted together right
     now or not? probably not, and if not then we need to change sorting
     for discrete objects to be state-dependent, but this could screw up
     byres/bychain actions which assume such atoms to be adjancent...
   */

  CGO *SculptCGO = nullptr;
  CGO *SculptShaderCGO = nullptr;
  MapType *Coord2Idx = nullptr;
  float Coord2IdxReq = 0, Coord2IdxDiv = 0;

  /* temporary / optimization */

  int objMolOpInvalidated = 0;
#ifdef _PYMOL_IP_EXTRAS
  mmpymolx_prop_state_t validMMStereo;
  mmpymolx_prop_state_t validTextType;
#endif

#ifdef _PYMOL_IP_PROPERTIES
#endif

  /* Atom-state Settings */
  pymol::vla<int> atom_state_setting_id;
  pymol::vla<char> has_atom_state_settings;
  CoordSet(PyMOLGlobals * G);
  CoordSet(const CoordSet &cs);
  ~CoordSet();
};

typedef void (*fUpdateFn) (CoordSet *, int);

#define cCSet_NoPeriodicity 0
#define cCSet_Orthogonal 1
#define cCSet_Octahedral 2

int BondInOrder(BondType * a, int b1, int b2);
int BondCompare(BondType * a, BondType * b);

PyObject *CoordSetAsNumPyArray(CoordSet * cs, short copy);
PyObject *CoordSetAsPyList(CoordSet * I);
int CoordSetFromPyList(PyMOLGlobals * G, PyObject * list, CoordSet ** cs);

void CoordSetAtomToPDBStrVLA(PyMOLGlobals * G, char **charVLA, int *c,
                             const AtomInfoType * ai,
                             const float *v, int cnt,
                             const PDBInfoRec * pdb_info,
                             const double *matrix);
#define CoordSetNew(G) new CoordSet(G)
CoordSet* CoordSetCopy(const CoordSet* src);

void CoordSetTransform44f(CoordSet * I, const float *mat);
void CoordSetTransform33f(CoordSet * I, const float *mat);
void CoordSetRealToFrac(CoordSet * I, const CCrystal * cryst);
void CoordSetFracToReal(CoordSet * I, const CCrystal * cryst);

bool CoordSetInsureOrthogonal(PyMOLGlobals * G,
    CoordSet * cset,
    const float * sca,
    const CCrystal *cryst=NULL,
    bool quiet=true);

void CoordSetGetAverage(CoordSet * I, float *v0);
PyObject *CoordSetAtomToChemPyAtom(PyMOLGlobals * G, AtomInfoType * ai, const float *v,
                                   const float *ref, int index, const double *matrix);
int CoordSetGetAtomVertex(CoordSet * I, int at, float *v);
int CoordSetGetAtomTxfVertex(CoordSet * I, int at, float *v);
int CoordSetSetAtomVertex(CoordSet * I, int at, const float *v);
int CoordSetMoveAtom(CoordSet * I, int at, const float *v, int mode);
int CoordSetMoveAtomLabel(CoordSet * I, int at, const float *v, const float *diff);

int CoordSetTransformAtomTTTf(CoordSet * I, int at, const float *TTT);
int CoordSetTransformAtomR44f(CoordSet * I, int at, const float *matrix);

int CoordSetValidateRefPos(CoordSet * I);

void CoordSetPurge(CoordSet * I);
void CoordSetAdjustAtmIdx(CoordSet * I, int *lookup, int nAtom);
int CoordSetMerge(ObjectMolecule *OM, CoordSet * I, CoordSet * cs);        /* must be non-overlapping */
void CoordSetRecordTxfApplied(CoordSet * I, const float *TTT, int homogenous);
void CoordSetUpdateCoord2IdxMap(CoordSet * I, float cutoff);

typedef struct _CCoordSetUpdateThreadInfo CCoordSetUpdateThreadInfo;

void CoordSetUpdateThread(CCoordSetUpdateThreadInfo * T);

void LabPosTypeCopy(const LabPosType * src, LabPosType * dst);
void RefPosTypeCopy(const RefPosType * src, RefPosType * dst);


#ifndef _PYMOL_NOPY
int CoordSetSetSettingFromPyObject(PyMOLGlobals * G, CoordSet *cs, int at, int setting_id, PyObject *val);
#endif
int CoordSetCheckSetting(PyMOLGlobals * G, CoordSet *cs, int at, int setting_id);
PyObject *SettingGetIfDefinedPyObject(PyMOLGlobals * G, CoordSet *cs, int at, int setting_id);
int CoordSetCheckUniqueID(PyMOLGlobals * G, CoordSet *cs, int at);

#define AtomStateGetSetting_b     AtomStateGetSetting
#define AtomStateGetSetting_i     AtomStateGetSetting
#define AtomStateGetSetting_f     AtomStateGetSetting
#define AtomStateGetSetting_s     AtomStateGetSetting
#define AtomStateGetSetting_color AtomStateGetSetting

#define ATOMSTATEGETSETTINGARGS PyMOLGlobals * G, \
  const ObjectMolecule * obj, \
  const CoordSet * cs, int idx, \
  const AtomInfoType * ai, \
  int setting_id

template <typename V> void AtomStateGetSetting(ATOMSTATEGETSETTINGARGS, V * out);

// atom-state level setting
template <typename V> void SettingSet(int index, V value, CoordSet *cs, int idx) {
  auto& G = cs->State.G;
  CoordSetCheckUniqueID(G, cs, idx);
  cs->has_atom_state_settings[idx] = true;
  SettingUniqueSet(G, cs->atom_state_setting_id[idx], index, value);
}

// object-state level setting
template <typename V> void SettingSet(int index, V value, CoordSet *cs) {
  SettingSet(cs->State.G, &cs->Setting, index, value);
}

// Rotates the ANISOU vector
bool RotateU(const double *matrix, float *anisou);

#endif

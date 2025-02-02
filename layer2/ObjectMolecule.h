/**
 * @file
 */
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
#ifndef _H_ObjectMolecule
#define _H_ObjectMolecule

#include"os_python.h"

#include"PyMOLObject.h"
#include"AtomInfo.h"
#include"Vector.h"
#include"Color.h"
#include"Symmetry.h"
#include"DistSet.h"
#include "Executive_pre.h"
#include "vla.h"
#include "Result.h"
#include "Sculpt.h"
#include <memory>

#ifdef _WEBGL
#endif

#define cKeywordAll "all"
#define cKeywordNone "none"
#define cKeywordSame "same"
#define cKeywordCenter "center"
#define cKeywordOrigin "origin"

#define cUndoMask 0xF

/*
 * ObjectMolecule's Bond Path (BP) Record
 */
typedef struct ObjectMoleculeBPRec {
  int *dist;
  int *list;
  int n_atom;
} ObjectMoleculeBPRec;

struct ObjectMolecule : public CObject {
	/* array of pointers to coordinate sets; one set per state */
  pymol::vla<CoordSet*> CSet;
	/* number of coordinate sets */
  int NCSet = 0;
  struct CoordSet *CSTmpl = nullptr;      /* template for trajectories, etc. */
	/* array of bonds */
  pymol::vla<BondType> Bond;
	/* array of atoms (infos) */
  pymol::vla<AtomInfoType> AtomInfo;
	/* number of atoms and bonds */
  int NAtom = 0, prevNAtom = 0;
  int NBond = 0, prevNBond = 0;
	/* is this object loaded as a discrete object? if so, number of states */
  int DiscreteFlag = 0;
  pymol::vla<int> DiscreteAtmToIdx;
  pymol::vla<CoordSet*> DiscreteCSet;
  /* proposed, for storing uniform trajectory data more efficiently:
     int *UniformAtmToIdx, *UniformIdxToAtm;  */
  int CurCSet = 0;                  /* Current state number */
  int SeleBase = 0;                 /* for internal usage by  selector & only valid during selection process */
  CSymmetry *Symmetry = 0;
  int *Neighbor = 0;
  float *UndoCoord[cUndoMask + 1] {};
  int UndoState[cUndoMask + 1] {};
  int UndoNIndex[cUndoMask + 1] {};
  int UndoIter = 0;
  CGO *UnitCellCGO = nullptr;
  int BondCounter = 0;
  int AtomCounter = 0;
  /* not stored */
  struct CSculpt *Sculpt =  nullptr;
  int RepVisCacheValid = 0;
  int RepVisCache = 0;     /* for transient storage during updates */

#ifndef _PYMOL_NO_UNDO
#endif

  // for reporting available assembly ids after mmCIF loading - SUBJECT TO CHANGE
  std::shared_ptr<pymol::cif_file> m_ciffile;
  const pymol::cif_data * m_cifdata = nullptr;

  // hetatm and ignore-flag by non-polymer classification
  bool need_hetatm_classification = false;

  // methods
  ObjectMolecule(PyMOLGlobals* G, int discreteFlag);
  ~ObjectMolecule();
  int getState() const;
  bool setNDiscrete(int natom);
  bool updateAtmToIdx();
  bool atomHasAnyCoordinates(size_t atm) const;
};

/* this is a record that holds information for specific types of Operatations on Molecules, eg. translation/rotation/etc */
typedef struct ObjectMoleculeOpRec {
  unsigned int code;
  Vector3f v1, v2;
  int cs1, cs2;
  int i1, i2, i3, i4, i5, i6, *vc1, *i1VLA, *ii1, *vp1;
  float f1, f2, *f1VLA, *f2VLA, *ff1;
  double d[3][3], d1;
  float *vv1, *vv2;
  char *charVLA;
  const char *s1;
  ObjectMolecule **obj1VLA, *obj3;
  AtomInfoType *ai, **ai1VLA;
  PyObject *py_ob1;
#ifdef _WEBGL
#endif
  float ttt[16], *mat1;
  int nvv1, nvv2;
  int include_static_singletons;
} ObjectMoleculeOpRec;

typedef struct {
  float maxAngle;
  float maxDistAtMaxAngle;
  float maxDistAtZero;
  float power_a, power_b;
  float factor_a, factor_b;     /* 0.5/(maxAngle^power_a), 0.5/(maxAngle^power_b)) */
  float cone_dangle;
} HBondCriteria;

typedef struct {
  int flag[3];
  float matrix[16];
} PDBScale;

enum {
  PDB_VARIANT_DEFAULT = 0,
  PDB_VARIANT_PQR,
  PDB_VARIANT_PDBQT,
  PDB_VARIANT_VDB,      /* VIPERdb */
};

typedef struct {
  int variant;
  int pqr_workarounds;
  PDBScale scale;
  int ignore_header_names;
  int multi_object_status;      /* 0 = unknown, 1 = is multi_object, -1 is not multi_object */
  int multiplex;

  inline bool is_pqr_file() const {
    return variant == PDB_VARIANT_PQR;
  }
} PDBInfoRec;


/* these four letter code are left over from an 
   earlier multicharacter constant implementation
   and should be replaced with something more verbose */

#define OMOP_AVRT 2
#define OMOP_SFIT 3
#define OMOP_COLR 4
#define OMOP_VISI 5
#define OMOP_TTTF 6
#define OMOP_ALTR 7
#define OMOP_SUMC 9
#define OMOP_VERT 10
#define OMOP_SVRT 11
#define OMOP_MOME 12
#define OMOP_INVA 13
#define OMOP_MNMX 15
#define OMOP_AlterState 16
#define OMOP_Flag 17
#define OMOP_LABL 18
#define OMOP_Identify    19
#define OMOP_Remove 20
#define OMOP_Protect 21
#define OMOP_Mask 22
#define OMOP_AddHydrogens 23
#define OMOP_SetB 24
#define OMOP_SaveUndo 25
#define OMOP_CountAtoms 26
#define OMOP_Cartoon 27
#define OMOP_Index 28
#define OMOP_PhiPsi 29
#define OMOP_SingleStateVertices 30
#define OMOP_IdentifyObjects 31
#define OMOP_FlagSet 32
#define OMOP_FlagClear 33
#define OMOP_PrepareFromTemplate 34
#define OMOP_SetGeometry 35
#define OMOP_CSetSumVertices 36
#define OMOP_CSetMoment 37
#define OMOP_CSetMinMax 38
#define OMOP_CSetIdxGetAndFlag 39
#define OMOP_CSetIdxSetFlagged 40
#define OMOP_GetObjects 41
#define OMOP_CSetMaxDistToPt 42
#define OMOP_MaxDistToPt 43
#define OMOP_CameraMinMax 44
#define OMOP_CSetCameraMinMax 45
#define OMOP_GetChains 46
#define OMOP_Spectrum 47
#if 0
#define OMOP_GetBFactors 48
#define OMOP_GetOccupancies 49
#define OMOP_GetPartialCharges 50
#endif
#define OMOP_StateVRT 51
#define OMOP_CheckVis 52
#define OMOP_OnOff 53
#define OMOP_Pop 54
#define OMOP_FixHydrogens 56
#define OMOP_Sort 57
#define OMOP_SetAtomicSetting 58
#define OMOP_CSetSumSqDistToPt 59
#define OMOP_RevalenceFromSource 60
#define OMOP_RevalenceByGuessing 61
#define OMOP_ReferenceStore 62
#define OMOP_ReferenceRecall 63
#define OMOP_ReferenceValidate 64
#define OMOP_ReferenceSwap 65
#define OMOP_RenameAtoms 66


typedef struct {
  ObjectMolecule *trg_obj, *mbl_obj;    /* target and mobile objects */
  int *trg_vla, *mbl_vla;
  int n_pair;
} ObjMolPairwise;

void ObjMolPairwiseInit(ObjMolPairwise * pairwise);
void ObjMolPairwisePurge(ObjMolPairwise * pairwise);

/**
 * loop iterators for the ObjectMolecule::Neighbor array
 *
 * Arguments: (Neighbor array, const int atom-index, int neighbor-atom-or-bond-index, int used-internally)
 *
 * Example:
 * @verbatim
   // iterate over neighbors of obj->AtomInfo[at]
   int neighbor_at, tmp;
   ITERNEIGHBORATOMS(obj->Neighbor, at, neighbor_at, tmp) {
     // do something with obj->AtomInfo[neighbor_at]
   }
   @endverbatim
 */
#define ITERNEIGHBORATOMS(N, a, n, i) for(i = N[a] + 1; (n = N[i]) > -1; i += 2)
#define ITERNEIGHBORBONDS(N, a, b, i) for(i = N[a] + 1; (b = N[i + 1]), N[i] > -1; i += 2)

int ObjectMoleculeGetMatrix(ObjectMolecule * I, int state, double **history);
int ObjectMoleculeSetMatrix(ObjectMolecule * I, int state, double *matrix);
int ObjectMoleculeGetTopNeighbor(PyMOLGlobals * G,
                                 ObjectMolecule * I, int start, int excluded);

int ObjectMoleculeGetNearestAtomIndex(ObjectMolecule * I, const float *point, float cutoff,
                                      int state, float *dist);
int ObjectMoleculeGetNearestBlendedColor(ObjectMolecule * I, const float *point, float cutoff,
                                         int state, float *dist, float *color,
                                         int sub_vdw);

int *ObjectMoleculeGetPrioritizedOtherIndexList(ObjectMolecule * I, struct CoordSet *cs);
int ObjectMoleculeGetPrioritizedOther(const int *other, int a1, int a2, int *double_sided);


/* */
int ObjectMoleculeAreAtomsBonded2(ObjectMolecule * obj0, int a0, ObjectMolecule * obj1,
                                  int a1);
int ObjectMoleculeIsAtomBondedToName(ObjectMolecule * obj, int a0, const char *name, int);
void ObjectMoleculeOpRecInit(ObjectMoleculeOpRec * op);
int ObjectMoleculeNewFromPyList(PyMOLGlobals * G, PyObject * list,
                                ObjectMolecule ** result);
PyObject *ObjectMoleculeAsPyList(ObjectMolecule * I);
int ObjectMoleculeSetStateTitle(ObjectMolecule * I, int state, const char *text);
const char *ObjectMoleculeGetStateTitle(ObjectMolecule * I, int state);
int ObjectMoleculeCheckFullStateSelection(ObjectMolecule * I, int sele, int state);

int ObjectMoleculeSetStateOrder(ObjectMolecule * I, int * order, int len);

int ObjectMoleculeAddPseudoatom(ObjectMolecule * I, int sele_index, const char *name,
                                const char *resn, const char *resi, const char *chain,
                                const char *segi, const char *elem, float vdw,
                                int hetatm, float b, float q, const char *label,
                                float *pos, int color, int state, int more, int quiet);

int ObjectMoleculeSort(ObjectMolecule * I);
ObjectMolecule *ObjectMoleculeCopy(const ObjectMolecule * obj);
void ObjectMoleculeFixChemistry(ObjectMolecule * I, int sele1, int sele2, int invalidate);

ObjectMolecule *ObjectMoleculeLoadTOPFile(PyMOLGlobals * G, ObjectMolecule * obj,
                                          const char *fname, int frame, int discrete);
ObjectMolecule *ObjectMoleculeLoadChemPyModel(PyMOLGlobals * G, ObjectMolecule * I,
                                              PyObject * model, int frame, int discrete);

ObjectMolecule *ObjectMoleculeLoadTRJFile(PyMOLGlobals * G, ObjectMolecule * obj,
                                          const char *fname, int frame, int interval,
                                          int average, int start, int stop, int max,
                                          const char *sele, int image, float *shift, int quiet);

ObjectMolecule *ObjectMoleculeLoadRSTFile(PyMOLGlobals * G, ObjectMolecule * obj,
                                          const char *fname, int frame, int quiet, char mode);

ObjectMolecule *ObjectMoleculeLoadCoords(PyMOLGlobals * G, ObjectMolecule * I,
                                         PyObject * coords, int frame);
ObjectMolecule *ObjectMoleculeLoadCoords(PyMOLGlobals * G, ObjectMolecule * I,
                                         const float * coords, int coords_len, int frame=-1);
ObjectMolecule *ObjectMoleculeLoadCoords(PyMOLGlobals * G, const char * name,
                                         const float * coords, int coords_len, int frame=-1);

ObjectMolecule *ObjectMoleculeReadStr(PyMOLGlobals * G, ObjectMolecule * I,
                                      const char **next_entry,
                                      cLoadType_t content_format, int frame,
                                      int discrete, int quiet, int multiplex,
                                      char *new_name,
				      short loadpropertiesall=false, OVLexicon *loadproplex=NULL);

ObjectMolecule *ObjectMoleculeReadPDBStr(PyMOLGlobals * G, ObjectMolecule * obj,
                                         const char *molstr, int frame, int discrete,
                                         char *pdb_name,
                                         const char **next_pdb, PDBInfoRec * pdb_info,
                                         int quiet, int *model_number);

int ObjectMoleculeExtendIndices(ObjectMolecule * I, int state);

void ObjectMoleculeInvalidate(ObjectMolecule * I, int rep, int level, int state);
void ObjectMoleculeInvalidateAtomType(ObjectMolecule *I, int state);

void ObjectMoleculeRenderSele(ObjectMolecule * I, int curState, int sele, int vis_only SELINDICATORARG);

void ObjectMoleculeSeleOp(ObjectMolecule * I, int sele, ObjectMoleculeOpRec * op);

struct CoordSet *ObjectMoleculeGetCoordSet(ObjectMolecule * I, int setIndex);
int ObjectMoleculeMerge(ObjectMolecule * I, pymol::vla<AtomInfoType>&& ai,
			struct CoordSet *cs, int bondSearchFlag,
			int aic_mask, int invalidate);
void ObjectMoleculeUpdateNonbonded(ObjectMolecule * I);
int ObjectMoleculeUpdateNeighbors(ObjectMolecule * I);
int ObjectMoleculeMoveAtom(ObjectMolecule * I, int state, int index, float *v, int mode,
                           int log);
int ObjectMoleculeMoveAtomLabel(ObjectMolecule * I, int state, int index, float *v, int log, float *diff);
int ObjectMoleculeGetAtomVertex(ObjectMolecule * I, int state, int index, float *v);
int ObjectMoleculeGetAtomTxfVertex(ObjectMolecule * I, int state, int index, float *v);
int ObjectMoleculeGetAtomIndex(ObjectMolecule * I, int sele);
int ObjectMoleculeTransformSelection(ObjectMolecule * I, int state,
                                     int sele, const float *TTT, int log,
                                     const char *sname, int homogenous, int global);
int ObjectMoleculeDoesAtomNeighborSele(ObjectMolecule * I, int index, int sele);
void ObjectMoleculeInferChemFromNeighGeom(ObjectMolecule * I, int state);
void ObjectMoleculeInferChemForProtein(ObjectMolecule * I, int state);
void ObjectMoleculeInferChemFromBonds(ObjectMolecule * I, int state);
void ObjectMoleculePurge(ObjectMolecule * I);

int ObjectMoleculeXferValences(ObjectMolecule * Ia, int sele1, int sele2,
                               int target_state, ObjectMolecule * Ib, int sele3,
                               int source_state, int quiet);
void ObjectMoleculeGuessValences(ObjectMolecule * I, int state, int *flag1, int *flag2,
                                 int reset);
int ObjectMoleculeAddBond(ObjectMolecule * I, int sele0, int sele1, int order);
pymol::Result<> ObjectMoleculeAddBondByIndices(
    ObjectMolecule* I, unsigned atm1, unsigned atm2, int order);
int ObjectMoleculeRemoveBonds(ObjectMolecule * I, int sele1, int sele2);

int ObjectMoleculeAutoDisableAtomNameWildcard(ObjectMolecule * I);

void ObjectMoleculeSaveUndo(ObjectMolecule * I, int state, int log);
void ObjectMoleculeUndo(ObjectMolecule * I, int dir);
int ObjectMoleculePrepareAtom(ObjectMolecule * I, int index, AtomInfoType * ai, bool uniquefy=true);
void ObjectMoleculeReplaceAtom(ObjectMolecule * I, int index, AtomInfoType&& ai);
int ObjectMoleculePreposReplAtom(ObjectMolecule * I, int index, AtomInfoType * ai);
void ObjectMoleculeCreateSpheroid(ObjectMolecule * I, int average);
int ObjectMoleculeSetAtomVertex(ObjectMolecule * I, int state, int index, float *v);
int ObjectMoleculeVerifyChemistry(ObjectMolecule * I, int state);
int ObjectMoleculeFindOpenValenceVector(ObjectMolecule * I, int state,
                                        int index, float *v, float *seek,
                                        int ignore_index);
int ObjectMoleculeFillOpenValences(ObjectMolecule * I, int index);
int ObjectMoleculeGetTotalAtomValence(ObjectMolecule * I, int atom);
int ObjectMoleculeAdjustBonds(ObjectMolecule * I, int sele0, int sele1, int mode,
                              int order);
int ObjectMoleculeAttach(ObjectMolecule * I, int index,
    pymol::vla<AtomInfoType>&& nai);
int ObjectMoleculeFuse(ObjectMolecule * I, int index0, ObjectMolecule * src, int index1,
                        int mode, int move_flag);
int ObjectMoleculeRenameAtoms(ObjectMolecule * I, int *flag, int force);
int ObjectMoleculeAreAtomsBonded(ObjectMolecule * I, int i0, int i1);
void ObjectGotoState(ObjectMolecule * I, int state);
float ObjectMoleculeGetAvgHBondVector(ObjectMolecule * I, int atom, int state, float *v,
                                      float *incoming);
int ObjectMoleculeCheckBondSep(ObjectMolecule * I, int a0, int a1, int dist);
int ObjectMoleculeGetPhiPsi(ObjectMolecule * I, int ca, float *phi, float *psi,
                            int state);
void ObjectMoleculeGetAtomSele(ObjectMolecule * I, int index, char *buffer);
void ObjectMoleculeGetAtomSeleFast(ObjectMolecule * I, int index, char *buffer);
void ObjectMoleculeGetAtomSeleLog(ObjectMolecule * I, int index, char *buffer, int quote);

void ObjectMoleculeUpdateIDNumbers(ObjectMolecule * I);

void ObjectMoleculeSculptImprint(ObjectMolecule * I, int state, int match_state,
                                 int match_by_segment);
float ObjectMoleculeSculptIterate(ObjectMolecule * I, int state, int n_cycle,
                                  float *center);
void ObjectMoleculeSculptClear(ObjectMolecule * I);

/* bond paths */
int ObjectMoleculeGetBondPaths(ObjectMolecule * I, int atom, int max,
                               ObjectMoleculeBPRec * bp);
int ObjectMoleculeInitBondPath(ObjectMolecule * I, ObjectMoleculeBPRec * bp);
int ObjectMoleculePurgeBondPath(ObjectMolecule * I, ObjectMoleculeBPRec * bp);
int ***ObjectMoleculeGetBondPrint(ObjectMolecule * I, int max_bond, int max_type,
                                  int *dim);

bool ObjectMoleculeConnect(ObjectMolecule* I, CoordSet* cs,
    bool searchFlag = true, int connectModeOverride = -1);
bool ObjectMoleculeConnect(ObjectMolecule* I, int& nbond, pymol::vla<BondType>& bond,
                          struct CoordSet *cs, int searchFlag, int connectModeOverride);
int ObjectMoleculeSetDiscrete(PyMOLGlobals * G, ObjectMolecule * I, int discrete);

float ObjectMoleculeGetMaxVDW(ObjectMolecule * I);
int ObjectMoleculeGetCheckHBond(AtomInfoType **h_real,
                                float *h_crd_ret,
                                ObjectMolecule * don_obj,
                                int don_atom,
                                int don_state,
                                ObjectMolecule * acc_obj,
                                int acc_atom, int acc_state, HBondCriteria * hbc);
void ObjectMoleculeInitHBondCriteria(PyMOLGlobals * G, HBondCriteria * hbc);
int ObjectMoleculeConvertIDsToIndices(ObjectMolecule * I, int *id, int n_id);

#define cObjectMoleculeDummyOrigin 1
#define cObjectMoleculeDummyCenter 2

ObjectMolecule *ObjectMoleculeDummyNew(PyMOLGlobals * G, int mode);
void ObjectMoleculeDummyUpdate(ObjectMolecule * I, int mode);

void ObjectMoleculeTransformState44f(ObjectMolecule * I, int state, const float *matrix,
                                     int log_trans, int homogenous, int transformed);


/* internal to ObjectMolecule */

struct CoordSet *ObjectMoleculePDBStr2CoordSet(PyMOLGlobals * G,
                                               const char *buffer,
                                               AtomInfoType ** atInfoPtr,
                                               const char **restart_model,
                                               char *segi_override,
                                               char *pdb_name,
                                               const char **next_pdb,
                                               PDBInfoRec * pdb_info,
                                               int quiet, int *model_number);

#ifndef NO_MMLIBS
int ObjectMoleculeUpdateAtomTypeInfoForState(PyMOLGlobals * G, ObjectMolecule * obj, int state, int initialize, int format);
#endif

#ifdef _PYMOL_IP_EXTRAS
int ObjectMoleculeUpdateMMStereoInfoForState(PyMOLGlobals * G, ObjectMolecule * obj, int state, int initialize=1);
#endif

#ifndef _PYMOL_NO_UNDO
void ObjectMoleculeSetAtomBondInfoTypeOldId(PyMOLGlobals * G, ObjectMolecule * obj);
void ObjectMoleculeSetAtomBondInfoTypeOldIdToNegOne(PyMOLGlobals * G, ObjectMolecule * obj);
#endif

void ObjectMoleculeAdjustDiscreteAtmIdx(ObjectMolecule *I, int *lookup, int nAtom);

void AtomInfoSettingGenerateSideEffects(PyMOLGlobals * G, ObjectMolecule *obj, int index, int id);

int *AtomInfoGetSortedIndex(PyMOLGlobals * G,
    const ObjectMolecule* obj,
    const AtomInfoType* rec, int n,
                            int **outdex);

ObjectMolecule *ObjectMoleculeReadMmtfStr(PyMOLGlobals * G, ObjectMolecule * I,
    const char *st, int st_len, int frame, int discrete, int quiet, int multiplex, int zoom);
ObjectMolecule *ObjectMoleculeReadCifStr(PyMOLGlobals * G, ObjectMolecule * I,
    const char *st, int frame, int discrete, int quiet, int multiplex, int zoom);

// object and object-state level setting
template <typename V> void SettingSet(int index, V value, ObjectMolecule * I, int state=-1) {
  SettingSet(index, value, (CObject*)I, state);
}

#endif

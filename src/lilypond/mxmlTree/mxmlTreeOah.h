/*
  MusicXML Library
  Copyright (C) Grame 2006-2013

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr
*/

#ifndef ___mxmlTreeOah___
#define ___mxmlTreeOah___

#include "oahBasicTypes.h"
#include "exports.h"


namespace MusicXML2
{

//______________________________________________________________________________
class mxmlTreeOah : public oahGroup
{
  public:

    // creation
    // ------------------------------------------------------

    static SMARTP<mxmlTreeOah> create (
      S_oahHandler handlerUpLink);

    SMARTP<mxmlTreeOah>   createCloneWithDetailedTrace ();

  private:

    // initialisation
    // ------------------------------------------------------

    void                  initializeMxmlTreeOah (
                            bool boolOptionsInitialValue);

  protected:

    // constructors/destructor
    // ------------------------------------------------------

    mxmlTreeOah (
      S_oahHandler handlerUpLink);

    virtual ~mxmlTreeOah ();

  public:

    // quiet mode
    // ------------------------------------------------------

    void                  enforceQuietness ();

  public:

    // consistency check
    // ------------------------------------------------------

    virtual void          checkOptionsConsistency ();

  public:

    // set and get
    // ------------------------------------------------------

    void                  setAllMusicXMLTraceOah (
                            bool boolOptionsInitialValue);

  public:

    // public services
    // ------------------------------------------------------

  private:

    // private services
    // ------------------------------------------------------

#ifdef TRACE_OAH
    void                  initializeMusicXMLTraceOah (
                            bool boolOptionsInitialValue);
#endif

    void                  initializeMusicXMLHeaderOptions (
                            bool boolOptionsInitialValue);

    void                  initializeMusicXMLClefsKeysTimesOptions (
                            bool boolOptionsInitialValue);

    void                  initializeMusicXMMeasuresOptions (
                            bool boolOptionsInitialValue);

    void                  initializeMusicXMLDynamicsAndWedgesOptions (
                            bool boolOptionsInitialValue);

    void                  initializeMusicXMLCombinedOptionsOptions (
                            bool boolOptionsInitialValue);

    void                  initializeMusicXMLLoopOptions (
                            bool boolOptionsInitialValue);

  public:

    // visitors
    // ------------------------------------------------------

    virtual void          acceptIn  (basevisitor* v);
    virtual void          acceptOut (basevisitor* v);

    virtual void          browseData (basevisitor* v);

  public:

    // print
    // ------------------------------------------------------

    void                  printMxmlTreeOahHelp ();

    void                  printMxmlTreeOahValues (int fieldWidth);

  public:

    // fields
    // ------------------------------------------------------

    // header
    // --------------------------------------

    bool                  fUseFilenameAsWorkTitle;

    bool                  fUseLyricistsAsPoets; // lyricist is unknown to LilyPond

    // clefs, keys, times
    // --------------------------------------

    bool                  fIgnoreRedundantClefs;
    bool                  fIgnoreRedundantKeys;
    bool                  fIgnoreRedundantTimes;

    S_oahBooleanAtom      fIgnoreRedundantClefsAtom;
    S_oahBooleanAtom      fIgnoreRedundantKeysAtom;
    S_oahBooleanAtom      fIgnoreRedundantTimesAtom;

    // measures
    // --------------------------------------

    map<string,int>       fAddEmptyMeasuresStringToIntMap;

    // dynamics and wedges
    // --------------------------------------

    bool                  fAllDynamicsBelow;
    bool                  fAllWedgesBelow;

    // combined options, cubase
    // --------------------------------------

    bool                  fCubase; // JMI ???
    bool                  fNoCubase;

    // loop to do JMI
    // --------------------------------------
    bool                  fLoopToMusicXML;

#ifdef TRACE_OAH
    // specific trace JMI move to traceOah
    // --------------------------------------

    // visitors
    bool                  fTraceMusicXMLTreeVisitors;

    // encoding
    bool                  fTraceEncoding;

    // divisions
    bool                  fTraceDivisions;

    // backup
    bool                  fTraceBackup;

    // forward
    bool                  fTraceForward;
#endif
};
typedef SMARTP<mxmlTreeOah> S_mxmlTreeOah;
EXP ostream& operator<< (ostream& os, const S_mxmlTreeOah& elt);

EXP extern S_mxmlTreeOah gMxmlTreeOah;
EXP extern S_mxmlTreeOah gMxmlTreeOahUserChoices;
EXP extern S_mxmlTreeOah gMxmlTreeOahWithDetailedTrace;

//______________________________________________________________________________
void initializeMxmlTreeOahHandling (
  S_oahHandler handler);


}


#endif

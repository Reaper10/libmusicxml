/*
  MusicXML Library
  Copyright (C) Grame 2006-2013

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr
*/

#ifndef __lpsr__
#define __lpsr__

#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <map>

#include "smartpointer.h"
#include "rational.h"
#include "exports.h"
#include "typedefs.h"

#include "msr.h"

//JMI#include "lpsrUtilities.h"

namespace MusicXML2 
{

/*!
\addtogroup lpsr
@{
*/

/*
  The classes in this file implement the
  
              LPSR (LilyPond Semantic Representation)
              
    - the base class is lpsrElement, derived from msrElement
    - the lpsrElement tree can be converted to:
        - a text description with         printLPSR()
        - LilyPond source code with       printLilypondCode()
*/

/*!
\brief Global variables.

  An global variable is implemented as a static member of this class.
*/
//______________________________________________________________________________

/*!
\brief A msr absolute octave representation.
*/
//______________________________________________________________________________
/* JMI
class EXP msrAbsoluteOctave : public msrElement
{
  public:
  
    static SMARTP<msrAbsoluteOctave> create (
          S_msrOptions&     msrOpts, 
      S_lpsrOptions&    lpsrOpts, 
      int                    inputLineNumber,
      int                    musicxmlOctave);
    
    msrAbsoluteOctave (
          S_msrOptions&     msrOpts, 
      S_lpsrOptions&    lpsrOpts, 
      int                    inputLineNumber,
      int                    musicxmlOctave);
      
    virtual ~msrAbsoluteOctave();
    
    msrAbsoluteOctave& operator= (const msrAbsoluteOctave& absOct)
      {
        fMsrOctave = absOct.fMsrOctave;
        return *this;
      }
          
    bool operator!= (const msrAbsoluteOctave& otherAbsOct) const 
      { 
        return fMsrOctave != otherAbsOct.fMsrOctave;
      }
    
    string  absoluteOctaveAsLilypondString ();

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  private:

    int  fMsrOctave;
};
typedef SMARTP<msrAbsoluteOctave> S_msrAbsoluteOctave;
EXP ostream& operator<< (ostream& os, const S_msrAbsoluteOctave& elt);
*/

/*!
  \brief The LPSR code generation options.

  A class is used to avoid passing arguments one by one
  to the various methods that need them.
*/
//______________________________________________________________________________
class EXP lpsrOptions : public smartable {
  public:

    static SMARTP<lpsrOptions> create ();
    
  public:
  
    lpsrOptions();
    virtual ~lpsrOptions();
 
  public:

    // MSR display
    bool                            fDisplayLPSR;

    // LilyPond code generation
    bool                            fGenerateAbsoluteOctaves;

    bool                            fDontKeepLineBreaks;
    bool                            fKeepStaffSize;
    
    bool                            fGenerateNumericalTime;
    bool                            fGenerateComments;
    bool                            fGenerateStems;
    bool                            fGeneratePositions;
    
    bool                            fDontGenerateLilyPondLyrics;

    // LilyPond source code display
    bool                            fDontDisplayLilyPondCode;
};
typedef SMARTP<lpsrOptions> S_lpsrOptions;

/*!
\brief A generic msr element representation.

  An element is represented by its name and the
  list of its enclosed elements plus optional parameters.
*/
//______________________________________________________________________________
class EXP lpsrElement : public msrElement
{
  public:

    static SMARTP<lpsrElement> create (
      S_msrOptions   msrOpts,
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

  protected:
         
    lpsrElement (
      S_msrOptions   msrOpts,
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);

    virtual ~lpsrElement();

    S_lpsrOptions fLpsrOptions;
};
typedef SMARTP<lpsrElement> S_lpsrElement;
EXP ostream& operator<< (ostream& os, const S_lpsrElement& elt);

/*!
\brief The lpsr parallel music element
*/
//______________________________________________________________________________
class EXP lpsrParallelMusic : public lpsrElement
{
  public:
    
    enum lpsrElementsSeparator { kEndOfLine, kSpace };

    static SMARTP<lpsrParallelMusic> create (
      S_msrOptions          msrOpts,
      S_lpsrOptions&        lpsrOpts, 
      int                   inputLineNumber,
      lpsrElementsSeparator elementsSeparator);

    void          addElementToParallelMusic (S_msrElement elem)
                    { fParallelMusicElements.push_back(elem); }
    S_msrElement  getLastElementOfParallelMusic()
                    { return fParallelMusicElements.back(); }
    void          removeLastElementOfParallelMusic ()
                    { fParallelMusicElements.pop_back(); }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrParallelMusic (
      S_msrOptions          msrOpts,
      S_lpsrOptions&        lpsrOpts, 
      int                   inputLineNumber,
      lpsrElementsSeparator elementsSeparato);
      
    virtual ~lpsrParallelMusic();
    
  private:
  
    vector<S_msrElement>  fParallelMusicElements;
    lpsrElementsSeparator fElementsSeparator;

};
typedef SMARTP<lpsrParallelMusic> S_lpsrParallelMusic;
EXP ostream& operator<< (ostream& os, const S_lpsrParallelMusic& elt);

/*!
\brief A lpsr LilyPond variable/value association representation.
*/
//______________________________________________________________________________
class EXP lpsrVarValAssoc : public lpsrElement
{
  public:

    enum lpsrCommentedKind   { kCommented, kUncommented };
    enum lpsrBackslashKind   { kWithBackslash, kWithoutBackslash };
    enum lpsrVarValSeparator { kSpace, kEqualSign };
    enum lpsrQuotesKind      { kQuotesAroundValue, kNoQuotesAroundValue };
    enum lpsrEndlKind        { kWithEndl, kWithoutEndl };

    static string const g_NoUnit;

    static SMARTP<lpsrVarValAssoc> create (
        S_msrOptions&       msrOpts, 
        S_lpsrOptions&      lpsrOpts, 
        int                 inputLineNumber,
        lpsrCommentedKind   commentedKind,
        lpsrBackslashKind   backslashKind,
        string              variableName,
        lpsrVarValSeparator varValSeparator,
        lpsrQuotesKind      quotesKind,
        string              value, 
        string              unit,
        lpsrEndlKind        endlKind);

    void      changeAssocValue (string value)
                  { fVariableValue = value; }

    lpsrCommentedKind
              getCommentedKind () const
                  { return fCommentedKind; };

    lpsrBackslashKind
              getBackslashKind () const
                  { return fBackslashKind; }
                  
    string    getVariableName  () const
                  { return fVariableName; };
                  
    lpsrVarValSeparator
              getVarValSeparator () const
                  { return fVarValSeparator; };
    
    lpsrQuotesKind
              getQuotesKind () const
                  { return fQuotesKind; };
                  
    string    getVariableValue () const
                  { return fVariableValue; };
    
    string    getUnit () const
                  { return fUnit; };

    lpsrEndlKind
              getEndlKind () const
                  { return fEndlKind; };

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrVarValAssoc (
        S_msrOptions&       msrOpts, 
        S_lpsrOptions&      lpsrOpts, 
        int                 inputLineNumber,
        lpsrCommentedKind   commentedKind,
        lpsrBackslashKind   backslashKind,
        string              variableName,
        lpsrVarValSeparator varValSeparator,
        lpsrQuotesKind      quotesKind,
        string              value, 
        string              unit,
        lpsrEndlKind        endlKind);
      
    virtual ~lpsrVarValAssoc();
  
  private:

    lpsrCommentedKind   fCommentedKind;
    lpsrBackslashKind   fBackslashKind;
    string              fVariableName;
    lpsrVarValSeparator fVarValSeparator;
    lpsrQuotesKind      fQuotesKind;
    string              fVariableValue;
    string              fUnit;
    lpsrEndlKind        fEndlKind;
};
typedef SMARTP<lpsrVarValAssoc> S_lpsrVarValAssoc;
EXP ostream& operator<< (ostream& os, const S_lpsrVarValAssoc& elt);

/*!
\brief A lpsr Scheme variable/value association representation.
*/
//______________________________________________________________________________
class EXP lpsrSchemeVarValAssoc : public lpsrElement
{
  public:

    enum lpsrCommentedKind { kCommented, kUncommented };
    enum lpsrEndlKind      { kWithEndl, kWithoutEndl };

    static SMARTP<lpsrSchemeVarValAssoc> create (
      S_msrOptions&     msrOpts, 
      S_lpsrOptions&    lpsrOpts, 
      int               inputLineNumber,
      string            variableName,
      string            value, 
      lpsrCommentedKind commentedKind,
      lpsrEndlKind      endlKind);
    
    void      changeAssocValue (string value)
                  { fVariableValue = value; }

    lpsrCommentedKind
              getCommentedKind () const { return fCommentedKind; };

    string    getVariableName  () const { return fVariableName; };
    string    getVariableValue () const { return fVariableValue; };

    lpsrEndlKind
              getEndlKind () const
                  { return fEndlKind; };

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrSchemeVarValAssoc (
      S_msrOptions&     msrOpts, 
      S_lpsrOptions&    lpsrOpts, 
      int               inputLineNumber,
      string            variableName,
      string            value, 
      lpsrCommentedKind commentedKind,
      lpsrEndlKind      endlKind);
      
    virtual ~lpsrSchemeVarValAssoc();
  
  private:

    lpsrCommentedKind fCommentedKind;
    
    string            fVariableName;
    string            fVariableValue;

    lpsrEndlKind      fEndlKind;
    
};
typedef SMARTP<lpsrSchemeVarValAssoc> S_lpsrSchemeVarValAssoc;
EXP ostream& operator<< (ostream& os, const S_lpsrSchemeVarValAssoc& elt);

/*!
\brief A lpsr comment representation.

  A comment is represented by its contents
*/
//______________________________________________________________________________
class EXP lpsrComment : public lpsrElement
{
  public:
    
    enum lpsrGapKind { kGapAfterwards, kNoGapAfterwards };

    static SMARTP<lpsrComment> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      string         contents,
      lpsrGapKind    gapKind = kNoGapAfterwards);

    string       getContents () const { return fContents; }

    lpsrGapKind  getGapKind  () const { return fGapKind; }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrComment (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      string         contents,
      lpsrGapKind    gapKind = kNoGapAfterwards);
      
    virtual ~lpsrComment();
  
  private:

    string      fContents;
    lpsrGapKind fGapKind;
};
typedef SMARTP<lpsrComment> S_lpsrComment;
EXP ostream& operator<< (ostream& os, const S_lpsrComment& elt);

/*!
\brief A lpsr barnumbercheck representation.

  A barnumbercheck is represented by the number of the next bar
*/
//______________________________________________________________________________
class EXP lpsrBarNumberCheck : public lpsrElement
{
  public:
    
    static SMARTP<lpsrBarNumberCheck> create (
          S_msrOptions&     msrOpts, 
      S_lpsrOptions&    lpsrOpts, 
      int                    inputLineNumber,
      int                    nextBarNumber);

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrBarNumberCheck(
          S_msrOptions&     msrOpts, 
      S_lpsrOptions&    lpsrOpts, 
      int                    inputLineNumber,
      int                    nextBarNumber);
      
    virtual ~lpsrBarNumberCheck();
  
  private:

    int fNextBarNumber;
};
typedef SMARTP<lpsrBarNumberCheck> S_lpsrBarNumberCheck;

/*!
\brief A lpsr new staff representation.

  A new staff is represented by a vactor of elements
*/
//______________________________________________________________________________
class EXP lpsrNewStaffgroupCommand : public lpsrElement
{
  public:

    static SMARTP<lpsrNewStaffgroupCommand> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumberr);
     
    void addElementToNewStaff (S_msrElement elem)
        { fNewStaffgroupElements.push_back(elem); }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrNewStaffgroupCommand (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
      
    virtual ~lpsrNewStaffgroupCommand();
  
  private:
  
    vector<S_msrElement> fNewStaffgroupElements;
};
typedef SMARTP<lpsrNewStaffgroupCommand> S_lpsrNewStaffgroupCommand;
EXP ostream& operator<< (ostream& os, const S_lpsrNewStaffgroupCommand& elt);

/*!
\brief A lpsr new staff representation.

  A new staff is represented by a vactor of elements
*/
//______________________________________________________________________________
class EXP lpsrNewStaffCommand : public lpsrElement
{
  public:

    static SMARTP<lpsrNewStaffCommand> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
     
    void addElementToNewStaff (S_msrElement elem)
        { fNewStaffElements.push_back(elem); }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrNewStaffCommand (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
      
    virtual ~lpsrNewStaffCommand();
  
  private:
  
    vector<S_msrElement> fNewStaffElements;
};
typedef SMARTP<lpsrNewStaffCommand> S_lpsrNewStaffCommand;
EXP ostream& operator<< (ostream& os, const S_lpsrNewStaffCommand& elt);

/*!
\brief A voice use representation.

  A voice use is represented by the voice to use
*/
//______________________________________________________________________________
class EXP lpsrUseVoiceCommand : public lpsrElement
{
  public:

    static SMARTP<lpsrUseVoiceCommand> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      S_msrVoice     voice);

    S_msrVoice   getVoice () const { return fVoice; }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrUseVoiceCommand (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      S_msrVoice     voice);
      
    virtual ~lpsrUseVoiceCommand();
  
  private:
  
    S_msrVoice fVoice;
};
typedef SMARTP<lpsrUseVoiceCommand> S_lpsrUseVoiceCommand;
EXP ostream& operator<< (ostream& os, const S_lpsrUseVoiceCommand& elt);

/*!
\brief A lpsr new lyrics representation.

  A new lyrics is represented by the voice name and the part name
*/

//______________________________________________________________________________
class EXP lpsrNewlyricsCommand : public lpsrElement
{
  public:

    static SMARTP<lpsrNewlyricsCommand> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      S_msrLyrics    lyrics,
      S_msrVoice     voice);

    S_msrLyrics  getLyrics () const { return fLyrics; }
    S_msrVoice   getVoice  () const { return fVoice; }
    
    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrNewlyricsCommand (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      S_msrLyrics    lyrics,
      S_msrVoice );
      
    virtual ~lpsrNewlyricsCommand();
  
  private:
  
    S_msrLyrics fLyrics;
    S_msrVoice  fVoice;
};
typedef SMARTP<lpsrNewlyricsCommand> S_lpsrNewlyricsCommand;
EXP ostream& operator<< (ostream& os, const S_lpsrNewlyricsCommand& elt);


/*!
\brief A lpsr variable use representation.

  A variable use is represented by the name of the variable to use
*/
//______________________________________________________________________________
class EXP lpsrVariableUseCommand : public lpsrElement
{
  public:

    static SMARTP<lpsrVariableUseCommand> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      string         variableName);

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrVariableUseCommand (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      string         variableName);
      
    virtual ~lpsrVariableUseCommand();
  
  private:
  
    string fVariableName;
};
typedef SMARTP<lpsrVariableUseCommand> S_lpsrVariableUseCommand;
EXP ostream& operator<< (ostream& os, const S_lpsrVariableUseCommand& elt);

/*!
\brief A lyrics use representation.

  A lyrics use is represented by the lyrics to use
*/
//______________________________________________________________________________
/*
class EXP lpsrUseLyricsCommand : public lpsrElement
{
  public:

    static SMARTP<lpsrUseLyricsCommand> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      S_msrLyrics&   lyrics);

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrUseLyricsCommand (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      S_msrLyrics&   lyrics);
      
    virtual ~lpsrUseLyricsCommand();
  
  private:
  
    S_msrLyrics fLyrics;
};
typedef SMARTP<lpsrUseLyricsCommand> S_lpsrUseLyricsCommand;
EXP ostream& operator<< (ostream& os, const S_lpsrUseLyricsCommand& elt);
*/

/*!
\brief A context representation.

  A context is represented by its type, name and contents
  It may be:
    existing (such as an implicit Voice in a Score), \context Voice...
    or a new one, \new Voice...
*/
//______________________________________________________________________________
class EXP lpsrContext : public lpsrElement
{
  public:

    enum lpsrContextKind { kExistingContext, kNewContext };
    
    static SMARTP<lpsrContext> create (
      S_msrOptions&   msrOpts, 
      S_lpsrOptions&  lpsrOpts, 
      int             inputLineNumber,
      lpsrContextKind contextKind,
      string          contextType,
      string          contextName);
    
    void addElementToContext (S_msrElement elem)
        { fContextElements.push_back(elem); }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrContext (
      S_msrOptions&   msrOpts, 
      S_lpsrOptions&  lpsrOpts, 
      int             inputLineNumber,
      lpsrContextKind contextKind,
      string          contextType,
      string          contextName);
      
    virtual ~lpsrContext();
  
  private:
  
    lpsrContextKind   fContextKind;
    string            fContextType;
    string            fContextName;

    vector<S_msrElement> fContextElements;
};
typedef SMARTP<lpsrContext> S_lpsrContext;
EXP ostream& operator<< (ostream& os, const S_lpsrContext& elt);

/*!
\brief A lpsr bar representation.

  A bar is represented by a vactor of elements
*/
//______________________________________________________________________________
class EXP lpsrBarCommand : public lpsrElement
{
  public:

    static SMARTP<lpsrBarCommand> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrBarCommand (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
      
    virtual ~lpsrBarCommand();
  
  private:
};
typedef SMARTP<lpsrBarCommand> S_lpsrBarCommand;
EXP ostream& operator<< (ostream& os, const S_lpsrBarCommand& elt);

/*!
\brief A lpsr header representation.

  A header is represented by variable/value pairs
*/
//______________________________________________________________________________
class EXP lpsrHeader : public lpsrElement
{
  public:

    static SMARTP<lpsrHeader> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
    
    void        setWorkNumber (
                  int    inputLineNumber,
                  string val);

    void        setWorkTitle (
                  int    inputLineNumber,
                  string val);

    void        setMovementNumber (
                  int    inputLineNumber,
                  string vall);

    void        setMovementTitle (
                  int    inputLineNumber,
                  string val);

    void        addCreator (
                  int    inputLineNumber,
                  string type,
                  string val);

    void        setRights (
                  int    inputLineNumber,
                  string val);

    void        addSoftware (
                  int    inputLineNumber,
                  string val);

    void        setEncodingDate (
                  int    inputLineNumber,
                  string val);

    void        setScoreInstrument (
                  int    inputLineNumber,
                  string val);

    S_lpsrVarValAssoc
                getWorkNumber () const
                    { return fWorkNumber; }
    
    S_lpsrVarValAssoc
                getWorkTitle () const
                    { return fWorkTitle; }
    
    S_lpsrVarValAssoc
                getMovementNumber () const
                    { return fMovementNumber; }
    
    S_lpsrVarValAssoc
                getMovementTitle () const
                    { return fMovementTitle; }
    
    vector<S_lpsrVarValAssoc>
                getCreators () const
                    { return fCreators; };
    
    S_lpsrVarValAssoc
                getRights () const
                    { return fRights; }
    
    vector<S_lpsrVarValAssoc>
                getSoftwares () const
                    { return fSoftwares; };
    
    S_lpsrVarValAssoc
                getEncodingDate () const
                    { return fEncodingDate; }
    
    S_lpsrVarValAssoc
                getScoreInstrument () const
                    { return fScoreInstrument; }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrHeader (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
      
    virtual ~lpsrHeader();
  
  private:

    S_lpsrVarValAssoc         fWorkNumber;
    S_lpsrVarValAssoc         fWorkTitle;
    S_lpsrVarValAssoc         fMovementNumber;
    S_lpsrVarValAssoc         fMovementTitle;
    vector<S_lpsrVarValAssoc> fCreators;
    S_lpsrVarValAssoc         fRights;
    vector<S_lpsrVarValAssoc> fSoftwares;
    S_lpsrVarValAssoc         fEncodingDate;
    S_lpsrVarValAssoc         fScoreInstrument;

};
typedef SMARTP<lpsrHeader> S_lpsrHeader;
EXP ostream& operator<< (ostream& os, const S_lpsrHeader& elt);

/*!
\brief A msr paper representation.

  A paper is represented by variable/value pairs
*/
//______________________________________________________________________________
class EXP lpsrPaper : public msrElement
{
  public:

    static SMARTP<lpsrPaper> create (
      S_msrOptions& msrOpts, 
      int           inputLineNumber);
    
    void    setPaperWidth         (float val) { fPaperWidth = val; }
    float   getPaperWidth         () const    { return fPaperWidth; }

    void    setPaperHeight        (float val) { fPaperHeight = val; }
    float   getPaperHeight        () const    { return fPaperHeight; }

    void    setTopMargin          (float val) { fTopMargin = val; }
    float   getTopMargin          () const    { return fTopMargin; }

    void    setBottomMargin       (float val) { fBottomMargin = val; }
    float   getBottomMargin       () const    { return fBottomMargin; }

    void    setLeftMargin         (float val) { fLeftMargin = val; }
    float   getLeftMargin         () const    { return fLeftMargin; }

    void    setRightMargin        (float val) { fRightMargin = val; }
    float   getRightMargin        () const    { return fRightMargin; }

    void    setBetweenSystemSpace (float val) { fBetweenSystemSpace = val; }
    float   getBetweenSystemSpace () const    { return fBetweenSystemSpace; }

    void    setPageTopSpace       (float val) { fPageTopSpace = val; }
    float   getPageTopSpace       () const    { return fPageTopSpace; }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);


  protected:

    lpsrPaper (
      S_msrOptions& msrOpts, 
      int           inputLineNumber);
      
    virtual ~lpsrPaper();
  
  private:

    // page height, margins and the like in centimeters are in centimeters
    float             fPaperWidth;
    float             fPaperHeight;
    float             fTopMargin;
    float             fBottomMargin;
    float             fLeftMargin;
    float             fRightMargin;
    
    float             fBetweenSystemSpace;
    float             fPageTopSpace; 
};
typedef SMARTP<lpsrPaper> S_lpsrPaper;
EXP ostream& operator<< (ostream& os, const S_lpsrPaper& elt);

/*!
\brief A LPSR layout representation.

  A layout is represented by variable/value pairs
*/
//______________________________________________________________________________
class EXP lpsrLayout : public lpsrElement
{
  public:

    static SMARTP<lpsrLayout> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
/* JMI    
    int     setMillimeters        (float val) { fMillimeters = val; }
    int     getMillimeters        () const    { return fMillimeters; }

    void    setTenths             (int val)   { fTenths = val; }
    float   getTenths             () const    { return fTenths; }
*/

    float   getGlobalStaffSize () const
                { return fStaffGlobalSize; }
                
    void    setGlobalStaffSize (float size)
                { fStaffGlobalSize = size; }

    void    addLpsrVarValAssoc (
              S_lpsrVarValAssoc assoc)
                { fLpsrVarValAssocs.push_back (assoc); }
      
    void    addLpsrSchemeVarValAssoc (
              S_lpsrSchemeVarValAssoc assoc)
                { fLpsrSchemeVarValAssocs.push_back (assoc); }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrLayout (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
      
    virtual ~lpsrLayout();
  
  private:
  
//    int                             fMillimeters;
//    int                             fTenths;

    float                           fStaffGlobalSize;
    
    vector<S_lpsrVarValAssoc>       fLpsrVarValAssocs;
    vector<S_lpsrSchemeVarValAssoc> fLpsrSchemeVarValAssocs;
};
typedef SMARTP<lpsrLayout> S_lpsrLayout;
EXP ostream& operator<< (ostream& os, const S_lpsrLayout& elt);

/*!
\brief A lpsr score block representation.

  A score is represented by parallel music, score layout and midi
*/
//______________________________________________________________________________
class EXP lpsrScoreCommand : public lpsrElement
{
  public:

    static SMARTP<lpsrScoreCommand> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
     
    S_lpsrParallelMusic
                  getScoreCommandParallelMusic () const
                      { return fScoreCommandParallelMusic; }

    S_lpsrLayout  getScoreCommandLayout () const
                    { return fScoreCommandLayout; }

    S_msrMidi     getScoreCommandMidi () const
                      { return fScoreCommandMidi; }

    void          appendVoiceUseToParallelMusic (
                    S_lpsrUseVoiceCommand voiceUse)
                      {
                        fScoreCommandParallelMusic->
                          addElementToParallelMusic (voiceUse);
                      }
                  
    void          appendLyricsUseToParallelMusic (
                    S_lpsrNewlyricsCommand lyricsUse)
                      {
                        fScoreCommandParallelMusic->
                          addElementToParallelMusic (lyricsUse);
                      }

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrScoreCommand (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber);
      
    virtual ~lpsrScoreCommand();
  
  private:

    S_lpsrParallelMusic fScoreCommandParallelMusic;
    S_lpsrLayout        fScoreCommandLayout;
    S_msrMidi           fScoreCommandMidi;    
};
typedef SMARTP<lpsrScoreCommand> S_lpsrScoreCommand;
EXP ostream& operator<< (ostream& os, const S_lpsrScoreCommand& elt);

/*!
\brief A lpsr score representation.

  A score is represented by parallel music, score layout and midi
*/
//______________________________________________________________________________
class EXP lpsrScore : public lpsrElement
{
  public:

    static SMARTP<lpsrScore> create (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      S_msrScore     mScore);
     
    S_lpsrVarValAssoc
              getLilyPondVersion () const
                  { return fLilyPondVersion; }

    S_lpsrSchemeVarValAssoc
              getGlobalStaffSizeAssoc () const
                  { return fGlobalStaffSizeAssoc; }

    S_lpsrHeader
              getHeader () const
                  { return fHeader; }

    S_lpsrPaper
              getPaper () const
                  { return fPaper; }

    S_lpsrLayout
              getScoreLayout () const
                  { return fScoreLayout; }

    list<S_msrElement>
              getVoicesAndLyricsList () const
                  { return fScoreElements; }

    S_lpsrScoreCommand
              getScoreCommand () const
                  { return fScoreCommand; }

    void      setGlobalStaffSize (float size)
                  {
                    stringstream s;
                    s << size;
                    fGlobalStaffSizeAssoc->
                      changeAssocValue (s.str());
                  }
    
    void      appendCommentToScore (
                S_lpsrComment comment)
                  { fScoreElements.push_back (comment); }
                  
    void      appendSchemeVarValAssocToScore (
                S_lpsrSchemeVarValAssoc assoc)
                  { fScoreElements.push_back (assoc); }
                  
    void      prependSchemeVarValAssocToScore (
                S_lpsrSchemeVarValAssoc assoc)
                  { fScoreElements.push_front (assoc); }
                  
    void      appendVoiceToScoreElements (
                S_msrVoice voice)
                  { fScoreElements.push_back (voice); }
                  
    void      appendLyricsToScoreElements (
                S_msrLyrics lyrics)
                  { fScoreElements.push_back (lyrics); }

    void      appendVoiceUseToStoreCommand (
                S_msrVoice voice);

    void      appendLyricsUseToStoreCommand (
                S_msrLyrics lyrics);

    virtual void acceptIn  (basevisitor* v);
    virtual void acceptOut (basevisitor* v);

    virtual void browseData (basevisitor* v);

    virtual void print (ostream& os);

  protected:

    lpsrScore (
      S_msrOptions&  msrOpts, 
      S_lpsrOptions& lpsrOpts, 
      int            inputLineNumber,
      S_msrScore     mScore);
      
    virtual ~lpsrScore();
  
  private:

    // MSR data
    S_msrScore              fMsrScore;

    // general information
    S_lpsrVarValAssoc       fLilyPondVersion;
    S_lpsrSchemeVarValAssoc fGlobalStaffSizeAssoc;
    S_lpsrHeader            fHeader;
    S_lpsrPaper             fPaper;
    S_lpsrLayout            fScoreLayout;

    // variables, voices and lyrics
    list<S_msrElement>      fScoreElements;

    // score command
    S_lpsrScoreCommand      fScoreCommand;
};
typedef SMARTP<lpsrScore> S_lpsrScore;
EXP ostream& operator<< (ostream& os, const S_lpsrScore& elt);


/*! @} */

}


#endif

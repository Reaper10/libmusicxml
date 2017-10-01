/*
  MusicXML Library
  Copyright (C) Grame 2006-2013

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr
*/

#include <climits>      /* INT_MIN, INT_MAX */
#include <iomanip>      // setw, set::precision, ...

#include "utilities.h"

#include "lilypondOptions.h"


using namespace std;

namespace MusicXML2 
{

// useful shortcut macros
#define idtr indenter::gIndenter
#define tab  indenter::gIndenter.getSpacer ()

//_______________________________________________________________________________
S_lilypondOptions gLilypondOptions;
S_lilypondOptions gLilypondOptionsUserChoices;
S_lilypondOptions gLilypondOptionsWithDetailedTrace;

S_lilypondOptions lilypondOptions::create ()
{
  lilypondOptions* o = new lilypondOptions();
  assert(o!=0);
  return o;
}

lilypondOptions::lilypondOptions()
  : msrOptionsGroup (
    "LilyPond",
    "hlp", "helpLilypond",
R"()"
    )
{
  initializeLilypondOptions (false);
}

lilypondOptions::~lilypondOptions()
{}

void lilypondOptions::initializeLilypondOptions (
  bool boolOptionsInitialValue)
{
  // time
  // --------------------------------------

  {
    // variables
    
    fNumericalTime = boolOptionsInitialValue;
  
    S_msrOptionsSubGroup timeSubGroup =
      msrOptionsSubGroup::create (
        "Time",
        "hlilyti", "helpLilypondtime",
R"()"
      );
  
    appendOptionsSubGroup (timeSubGroup);

    timeSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "numt", "numericalTime",
R"(Generate numerical time signatures, such as '4/4' instead of 'C'.)",
          "numericalTime",
          fNumericalTime));
  }


  // notes
  // --------------------------------------

  {
    // variables
      
    fAbsoluteOctaves  = boolOptionsInitialValue;
    
    fAllDurations  = boolOptionsInitialValue;
  
    fStems          = boolOptionsInitialValue;
    fNoAutoBeaming  = boolOptionsInitialValue;
    
    fRomanStringNumbers = boolOptionsInitialValue;
    fAvoidOpenStrings    = boolOptionsInitialValue;
    
    fAccidentalStyle = kDefaultStyle;
  
    fCompressMultiMeasureRests = boolOptionsInitialValue;
  
    fNoteInputLineNumbers = boolOptionsInitialValue;
  
    S_msrOptionsSubGroup notesSubGroup =
      msrOptionsSubGroup::create (
        "Notes",
        "hlilyn", "helpLilypondNotes",
R"()"
      );
  
    appendOptionsSubGroup (notesSubGroup);

    notesSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "abs", "absolute",
R"(Generate LilyPond absolute note octaves. 
By default, relative octaves are generated.)",
          "absoluteOctaves",
          fAbsoluteOctaves));

    notesSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "alldurs", "allDurations",
R"(Generate all LilyPond durations. 
By default, a duration equal to preceding one met in the current voice
is omitted for code conciseness.)",
          "allDurations",
          fAllDurations));

    notesSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "stems", "",
R"(Generate \stemUp and \stemDown LilyPond commands.
By default, LilyPond will take care of that by itself.)",
          "stems",
          fStems));

    notesSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "noab", "noAutoBeaming",
R"(Generate '\set Voice.autoBeaming = ##f' in each voice 
to prevent LilyPond from handling beams automatically.)",
          "noAutoBeaming",
          fNoAutoBeaming));

    notesSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "rsn", "romanStringNumbers",
R"(Generate '\romanStringNumbers' in each voice 
for LilyPond to generate roman instead of arabic string numbers.)",
          "romanStringNumbers",
          fRomanStringNumbers));

    notesSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "aos", "avoidOpenStrings",
R"(Generate '\set TabStaff.restrainOpenStrings = ##t' in each voice 
to prevent LilyPond from using open strings.)",
          "avoidOpenStrings",
          fAvoidOpenStrings));

    notesSubGroup->
      appendOptionsItem (
        msrOptionsAccidentalStyleItem::create (
          "as", "accidentalStyle", // JMI
R"(Choose the LilyPond accidental 'style' among: 
  voice, modern, modern-cautionary, modern-voice, 
  modern-voice-cautionary, piano, piano-cautionary, 
  neo-modern, neo-modern-cautionary, neo-modern-voice,
  neo-modern-voice-cautionary, dodecaphonic, dodecaphonic-no-repeat,
  dodecaphonic-first, teaching, no-reset, forget.
The default is... 'default'.)",
          "style",
          "accidentalStyle",
          fAccidentalStyle));

    notesSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "cmmr", "compressMultiMeasureRests",
R"(Generate '\compressMMRests' at the beginning of voices.
By default, this command is commented.)",
          "compressMultiMeasureRests",
          fCompressMultiMeasureRests));

    notesSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "niln", "noteInputLineNumbers",
R"(Generate after each note and barline a comment containing
its MusicXML input line number.)",
          "noteInputLineNumbers",
          fNoteInputLineNumbers));
  }

  
  // bars
  // --------------------------------------

  {
    // variables
      
    fShowAllBarNumbers = boolOptionsInitialValue;
  
    // options
  
    S_msrOptionsSubGroup barsSubGroup =
      msrOptionsSubGroup::create (
        "Bars",
        "hlilyb", "helpLilypondBars",
R"()"
      );
  
    appendOptionsSubGroup (barsSubGroup);

    barsSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "sabn", "showAllBarNumbers",
R"(Generate LilyPond code to show all bar numbers.)",
          "showAllBarNumbers",
          fShowAllBarNumbers));
  }   


  // line breaks
  // --------------------------------------

  {
    // variables
      
    fDontKeepLineBreaks                  = boolOptionsInitialValue;
  
    fBreakLinesAtIncompleteRightMeasures = boolOptionsInitialValue;
    
    fSeparatorLineEveryNMeasures         = -1;
    
    // options
  
    S_msrOptionsSubGroup lineBreaksSubGroup =
      msrOptionsSubGroup::create (
        "Line breaks",
        "hlilylb", "helpLilypondLineBreaks",
R"()"
      );
  
    appendOptionsSubGroup (lineBreaksSubGroup);

    lineBreaksSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "ilb", "ignoreLineBreaks",
R"(Ignore the line breaks from the MusicXML input
and let LilyPond decide about them.)",
          "dontKeepLineBreaks",
          fDontKeepLineBreaks));

    lineBreaksSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "blairm", "breakLinesAtIncompleteRightMeasures",
R"(Generate a '\break' command at the end of incomplete right measures
which is handy in popular folk dances and tunes.)",
          "breakLinesAtIncompleteRightMeasures",
          fBreakLinesAtIncompleteRightMeasures));

    lineBreaksSubGroup->
      appendOptionsItem (
        msrOptionsIntegerItem::create (
          "slenm", "separatorLineEveryNMeasures",
R"(Generate an additional separator line for readability every 'n' measures,
where 'n' is a positive integer.
Nothing special is done by default.)",
          "n",
          "separatorLineEveryNMeasures",
          fSeparatorLineEveryNMeasures));
  }
  

  // page breaks
  // --------------------------------------

  {
    // variables  
   
    fDontKeepPageBreaks                  = boolOptionsInitialValue;
  
    // options
  
    S_msrOptionsSubGroup pageBreaksSubGroup =
      msrOptionsSubGroup::create (
        "Page breaks",
        "hlilypb", "helpLilypondPageBreaks",
R"()"
      );
  
    appendOptionsSubGroup (pageBreaksSubGroup);

    pageBreaksSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "ipb", "ignorePageBreaks",
R"(Ignore the page breaks from the MusicXML input
and let LilyPond decide about them.)",
          "dontKeepPageBreaks",
          fDontKeepPageBreaks));
  }
    

  // staves
  // --------------------------------------

  {
    // variables  
    
    fModernTab = boolOptionsInitialValue;
  
    // options
  
    S_msrOptionsSubGroup stavesSubGroup =
      msrOptionsSubGroup::create (
        "Staves",
        "hlilys", "helpLilypondStaves",
R"()"
      );
  
    appendOptionsSubGroup (stavesSubGroup);

    stavesSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "mtab", "modernTab",
R"(Generate '\moderntab' instead of the default '\tab'.)",
          "modernTab",
          fModernTab));
  }
  

  // tuplets
  // --------------------------------------

  {
    // variables  
  
    fTupletsOnALine = boolOptionsInitialValue;
    
    // options
  
    S_msrOptionsSubGroup tupletsSubGroup =
      msrOptionsSubGroup::create (
        "Tuplets",
        "hlilytu", "helpLilypondTuplets",
R"()"
      );
  
    appendOptionsSubGroup (tupletsSubGroup);

    tupletsSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "toal", "tupletsOnALine",
R"(Keep tuplets notes on the same line, instead of
'having \tuplet {' and '}' on separate lines.)",
          "tupletsOnALine",
          fTupletsOnALine));
  }
      

  // repeats
  // --------------------------------------
  
  {
    // variables
    
    fRepeatBrackets = boolOptionsInitialValue;
  
    // options
  
    S_msrOptionsSubGroup repeatsSubGroup =
      msrOptionsSubGroup::create (
        "Repeats",
        "hlilyr", "helpLilypondRepeats",
R"()"
      );
  
    appendOptionsSubGroup (repeatsSubGroup);

    repeatsSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "rbracks", "repeatBrackets",
R"(Generate repeats with brackets instead of regular bar lines)",
          "repeatBrackets",
          fRepeatBrackets));
  }
      
  // ornaments
  // --------------------------------------
  
  {
    // variables
    
    fDelayedOrnamentsFraction = rational (2, 3);
  
    // options
  
    S_msrOptionsSubGroup ornamentsSubGroup =
      msrOptionsSubGroup::create (
        "Ornaments",
        "hlilyo", "helpLilypondOrnaments",
R"()"
      );
  
    appendOptionsSubGroup (ornamentsSubGroup);

    ornamentsSubGroup->
      appendOptionsItem (
        msrOptionsRationalItem::create (
          "dof", "delayedOrnamentsFraction",
R"(Place the delayed turn/reverseturn at the given fraction
between the ornemented note and the next one.
The default value is '2/3'.)",
          "num/denom",
          "delayedOrnamentsFraction",
          fDelayedOrnamentsFraction));
  }

      
  // fonts
  // --------------------------------------

  {
    // variables
    
    fJazzFonts   = boolOptionsInitialValue;
  
    // options
  
    S_msrOptionsSubGroup fontsSubGroup =
      msrOptionsSubGroup::create (
        "Fonts",
        "hfonts", "helpFonts",
R"()"
      );
  
    appendOptionsSubGroup (fontsSubGroup);

    fontsSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "jazz", "jazzFonts",
R"(Use the LilyJazz font for notes, braces, text and chords.
This font should be installed so that LilyPond can use it.)",
          "jazzFonts",
          fJazzFonts));
  }


  // code generation
  // --------------------------------------
  
  {
    // variables  
  
    fComments            = boolOptionsInitialValue;
  
    fGlobal              = boolOptionsInitialValue;
    
    fDisplayMusic        = boolOptionsInitialValue;
      
    fNoLilypondCode      = boolOptionsInitialValue;
    
    fNoLilypondLyrics    = boolOptionsInitialValue;
    
    fLilypondCompileDate = boolOptionsInitialValue;
  
    // options
  
    S_msrOptionsSubGroup codeGenerationSubGroup =
      msrOptionsSubGroup::create (
        "Code generation",
        "hlilycg", "helpLilypondCodeGeneration",
R"()"
      );
  
    appendOptionsSubGroup (codeGenerationSubGroup);

    codeGenerationSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "com", "comments",
R"(Generate comments showing the structure of the score
such as '% part P_POne (P1).)",
          "comments",
          fComments));

    codeGenerationSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "global", "",
R"(Generate a 'global' empty variable and place a use of it
at the beginning of all voices.)",
          "global",
          fGlobal));

    codeGenerationSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "dm", "displayMusic",
R"(Place the contents of all voices inside a '\displayMusic' block,
for LilyPond to show its internal representation of the music.)",
          "displayMusic",
          fDisplayMusic));

    codeGenerationSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "nolpc", "noLilypondCode",
R"(Don't generate any LilyPond code.
That can be useful if only a summary of the score is needed.)",
          "noLilypondCode",
          fNoLilypondCode));

    codeGenerationSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "nolpl", "noLilypondLyrics",
R"(Don't generate any lyrics in the LilyPond code.)",
          "noLilypondLyrics",
          fNoLilypondLyrics));

    codeGenerationSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "lpcd", "lilypondCompileDate",
R"(Generate code to include the compilation date
when LilyPond creates the score.)",
          "lilypondCompileDate",
          fLilypondCompileDate));
  } 
      
    
  // score notation
  // --------------------------------------

  {
    // variables  
  
  //  fScoreNotationKind =  lilypondOptions::kWesternNotation;
      // default value

    fJianpu = false;
    
    // options
  
    S_msrOptionsSubGroup scoreNotationSubGroup =
      msrOptionsSubGroup::create (
        "Score notation",
        "hlilysn", "helpLilypondScoreNotation",
R"()"
      );
  
    appendOptionsSubGroup (scoreNotationSubGroup);

    scoreNotationSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "jianpu", "",
R"(Generate the score using jianpu (numbered) notation
instead of the default western notation.
That option needs lilypond-Jianpu to be accessible to LilyPond
(https://github.com/nybbs2003/lilypond-Jianpu/blob/master/jianpu10a.ly).)",
          "jianpu",
          fJianpu));

  } 
      

  // midi
  // --------------------------------------
  
  {
    // variables
    string midiTempoDuration  = "4";
    int    midiTempoPerSecond = 90;
    
    fMidiTempo.first  = midiTempoDuration;
    fMidiTempo.second = midiTempoPerSecond;
    
    fNoMidi = boolOptionsInitialValue;
    
    // options
  
    S_msrOptionsSubGroup midiSubGroup =
      msrOptionsSubGroup::create (
        "Midi",
        "hlilym", "helpLilypondMidi",
R"()"
      );
  
    appendOptionsSubGroup (midiSubGroup);

    midiSubGroup->
      appendOptionsItem (
        msrOptionsMidiTempoItem::create (
          "midiTempo", "",
R"(Generate a '\tempo' command in the \midi block.
'midiTempoSpecification' can be:
  'duration = perSecond'
or
  "duration = perSecond" .
'duration' is a string such as '8.', and 'perSecond' is an integer.
The single or double quotes are used to allow spaces around the '=' sign,
otherwise they can be dispensed with.
Using double quotes allows for shell variables substitutions, as in:
  PER_SECOND=66
  xml2lilypond -midiTempo "8. ${PER_SECOND}" .
The default midi tempo is '4 = 90'.)",
          "midiTempoSpecification",
          "midiTempo",
          fMidiTempo));

    midiSubGroup->
      appendOptionsItem (
        msrOptionsBooleanItem::create (
          "noMidi", "",
R"(Generate the '\midi' block as a comment instead of active code.)",
          "noMidi",
          fNoMidi));
  } 

  
  // JMI ???

  fKeepStaffSize = boolOptionsInitialValue;

  fSilentVoices = boolOptionsInitialValue;


  /* JMI
  for (
    set<string>::const_iterator i=fLilypondAccidentalStyles.begin();
    i!=fLilypondAccidentalStyles.end();
    i++) {
      cout << (*i) << endl;
  } // for
  */
}

S_lilypondOptions lilypondOptions::createCloneWithDetailedTrace ()
{
  S_lilypondOptions
    clone =
      lilypondOptions::create ();

  // time
  
  clone->fNumericalTime =
    fNumericalTime;

  // notes
  
  clone->fAbsoluteOctaves =
    fAbsoluteOctaves;

  clone->fAllDurations =
    fAllDurations;

  clone->fStems =
    fStems;

  clone->fNoAutoBeaming =
    fNoAutoBeaming;

  clone->fRomanStringNumbers =
    fRomanStringNumbers;

  clone->fAvoidOpenStrings =
    fAvoidOpenStrings;

  clone->fAccidentalStyle =
    fAccidentalStyle;

  clone->fCompressMultiMeasureRests =
    fCompressMultiMeasureRests;

  clone->fNoteInputLineNumbers =
    true;

  // bars
  
  clone->fShowAllBarNumbers =
    true;

  // line breaks

  clone->fDontKeepLineBreaks =
    fDontKeepLineBreaks;

  clone->fBreakLinesAtIncompleteRightMeasures =
    fBreakLinesAtIncompleteRightMeasures;

  clone->fSeparatorLineEveryNMeasures =
    fSeparatorLineEveryNMeasures;

  // page breaks

  clone->fDontKeepPageBreaks =
    fDontKeepPageBreaks;

  // staves
  
  clone->fModernTab =
    fModernTab;
  
  // tuplets
    
  clone->fTupletsOnALine =
    fTupletsOnALine;
  
  // repeats
  
  clone->fRepeatBrackets =
    fRepeatBrackets;
  
  // ornaments
  
  clone->fDelayedOrnamentsFraction =
    fDelayedOrnamentsFraction;
  
  // code generation

  clone->fComments =
    true;
  
  clone->fGlobal =
    fGlobal;
  
  clone->fDisplayMusic =
    fDisplayMusic;  
  
  clone->fNoLilypondCode =
    fNoLilypondCode;
    
  clone->fNoLilypondLyrics =
    fNoLilypondLyrics;
    
  clone->fLilypondCompileDate =
    fLilypondCompileDate;
    
  // score notation

  clone->fJianpu =
    fJianpu;
        
  // midi
  
  clone->fMidiTempo =
    fMidiTempo;
    
  clone->fNoMidi =
    fNoMidi;
    
// JMI ???

  clone->fKeepStaffSize =
    fKeepStaffSize;
    
  clone->fSilentVoices =
    fSilentVoices;

  return clone;
}

/* JMI
bool lilypondOptions::setAccidentalStyle (lpsrAccidentalStyle accidentalStyle)
{
  // is accidentalStyle in the accidental styles set?
  map<string, lpsrAccidentalStyle>::const_iterator
    it =
      gLpsrAccidentalStylesMap.find (accidentalStyle);
        
  if (it == gLpsrAccidentalStylesMap.end ()) {
    // no, accidentalStyle is unknown
    return false;
  }

  fAccidentalStyle = it.second;
  
  return true;
} 
*/

string lilypondOptions::scoreNotationKindAsString (
  scoreNotationKind notationKind)
{
  string result;

  switch (notationKind) {
    case lilypondOptions::kWesternNotation:
      result = "western";
      break;
    case lilypondOptions::kJianpuNotation:
      result = "jianpu";
      break;
    case lilypondOptions::kABCNotation:
      result = "abc";
      break;
  } // switch

  return result;
}

void lilypondOptions::printLilypondOptionsValues (int fieldWidth)
{
  cerr << idtr <<
    "The LilyPond options are:" <<
    endl;

  idtr++;
  
  // time
  // --------------------------------------
  cerr <<
    idtr << "Time:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "numericalTime" << " : " <<
      booleanAsString (fNumericalTime) <<
      endl;

  idtr--;
  
  // notes
  // --------------------------------------
  cerr <<
    idtr << "Notes:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "absoluteOctaves" << " : " <<
      booleanAsString (fAbsoluteOctaves) <<
      endl <<
    
    idtr << setw (fieldWidth) << "allDurations" << " : " <<
      booleanAsString (fAllDurations) <<
      endl <<
    
    idtr << setw (fieldWidth) << "stems" << " : " <<
      booleanAsString (fStems) <<
      endl <<

    idtr << setw (fieldWidth) << "noAutoBeaming" << " : " <<
      booleanAsString (fNoAutoBeaming) <<
      endl <<

    idtr << setw (fieldWidth) << "romanStringNumbers" << " : " <<
      booleanAsString (fRomanStringNumbers) <<
      endl <<
      
    idtr << setw (fieldWidth) << "avoidOpenString" << " : " <<
      booleanAsString (fAvoidOpenStrings) <<
      endl <<

    idtr << setw (fieldWidth) << "accidentalStyle" << " : " <<
      fAccidentalStyle <<
      endl <<
    
    idtr << setw (fieldWidth) << "compressMultiMeasureRests" << " : " <<
      booleanAsString (fCompressMultiMeasureRests) <<
      endl <<

    idtr << setw (fieldWidth) << "inputLineNumbers" << " : " <<
      booleanAsString (fNoteInputLineNumbers) <<
      endl;

  idtr--;
  
  // bars
  // --------------------------------------
  cerr <<
    idtr << "Bars:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "showAllBarNumbers" << " : " <<
      booleanAsString (fShowAllBarNumbers) <<
      endl;

  idtr--;

  // line breaks
  // --------------------------------------
  cerr <<
    idtr << "Line breaks:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "dontKeepLineBreaks" << " : " <<
      booleanAsString (fDontKeepLineBreaks) <<
      endl <<

    idtr << setw (fieldWidth) << "breakLinesAtIncompleteRightMeasures" << " : " <<
      booleanAsString (fBreakLinesAtIncompleteRightMeasures) <<
      endl <<
    
    idtr << setw (fieldWidth) << "separatorLineEveryNMeasures" << " : " <<
      booleanAsString (fSeparatorLineEveryNMeasures) <<
      endl;

  idtr--;
  
  // page breaks
  // --------------------------------------
  cerr <<
    idtr << "Page breaks:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "dontKeepPageBreaks" << " : " <<
      booleanAsString (fDontKeepPageBreaks) <<
      endl;

  idtr--;
  
  // staves
  // --------------------------------------
  cerr <<
    idtr << "Staves:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "modernTab" << " : " <<
      booleanAsString (fModernTab) <<
      endl;

  idtr--;
  
  // tuplets
  // --------------------------------------

  cerr <<
    idtr << "Tuplets:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "tupletsOnALine" << " : " <<
      booleanAsString (fTupletsOnALine) <<
      endl;
    
  idtr--;

  // repeats
  // --------------------------------------

  cerr <<
    idtr << "Repeats:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "repeatBrackets" << " : " <<
      booleanAsString (fRepeatBrackets) <<
      endl;
    
  idtr--;

  // ornaments
  // --------------------------------------

  cerr <<
    idtr << "Ornaments:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "fDelayedOrnamentsFraction" << " : " <<
      fDelayedOrnamentsFraction <<
      endl;

  idtr--;

  // fonts
  // --------------------------------------

  cerr <<
    idtr << "Fonts:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "jazzFonts" << " : " <<
      booleanAsString (fJazzFonts) <<
      endl;

  idtr--;

  // code generation
  // --------------------------------------

  cerr <<
    idtr << "LilyPond code generation:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "comments" << " : " <<
      booleanAsString (fComments) <<
      endl <<

    idtr << setw (fieldWidth) << "global" << " : " <<
      booleanAsString (fGlobal) <<
      endl <<
    
    idtr << setw (fieldWidth) << "noLilypondCode" << " : " <<
      booleanAsString (fNoLilypondCode) <<
      endl <<

    idtr << setw (fieldWidth) << "noLilypondLyrics" << " : " <<
      booleanAsString (fNoLilypondLyrics) <<
      endl <<

    idtr << setw (fieldWidth) << "lilypondCompileDate" << " : " <<
      booleanAsString (fLilypondCompileDate) <<
      endl;

  idtr--;

  // score notation
  // --------------------------------------
    
  cerr <<
    idtr << "Score notation:" <<
    endl;

  idtr++;

/*
  cerr <<
    idtr << setw (fieldWidth) << "scoreNotationKind" << " : " <<
      scoreNotationKindAsString (
        fScoreNotationKind) <<
      endl;
*/
  idtr--;
  
  // midi
  // --------------------------------------
  cerr <<
    idtr << "Midi:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw (fieldWidth) << "midiTempoDuration" << " : " <<
 //     fMidiTempoDuration <<
      endl <<

    idtr << setw (fieldWidth) << "midiTempoPerSecond" << " : " <<
  //    fMidiTempoPerSecond <<
      endl <<

    idtr << setw (fieldWidth) << "noMidiCommand" << " : " <<
      booleanAsString (fNoMidi) <<
      endl;

  idtr--;
  

  idtr--;
}

ostream& operator<< (ostream& os, const S_lilypondOptions& elt)
{
  elt->print (os);
  return os;
}


}

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

#include "lpsrOptions.h"


using namespace std;

namespace MusicXML2 
{

// useful shortcut macros
#define idtr indenter::gIndenter
#define tab  indenter::gIndenter.getSpacer ()

//_______________________________________________________________________________
S_lpsrOptions gLpsrOptions;
S_lpsrOptions gLpsrOptionsUserChoices;
S_lpsrOptions gLpsrOptionsWithDetailedTrace;

S_lpsrOptions lpsrOptions::create ()
{
  lpsrOptions* o = new lpsrOptions();
  assert(o!=0);
  return o;
}

lpsrOptions::lpsrOptions()
  : msrOptionsGroup (
    "hlpsr", "helpLPSR",
R"(LPSR)"
    )
{
  initializeLpsrOptions (false);
}

lpsrOptions::~lpsrOptions()
{}

void lpsrOptions::initializeLpsrOptions (
  bool boolOptionsInitialValue)
{

/*
  LPSR:
  ----

    Trace and display:

      --tlpsr, --traceLpsr
            Write a trace of the LPSR graphs visiting activity to standard error.

      --ttlpvisits, --traceLpsrVisitors
            Write a trace of the LPSR graphs visiting activity to standard error.

      --lpsr, --displayLpsr
            Write the contents of the LPSR data to standard error.

      --tscheme, --traceSchemeFunctions
            Write a trace of the activity regarding Scheme functions to standard error.

    Languages:

      --lppl, --lpsrPitchesLanguage language
            Use 'language' to display note pitches in the LPSR logs and views,
            as well as in the generated LilyPond code.
            The 12 LilyPond pitches languages are available:
            nederlands, catalan, deutsch, english, espanol, français, 
            italiano, norsk, portugues, suomi, svenska and vlaams.
            The default is to use 'nederlands'.

      --lpcl , --lpsrChordsLanguage language
            Use 'language' to display chord names, their root and bass notes,
            in the LPSR logs and views and the generated LilyPond code.
            The 4 LilyPond chords languages are available:
            german, semiGerman, italian and french.
            The default used by LilyPond is Ignatzek's jazz-like, english naming.
 */




  // trace and display
  // --------------------------------------
  
  // variables
  fTraceLpsr         = boolOptionsInitialValue;
  
  fTraceLpsrVisitors = boolOptionsInitialValue;

  fDisplayLpsr       = boolOptionsInitialValue;
  
  // options

  // Scheme functions
   // --------------------------------------

  // variables
 fTraceSchemeFunctions = boolOptionsInitialValue;

  // options

  // languages
  // --------------------------------------
  
  // variables
  if (! setLpsrQuarterTonesPitchesLanguage ("nederlands")) {
    stringstream s;

    s <<
      "INTERNAL INITIALIZATION ERROR: "
      "LPSR pitches language 'nederlands' is unknown" <<
      endl <<
      "The " <<
      gQuarterTonesPitchesLanguagesMap.size () <<
      " known LPSR pitches languages are:" <<
      endl;

    idtr++;
  
    s <<
      existingQuarterTonesPitchesLanguages ();

    idtr--;

    optionError (s.str());
  }

  // options
}

S_lpsrOptions lpsrOptions::createCloneWithDetailedTrace ()
{
  S_lpsrOptions
    clone =
      lpsrOptions::create ();

  // trace and display
  // --------------------------------------
    
  clone->fTraceLpsr =
    true;

  clone->fTraceLpsrVisitors =
    true;

  clone->fDisplayLpsr =
    true;
    
  // Scheme functions
  clone->fTraceSchemeFunctions =
    true;
  
  // languages
  // --------------------------------------

  clone->fLpsrQuarterTonesPitchesLanguage =
    fLpsrQuarterTonesPitchesLanguage;

  clone->fLpsrChordsLanguage =
    fLpsrChordsLanguage;

  return clone;
}

bool lpsrOptions::setLpsrQuarterTonesPitchesLanguage (string language)
{
  // is language in the note names languages map?
  map<string, msrQuarterTonesPitchesLanguage>::const_iterator
    it =
      gQuarterTonesPitchesLanguagesMap.find (language);
        
  if (it == gQuarterTonesPitchesLanguagesMap.end ()) {
    // no, language is unknown in the map    
    return false;
  }

  fLpsrQuarterTonesPitchesLanguage = (*it).second;
  
  return true;
}

bool lpsrOptions::setLpsrChordsLanguage (string language)
{
  // is language in the chords languages map?
  map<string, lpsrChordsLanguage>::const_iterator
    it =
      gLpsrChordsLanguagesMap.find (language);
        
  if (it == gLpsrChordsLanguagesMap.end ()) {
    // no, language is unknown in the map    
    return false;
  }

  fLpsrChordsLanguage = (*it).second;
  
  return true;
}

void lpsrOptions::printLpsrOptionsHelp ()
{
  idtr++;

  cerr <<
    idtr << "LPSR:" <<
    endl <<
    idtr << "----" <<
    endl <<
    endl;

  idtr++;

  // trace and display
  // --------------------------------------
  cerr <<
    idtr << "Trace and display:" <<
    endl <<
    endl;

  idtr++;

  cerr <<
    idtr <<
      "--" _TRACE_LPSR_SHORT_NAME_ ", --" _TRACE_LPSR_LONG_NAME_ <<
      endl <<
    idtr << tab << tab << tab <<
      "Write a trace of the LPSR graphs visiting activity to standard error." <<
      endl <<
    endl <<
      
    idtr <<
      "--t" _TRACE_LPSR_VISITORS_SHORT_NAME_ ", --" _TRACE_LPSR_VISITORS_LONG_NAME_ <<
      endl <<
    idtr << tab << tab << tab <<
      "Write a trace of the LPSR graphs visiting activity to standard error." <<
      endl <<
    endl;

  cerr <<
    idtr <<
      "--" _DISPLAY_LPSR_SHORT_NAME_ ", --" _DISPLAY_LPSR_LONG_NAME_ << 
      endl <<
    idtr << tab << tab << tab <<
      "Write the contents of the LPSR data to standard error." << 
      endl <<
    endl;

  cerr <<
    idtr <<
      "--" _TRACE_SCHEME_FUNCTIONS_SHORT_NAME_ ", --" _TRACE_SCHEME_FUNCTIONS_LONG_NAME_ << 
      endl <<
    idtr << tab << tab << tab <<
      "Write a trace of the activity regarding Scheme functions to standard error." << 
      endl <<
    endl;

  idtr--;

  // languages
  // --------------------------------------
  cerr <<
    idtr << "Languages:" <<
    endl <<
    endl;

  idtr++;

  cerr <<
   idtr <<
      "--" _LPSR_PITCHES_LANGUAGE_SHORT_NAME_ ", --" _LPSR_PITCHES_LANGUAGE_LONG_NAME_ " language" << 
      endl <<
    idtr << tab << tab << tab <<
      "Use 'language' to display note pitches in the LPSR logs and views," <<
      endl <<
    idtr << tab << tab << tab <<
      "as well as in the generated LilyPond code." << 
      endl <<
    idtr << tab << tab << tab <<
      "The 12 LilyPond pitches languages are available:" << 
      endl <<
    idtr << tab << tab << tab <<
      "nederlands, catalan, deutsch, english, espanol, français, " << 
      endl <<
    idtr << tab << tab << tab <<
      "italiano, norsk, portugues, suomi, svenska and vlaams." << 
      endl <<
    idtr << tab << tab << tab <<
      "The default is to use 'nederlands'." << 
      endl <<
    endl;

  cerr <<
    idtr <<
      "--" _LPSR_CHORDS_LANGUAGE_SHORT_NAME_ " , --" _LPSR_CHORDS_LANGUAGE_LONG_NAME_ " language" << 
      endl <<
    idtr << tab << tab << tab <<
      "Use 'language' to display chord names, their root and bass notes," << 
      endl <<
    idtr << tab << tab << tab <<
      "in the LPSR logs and views and the generated LilyPond code." << 
      endl <<
    idtr << tab << tab << tab <<
      "The 4 LilyPond chords languages are available:" << 
      endl <<
    idtr << tab << tab << tab <<
      "german, semiGerman, italian and french." << 
      endl <<
    idtr << tab << tab << tab <<
      "The default used by LilyPond is Ignatzek's jazz-like, english naming." << 
      endl <<
    endl;

  idtr--;
  
  idtr--;
    
  
  idtr--;
}

void lpsrOptions::printLpsrOptionsValues (int fieldWidth)
{
  cerr << idtr <<
    "The LPSR options are:" <<
    endl;

  idtr++;
  
  // trace and display
  // --------------------------------------
  cerr <<
    idtr << "Trace and display:" <<
    endl;

  idtr++;
  
  cerr << left <<
    idtr <<
      setw(fieldWidth) << "traceLpsr" << " : " <<
      booleanAsString (fTraceLpsr) <<
      endl <<
    
    idtr <<
      setw(fieldWidth) << "traceLpsrVisitors" << " : " <<
      booleanAsString (fTraceLpsrVisitors) <<
      endl <<

    idtr << setw(fieldWidth) << "displayLpsr" << " : " <<
      booleanAsString (fDisplayLpsr) <<
      endl <<
      
    // Scheme functions
    idtr <<
      setw(fieldWidth) << "traceSchemeFunctions" << " : " <<
      booleanAsString (fTraceSchemeFunctions) <<
      endl;

  idtr--;
  
  // languages
  // --------------------------------------
  cerr <<
    idtr << "Languages:" <<
    endl;

  idtr++;

  cerr <<
    idtr << setw(fieldWidth) << "lpsrQuarterTonesPitchesLanguage" << " : \"" <<
      msrQuarterTonesPitchesLanguageAsString (
        fLpsrQuarterTonesPitchesLanguage) <<
        "\"" <<
      endl <<

    idtr << setw(fieldWidth) << "lpsrChordsLanguage" << " : \"" <<
      lpsrChordsLanguageAsString (
        fLpsrChordsLanguage) <<
        "\"" <<
      endl;  

  idtr--;

  
  idtr--;
}


}

/*
  MusicXML Library
  Copyright (C) Grame 2006-2013

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr
*/

#include <iomanip>      // setw, setprecision, ...

#include "msrMutualDependencies.h"

#include "musicXMLOptions.h"
#include "msrOptions.h"

using namespace std;

namespace MusicXML2 
{

//______________________________________________________________________________ 
int msrPart::gPartsCounter = 0;

S_msrPart msrPart::create (
  int            inputLineNumber,
  string         partID,
  S_msrPartGroup partPartGroupUplink)
{
  msrPart* o =
    new msrPart (
      inputLineNumber,
      partID,
      partPartGroupUplink);
  assert(o!=0);
  return o;
}

msrPart::msrPart (
  int            inputLineNumber,
  string         partID,
  S_msrPartGroup partPartGroupUplink)
    : msrPartGroupElement (inputLineNumber)
{
  // replace spaces in partID to set fPartID
  for_each (
    partID.begin (),
    partID.end (),
    stringSpaceReplacer (fPartID, '_'));

/* JMI
  // sanity check
  msrAssert(
    partPartGroupUplink != nullptr,
    "partPartGroupUplink is null");
    */

  // set part number
  fPartAbsoluteNumber = ++gPartsCounter;
  
  // set part's part group uplink
  fPartPartGroupUplink = partPartGroupUplink;

  // do other initializations
  initializePart ();
}

void msrPart::initializePart ()
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Creating part \"" << getPartCombinedName () << "\"" <<
      endl;
  }
#endif
  
  // is this part name in the part renaming map?
  map<string, string>::const_iterator
    it =
      gMsrOptions->fPartsRenamingMap.find (fPartID);
        
  if (it != gMsrOptions->fPartsRenamingMap.end ()) {
    // yes, rename the part accordinglingly
    fPartMsrName = (*it).second;
  }
  else {
    // coin the name from the argument
    fPartMsrName =
      "Part_"+stringNumbersToEnglishWords (fPartID);
  }
  
  // initialize part's number of measures
  fPartNumberOfMeasures = 0;
  
  // initialize part measure length high tide
  setPartMeasureLengthHighTide (
    fInputLineNumber,
    rational (0, 1));
}

msrPart::~msrPart ()
{}

S_msrPart msrPart::createPartNewbornClone (S_msrPartGroup partGroupClone)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Creating a newborn clone of part " <<
      getPartCombinedName () <<
      endl;
  }
#endif

  // sanity check
  msrAssert(
    partGroupClone != nullptr,
    "partGroupClone is null");
    
  S_msrPart
    newbornClone =
      msrPart::create (
        fInputLineNumber,
        fPartID,
        partGroupClone);

  newbornClone->fPartMsrName =
    fPartMsrName;
    
  newbornClone->fPartName =
    fPartName;
  newbornClone->fPartNameDisplayText =
    fPartNameDisplayText;

  newbornClone->fPartAbbreviation =
    fPartAbbreviation;
  newbornClone->fPartAbbreviationDisplayText =
    fPartAbbreviationDisplayText;
  
  newbornClone->fPartInstrumentName =
    fPartInstrumentName;
  newbornClone->fPartInstrumentAbbreviation =
    fPartInstrumentAbbreviation;

  return newbornClone;
}

/* JMI
void msrPart::setPartInstrumentAbbreviation (
  string partInstrumentAbbreviation)
    {
      fPartInstrumentAbbreviation =
        partInstrumentAbbreviation;
    }
*/

void msrPart::createPartFiguredBassStaffAndVoiceIfNotYetDone (
  int inputLineNumber)
{
  if (! fPartFiguredBassStaff) {    
    // create the part figured bass staff
#ifdef TRACE_OPTIONS
    if (
      gGeneralOptions->fTraceParts
        ||
      gGeneralOptions->fTraceFiguredBass
        ||
      gGeneralOptions->fTraceStaves
        ||
      gGeneralOptions->fTraceVoices) {
      gLogIOstream <<
        "Creating the figured bass staff" <<
        " with number " << K_PART_FIGURED_BASS_STAFF_NUMBER <<
        " for part " <<
        getPartCombinedName () <<
        ", line " << inputLineNumber <<
        endl;
    }
#endif

    gIndenter++;

    fPartFiguredBassStaff =
      addStaffToPartByItsNumber (
        inputLineNumber,
        msrStaff::kStaffFiguredBass,
        K_PART_FIGURED_BASS_STAFF_NUMBER);
      
    // create the part figured bass voice  
#ifdef TRACE_OPTIONS
    if (gGeneralOptions->fTraceHarmonies || gGeneralOptions->fTraceVoices) {
      gLogIOstream <<
        "Creating figured bass voice " <<
        " number " << K_PART_FIGURED_BASS_VOICE_NUMBER <<
        " for part " <<
        getPartCombinedName () <<
        ", line " << inputLineNumber <<
        endl;
    }
#endif

    // create the figured bass voice
    fPartFiguredBassVoice =
      msrVoice::create (
        inputLineNumber,
        msrVoice::kFiguredBassVoice,
        K_PART_FIGURED_BASS_VOICE_NUMBER,
        msrVoice::kCreateInitialLastSegmentYes,
        fPartFiguredBassStaff);

    // register it in figured bass staff
    fPartFiguredBassStaff->
      registerVoiceInStaff (
        inputLineNumber,
        fPartFiguredBassVoice );

    if (fPartCurrentTime) {
      // append part current time to figured bass voice
      fPartFiguredBassVoice->
        appendTimeToVoice (
          fPartCurrentTime);
    }

    gIndenter--;
  }

/* JMI
  gLogIOstream <<
    endl <<
    "***********" <<
    endl <<
    endl;
  print (gLogIOstream);
  gLogIOstream <<
    "***********" <<
    endl <<
    endl;
  */  
}

void msrPart::addAVoiceToStavesThatHaveNone (
  int inputLineNumber)
{
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      addAVoiceToStaffIfItHasNone (
        inputLineNumber);
  } // for
}

void msrPart::setPartMeasureLengthHighTide (
  int      inputLineNumber,
  rational measureLength)
{
#ifdef TRACE_OPTIONS
  if (gMusicXMLOptions->fTraceDivisions || gGeneralOptions->fTraceMeasures) {
    gLogIOstream <<
      "Setting measure length high tide for part \"" <<
      getPartCombinedName () <<
      "\" to " << measureLength <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  fPartMeasureLengthHighTide = measureLength;
}

void msrPart::updatePartMeasureLengthHighTide (
  int      inputLineNumber,
  rational measureLength)
{
  if (measureLength > fPartMeasureLengthHighTide) {
#ifdef TRACE_OPTIONS
    if (gMusicXMLOptions->fTraceDivisions || gGeneralOptions->fTraceMeasures) {
      gLogIOstream <<
        "Updating measure length high tide for part \"" <<
        getPartCombinedName () <<
        "\" to " << measureLength <<
        ", line " << inputLineNumber <<
        endl;
    }
#endif

    fPartMeasureLengthHighTide = measureLength;
  }
}

void msrPart::padUpToMeasureLengthInPart (
  int      inputLineNumber,
  rational measureLength)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceParts || gGeneralOptions->fTraceMeasures) {
    gLogIOstream <<
      "Padding up to measure length '" << measureLength <<
      "' in part \"" <<
      getPartCombinedName () <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;
  
  // pad the registered staves up to measure length  
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      padUpToMeasureLengthInStaff (
        inputLineNumber,
        measureLength);
  } // for

  gIndenter--;
}

void msrPart::setPartMsrName (string partMsrName)
{
  // is this part name in the part renaming map?
  map<string, string>::const_iterator
    it =
      gMsrOptions->fPartsRenamingMap.find (fPartMsrName);
        
  if (it != gMsrOptions->fPartsRenamingMap.end ()) {
    // yes, rename the part accordinglingly
    fPartMsrName = (*it).second;

#ifdef TRACE_OPTIONS
    if (gGeneralOptions->fTraceParts) {
      gLogIOstream <<
        "Setting part name of " << getPartCombinedName () <<
        " to \"" << fPartMsrName << "\"" <<
         endl;
    }
#endif
  }
  else {
    // use the argument
    fPartMsrName = partMsrName;

#ifdef TRACE_OPTIONS
    if (gGeneralOptions->fTraceParts) {
      gLogIOstream <<
        "Keeping partID \"" << partMsrName <<
        "\" as part name  for " << getPartCombinedName () <<
      endl;
    }
#endif
  }
}

string msrPart::getPartCombinedName () const
{
  stringstream s;

  if (! fPartMsrName.size ()) {
    s <<
      "[empty name]";
  }
  else {
    s <<
      "\"" << fPartMsrName << "\"";
  }

  s <<
    " (partID \"" << fPartID << "\"";

  if (fPartName.size ()) {
    s <<
      ", partName \"" << fPartName << "\"";
  }

  s <<
    ")";

  return s.str ();
}

void msrPart::createMeasureAndAppendItToPart (
  int    inputLineNumber,
  string measureNumber,
  int    measureOrdinalNumber,
  msrMeasure::msrMeasureImplicitKind
         measureImplicitKind)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceMeasures) {
    gLogIOstream <<
      "Creating and appending measure '" <<
      measureNumber <<
      "' to part " <<
      getPartCombinedName () <<
      "', line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;
  
  // set part current measure number
  fPartCurrentMeasureNumber = measureNumber;

  // create and append measure to registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;
      
    staff->
      createMeasureAndAppendItToStaff (
        inputLineNumber,
        measureNumber,
        measureOrdinalNumber,
        measureImplicitKind);
  } // for

  gIndenter--;
}

void msrPart::setNextMeasureNumberInPart (
  int    inputLineNumber,
  string nextMeasureNumber)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceMeasures) {
    gLogIOstream <<
      "Setting next measure number to '" <<
      nextMeasureNumber <<
      "' in part " <<
      getPartCombinedName () <<
      "', line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;
  
  // set next measure number in registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;
      
    staff->
      setNextMeasureNumberInStaff (
        inputLineNumber,
        nextMeasureNumber);
  } // for

  gIndenter--;
}

/* JMI
void msrPart::complementPartVoicesUpToMeasure (
  int    inputLineNumber,
  string measureNumber)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceMeasures) {
    gLogIOstream <<
      "Complement part voices up to measure number " << measureNumber <<
      ", line " << inputLineNumber <<
      ", in part " << getPartCombinedName () <<
      endl;
  }
#endif

  // propagate to all staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;
      / * JMI
    staff->
      createMeasureAndAppendItToStaff (
        inputLineNumber,
        measureNumber,
        measureOrdinalNumber,
        measureImplicitKind);
  } // for
}
*/

void msrPart::appendStaffDetailsToPart (
  S_msrStaffDetails staffDetails)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceStaves || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending staff details\"" <<
      staffDetails->asShortString () <<
      "\" to part " << getPartCombinedName () <<
    endl;
  }
#endif

  // register staff details in part
  fCurrentPartStaffDetails = staffDetails;

  // append staff details to registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;

    staff->
      appendStaffDetailsToStaff (
        staffDetails);
  } // for
}

void msrPart::appendClefToPart (S_msrClef clef)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceParts || gGeneralOptions->fTraceClefs) {
    gLogIOstream <<
      "Appending clef '" <<
      clef->asString () <<
      "' to part " << getPartCombinedName () <<
    endl;
  }
#endif

  gIndenter++;
  
  // append clef to registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      appendClefToStaff (
        clef);
  } // for

  gIndenter--;
}

void msrPart::appendKeyToPart  (S_msrKey  key)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceKeys || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending key '" <<
      key->asString () <<
      "' to part " << getPartCombinedName () <<
      endl;
  }
#endif
  
  // set part key
  fPartCurrentKey = key;

  gIndenter++;
  
  // append key to registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;

    staff->
      appendKeyToStaff (
        key);
  } // for

  gIndenter--;
}

void msrPart::appendTimeToPart (S_msrTime time)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceTimes || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending time '" <<
      time->asString () <<
      "' to part " << getPartCombinedName () <<
      endl;
  }
#endif
  
  // set part time
  fPartCurrentTime = time;

  gIndenter++;
  
  // append time to registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;

    staff->
      appendTimeToStaff (
        time);
  } // for

  gIndenter--;
}

void msrPart::appendTimeToPartClone (S_msrTime time)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceTimes || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending time '" <<
      time->asString () <<
      "' to part clone " << getPartCombinedName () <<
      endl;
  }
#endif
  
  // set part time
  fPartCurrentTime = time;

  gIndenter++;
  
  // propagate it to all staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;

    staff->
      appendTimeToStaffClone (time);
  } // for

  gIndenter--;
}

void msrPart::appendTransposeToPart (
  S_msrTranspose transpose)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceTranspositions || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending transpose \"" <<
      transpose->asString () <<
      "\" to part " << getPartCombinedName () <<
    endl;
  }
#endif

  // set part current transpose
  fPartCurrentTranspose = transpose;

  // propagate it to all staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;

    staff->
      appendTransposeToStaff (transpose);
  } // for
}

void msrPart::appendPartNameDisplayToPart (
  S_msrPartNameDisplay partNameDisplay)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceTranspositions || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending part name display \"" <<
      partNameDisplay->asString () <<
      "\" to part " << getPartCombinedName () <<
    endl;
  }
#endif

  // set part current transpose
// JMI ???  fPartCurrentTranspose = partNameDisplay;

  // propagate it to all staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;

    staff->
      appendPartNameDisplayToStaff (partNameDisplay);
  } // for
}

void msrPart::appendPartAbbreviationDisplayToPart (
  S_msrPartAbbreviationDisplay partAbbreviationDisplay)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceTranspositions || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending part abbreviation display \"" <<
      partAbbreviationDisplay->asString () <<
      "\" to part " << getPartCombinedName () <<
    endl;
  }
#endif

  // set part current transpose
// JMI ???  fPartCurrentTranspose = partAbbreviationDisplay;

  // propagate it to all staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff = (*i).second;

    staff->
      appendPartAbbreviationDisplayToStaff (partAbbreviationDisplay);
  } // for
}

void msrPart::nestContentsIntoNewRepeatInPart (
  int inputLineNumber)
{
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      nestContentsIntoNewRepeatInStaff (
        inputLineNumber);
  } // for
}

/* JMI
    void                      createAndPrependImplicitBarLine (
                                int inputLineNumber);

void mxmlTree2MsrTranslator::createAndPrependImplicitBarLine (
  int inputLineNumber)
{     
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceBarlines || gGeneralOptions->fTraceRepeats) {
    fLogOutputStream <<
      "Prepending an implicit repeat start barline at the beginning of part" <<
      fCurrentPart->getPartCombinedName () <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  // fetch current voice
  S_msrVoice
    currentVoice =
      fetchVoiceFromPart (
        inputLineNumber,
        fCurrentMusicXMLStaffNumber,
        fCurrentMusicXMLVoiceNumber);

  // create the implicit barline
  S_msrBarline
    implicitBarline =
      msrBarline::create (
        inputLineNumber,
        msrBarline::kBarlineCategoryRepeatStart,
        msrBarline::kBarlineHasSegnoNo,
        msrBarline::kBarlineHasCodaNo,
        msrBarline::kBarlineLocationLeft,
        msrBarline::kBarlineStyleHeavyLight,
        msrBarline::kBarlineEndingTypeStart,
        fCurrentBarlineEndingNumber,
        msrBarline::kBarlineRepeatDirectionForward,
        fCurrentBarlineRepeatWingedKind,
        fCurrentBarlineTimes);

  // prepend the implicit barline to the voice
  gIndenter++;
  
  currentVoice->
    prependBarlineToVoice (implicitBarline);

  gIndenter--;
}
 */
 
/* remove the whole method ??? JMI
  // create the enclosing repeat and append it to the part
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceRepeats) {
    fLogOutputStream <<
      "Creating a regular repeat in part " <<
      fCurrentPart->getPartCombinedName () <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  fCurrentPart->
    createRegularRepeatFirstEndingInPart (
      inputLineNumber,
      barline->getBarlineTimes ());
*/

/*
  else {
    // no, there is an implicit repeat starting at the beginning of the part,
    // that encloses everything from the beginning on
*/

/*
    // append an implicit repeat to the current part
#ifdef TRACE_OPTIONS
    if (gGeneralOptions->fTraceRepeats) {
      fLogOutputStream <<
        "Prepending an implicit barline ahead of part " <<
        fCurrentPart->getPartCombinedName () <<
        ", line " << inputLineNumber <<
        endl;
    }
#endif

    createAndPrependImplicitBarLine (
      inputLineNumber);
}
*/

void msrPart::handleRepeatStartInPart (
  int inputLineNumber)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceRepeats) {
    gLogIOstream <<
      "Handling a repeat start in part \"" <<
      getPartCombinedName () <<
      "\", line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;

  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      handleRepeatStartInStaff (
        inputLineNumber);
  } // for

  gIndenter--;
}

void msrPart::handleRepeatEndInPart (
  int    inputLineNumber,
  string measureNumber,
  int    repeatTimes)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceRepeats) {
    gLogIOstream <<
      "Handling a repeat end in part \"" <<
      getPartCombinedName () <<
      "\"" <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;
  
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      handleRepeatEndInStaff (
        inputLineNumber,
        measureNumber,
        repeatTimes);
  } // for

  gIndenter--;
}

void msrPart::handleRepeatEndingStartInPart (
  int    inputLineNumber)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceRepeats) {
    gLogIOstream <<
      "Handling a repeat ending start in part \"" <<
      getPartCombinedName () <<
      "\"" <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;
  
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      handleRepeatEndingStartInStaff (
        inputLineNumber);
  } // for

  gIndenter--;
}

void msrPart::handleRepeatEndingEndInPart (
  int       inputLineNumber,
  string    repeatEndingNumber, // may be "1, 2"
  msrRepeatEnding::msrRepeatEndingKind
            repeatEndingKind)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceRepeats) {
    gLogIOstream <<
      "Handling a " <<
      msrRepeatEnding::repeatEndingKindAsString (
        repeatEndingKind) <<
      " repeat ending end in part \"" <<
      getPartCombinedName () <<
      "\"" <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;
  
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      handleRepeatEndingEndInStaff (
        inputLineNumber,
        repeatEndingNumber,
        repeatEndingKind);
  } // for

  gIndenter--;
}

void msrPart::finalizeRepeatEndInPart (
  int    inputLineNumber,
  string measureNumber,
  int    repeatTimes)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceRepeats) {
    gLogIOstream <<
      "Finalizing a repeat upon its end in part \"" <<
      getPartCombinedName () <<
      "\"" <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;
  
  // create repeat and append it to registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      finalizeRepeatEndInStaff (
        inputLineNumber,
        measureNumber,
        repeatTimes);
  } // for

  gIndenter--;
}

void msrPart::appendRepeatCloneToPart (
  int         inputLineNumber,
  S_msrRepeat repeatCLone)
{
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff =
        (*i).second;

    switch (staff->getStaffKind ()) {
      case msrStaff::kStaffRegular:
        staff->appendRepeatCloneToStaff (
          inputLineNumber, repeatCLone);
        break;
        
      case msrStaff::kStaffTablature:
        break;
        
      case msrStaff::kStaffHarmony:
        break;
        
      case msrStaff::kStaffFiguredBass:
        break;

      case msrStaff::kStaffDrum:
        staff->appendRepeatCloneToStaff (
          inputLineNumber, repeatCLone);
        break;

      case msrStaff::kStaffRythmic:
        staff->appendRepeatCloneToStaff (
          inputLineNumber, repeatCLone);
        break;
    } // switch
  } // for
}

void msrPart::appendRepeatEndingCloneToPart (
  S_msrRepeatEnding repeatEndingCLone)
{
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      appendRepeatEndingCloneToStaff (repeatEndingCLone);
  } // for
}

void msrPart::createMeasuresRepeatFromItsFirstMeasuresInPart (
  int inputLineNumber,
  int measuresRepeatMeasuresNumber,
  int measuresRepeatSlashes)
{
  // create measure repeat from its first measure in registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      createMeasuresRepeatFromItsFirstMeasuresInStaff (
        inputLineNumber,
        measuresRepeatMeasuresNumber,
        measuresRepeatSlashes);
  } // for
}

void msrPart::appendPendingMeasuresRepeatToPart (
  int inputLineNumber)
{
  // append pending measure repeat to registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      appendPendingMeasuresRepeatToStaff (
        inputLineNumber);
  } // for
}

void msrPart::createMultipleRestInPart (
  int inputLineNumber,
  int multipleRestMeasuresNumber)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceRepeats || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Creating a multiple rest in part " <<
      getPartCombinedName () <<
      ", " <<
      singularOrPlural (
        multipleRestMeasuresNumber, "measure", "measures") <<
      endl;
  }
#endif

  // create multiple rest in registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      createMultipleRestInStaff (
        inputLineNumber,
        multipleRestMeasuresNumber);
  } // for
}

void msrPart::appendPendingMultipleRestToPart (
  int inputLineNumber)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceRepeats || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending the pending multiple rest to part " <<
      getPartCombinedName () <<
      endl;
  }
#endif

  // append pending multiple rest to registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      appendPendingMultipleRestToStaff (
        inputLineNumber);
  } // for
}

void msrPart::appendMultipleRestCloneToPart (
  int               inputLineNumber,
  S_msrMultipleRest multipleRest)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceStaves || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending multiple rest '" <<
      multipleRest->asString () <<
      "' to part clone " <<
      getPartCombinedName () <<
      endl;
  }
#endif

  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      appendMultipleRestCloneToStaff (
        inputLineNumber,
        multipleRest);
  } // for
}

void msrPart::appendBarlineToPart (S_msrBarline barline)
{
  // append barline to registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      appendBarlineToStaff (barline);
  } // for
}
     
S_msrStaff msrPart::addStaffToPartByItsNumber (
  int                    inputLineNumber,
  msrStaff::msrStaffKind staffKind,
  int                    staffNumber)
{
  if (fPartStavesMap.count (staffNumber)) {
    stringstream s;
    
    s <<      
      "staffNumber " << staffNumber <<
      " already exists in part " << getPartCombinedName () <<
      ", line " << inputLineNumber;

    msrInternalError ( // JMI ???
      gGeneralOptions->fInputSourceName,
      inputLineNumber,
      __FILE__, __LINE__,
      s.str ());
      
    return fPartStavesMap [staffNumber];
  }

#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceStaves || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Adding " <<
      msrStaff::staffKindAsString (staffKind) <<
      " staff " << staffNumber <<
      " to part " << getPartCombinedName () <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif
  
  // create the staff
  S_msrStaff
    staff =
      msrStaff::create (
        inputLineNumber,
        staffKind,
        staffNumber,
        this);

  // register staff in this part if relevant
  switch (staffKind) {
    case msrStaff::kStaffRegular:
    case msrStaff::kStaffTablature:
    case msrStaff::kStaffHarmony:
    case msrStaff::kStaffFiguredBass:
    case msrStaff::kStaffDrum:
    case msrStaff::kStaffRythmic:
      fPartStavesMap [staffNumber] = staff;
      break;
  } // switch
  
  // initialize staff current clef and key if relevant // JMI
  switch (staffKind) {
    case msrStaff::kStaffRegular:
    case msrStaff::kStaffTablature:
    case msrStaff::kStaffHarmony:
    case msrStaff::kStaffFiguredBass:
    case msrStaff::kStaffDrum:
    case msrStaff::kStaffRythmic:
      staff->setStaffCurrentClef (fPartCurrentClef);
      staff->setStaffCurrentKey (fPartCurrentKey);
      break;
  } // switch
  
  // return staff
  return staff;
}

void msrPart::addStaffToPartCloneByItsNumber (S_msrStaff staff)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceStaves || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Adding staff \"" << staff->getStaffName () <<
      "\" to part clone " << getPartCombinedName () <<
      endl;
  }
#endif

  // register staff in this part
  fPartStavesMap [staff->getStaffNumber ()] = staff;
}

S_msrStaff msrPart::fetchStaffFromPart (
  int staffNumber)
{
  S_msrStaff result;
  
  if (fPartStavesMap.count (staffNumber)) {
    result = fPartStavesMap [staffNumber];
  }

  return result;
}

void msrPart::appendFiguredBassToPart (
  S_msrVoice       figuredBassSupplierVoice,
  S_msrFiguredBass figuredBass)
{
  int inputLineNumber =
    figuredBass->getInputLineNumber ();

  switch (figuredBassSupplierVoice->getVoiceKind ()) {
    case msrVoice::kRegularVoice:
      // create the figured bass staff and voice if not yet done
      createPartFiguredBassStaffAndVoiceIfNotYetDone (
        inputLineNumber);

      /* JMI ???
      // register this voice as the part figured bass supplier voice
      setPartFiguredBassSupplierVoice (
        inputLineNumber,
        figuredBassSupplierVoice);
    */
    
      // append the figured bass to the part figured bass voice
#ifdef TRACE_OPTIONS
      if (gGeneralOptions->fTraceHarmonies || gGeneralOptions->fTraceParts) {
        gLogIOstream <<
          "Appending figured bass '" <<
          figuredBass->asString () <<
          "' to part " <<
          getPartCombinedName () <<
          ", line " << inputLineNumber <<
          endl;
      }
#endif
    
      fPartFiguredBassVoice->
        appendFiguredBassToVoice (figuredBass);
      break;
      
    case msrVoice::kHarmonyVoice:
    case msrVoice::kFiguredBassVoice:
      {
        stringstream s;
    
        s <<
          "figured bass cannot by supplied to part by " <<
          msrVoice::voiceKindAsString (
            figuredBassSupplierVoice->getVoiceKind ()) <<
          " voice \" " <<
          figuredBassSupplierVoice->getVoiceName () <<
          "\"";
    
        msrInternalError (
          gGeneralOptions->fInputSourceName,
          inputLineNumber,
          __FILE__, __LINE__,
          s.str ());
      }
      break;
  } // switch
}

void msrPart::appendFiguredBassToPartClone (
  S_msrVoice       figuredBassSupplierVoice,
  S_msrFiguredBass figuredBass)
{
  int inputLineNumber =
    figuredBass->getInputLineNumber ();

  switch (figuredBassSupplierVoice->getVoiceKind ()) {
    case msrVoice::kFiguredBassVoice:
      // create the figured bass staff and voice if not yet done
      createPartFiguredBassStaffAndVoiceIfNotYetDone (
        inputLineNumber);

      /* JMI NON
      // register this voice as the part figuredBass supplier voice
      setPartfiguredBassSupplierVoice (
        inputLineNumber,
        figuredBassSupplierVoice);
        */
    
      // append the figured bass to the part figured bass voice
#ifdef TRACE_OPTIONS
      if (gGeneralOptions->fTraceFiguredBass || gGeneralOptions->fTraceParts) {
        gLogIOstream <<
          "Appending figured bass '" <<
          figuredBass->asString () <<
          "' to part clone " <<
          getPartCombinedName () <<
          ", line " << inputLineNumber <<
          endl;
      }
#endif
    
      fPartFiguredBassVoice->
        appendFiguredBassToVoiceClone (figuredBass);
      break;
      
    case msrVoice::kRegularVoice:
    case msrVoice::kHarmonyVoice:
      {
        stringstream s;
    
        s <<
          "figured bass cannot by supplied to part clone by " <<
          msrVoice::voiceKindAsString (
            figuredBassSupplierVoice->getVoiceKind ()) <<
          " voice \" " <<
          figuredBassSupplierVoice->getVoiceName () <<
          "\"";
    
        msrInternalError (
          gGeneralOptions->fInputSourceName,
          inputLineNumber,
          __FILE__, __LINE__,
          s.str ());
      }
      break;
  } // switch
}

void msrPart::appendScordaturaToPart (
  S_msrScordatura scordatura)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending scordatura '" <<
      scordatura->asString () <<
      "' to part " <<
      getPartCombinedName () <<
      endl;
  }
#endif

  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      appendScordaturaToStaff (
        scordatura);
  } // for
}

void msrPart::appendAccordionRegistrationToPart (
  S_msrAccordionRegistration
    accordionRegistration)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending accordion registration '" <<
      accordionRegistration->asString () <<
      "' to part " <<
      getPartCombinedName () <<
      endl;
  }
#endif

  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      appendAccordionRegistrationToStaff (
        accordionRegistration);
  } // for
}

void msrPart::appendHarpPedalsTuningToPart (
  S_msrHarpPedalsTuning
    harpPedalsTuning)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Appending harp pedals tuning '" <<
      harpPedalsTuning->asString () <<
      "' to part " <<
      getPartCombinedName () <<
      endl;
  }
#endif

  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      appendHarpPedalsTuningToStaff (
        harpPedalsTuning);
  } // for
}

void msrPart:: handleBackup (
  int inputLineNumber,
  int divisions,
  int divisionsPerQuarterNote)
{
#ifdef TRACE_OPTIONS
  if (
    gGeneralOptions->fTraceParts
      ||
    gMusicXMLOptions->fTraceDivisions
      ||
    gGeneralOptions->fTraceMeasures) {
    gLogIOstream <<
      "Handling backup, divisions = '" <<
      divisions <<
      "' in part " <<
      getPartCombinedName () <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  // compute the backup step length
  rational
    backupStepLength =
      rational (
        divisions,
        divisionsPerQuarterNote * 4); // hence a whole note    
  
  // determine the measure position 'divisions' backward
  rational
    positionInMeasure =
      fPartMeasureLengthHighTide - backupStepLength;

  positionInMeasure.rationalise ();

  // bring the part back to that measure position
  padUpToMeasureLengthInPart (
    inputLineNumber,
    positionInMeasure);
}

void msrPart::addSkipGraceNotesGroupBeforeAheadOfVoicesClonesIfNeeded (
  S_msrVoice           graceNotesGroupOriginVoice,
  S_msrGraceNotesGroup skipGraceNotesGroup)
{
  int inputLineNumber =
    skipGraceNotesGroup->getInputLineNumber ();

  rational
    graceNotesGroupOriginVoiceMeasureLength =
      graceNotesGroupOriginVoice->
        getVoiceLastSegment ()->
          getSegmentMeasuresList ().back ()->
            getMeasureLength ();
        
#ifdef TRACE_OPTIONS
  if (
    gGeneralOptions->fTraceMeasures
      ||
    gGeneralOptions->fTraceGraceNotes
      ||
    gGeneralOptions->fTraceParts
    ) {
    gLogIOstream <<
      "addSkipGraceNotesGroupBeforeAheadOfVoicesClonesIfNeeded () in " <<
      getPartCombinedName () <<
      ", graceNotesGroupOriginVoiceMeasureLength = " <<
      graceNotesGroupOriginVoiceMeasureLength <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  for (
    map<int, S_msrStaff>::const_iterator i=fPartStavesMap.begin ();
    i!=fPartStavesMap.end ();
    i++) {

    map<int, S_msrVoice>
      staffAllVoicesMap =
        (*i).second->
          getStaffAllVoicesMap ();
          
    for (
      map<int, S_msrVoice>::const_iterator j=staffAllVoicesMap.begin ();
      j!=staffAllVoicesMap.end ();
      j++
    ) {

      S_msrVoice voice = (*j).second;
      
      if (voice != graceNotesGroupOriginVoice) {        
        // add skip grace notes group ahead of voice
        voice->
          addGraceNotesGroupBeforeAheadOfVoiceIfNeeded (
            skipGraceNotesGroup);
      }
    } // for
  } // for
}

void msrPart::finalizeCurrentMeasureInPart (
  int    inputLineNumber)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceMeasures || gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Finalizing current measure in part " <<
      getPartCombinedName () <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;
  
  // finalize current measure in registered staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    (*i).second->
      finalizeCurrentMeasureInStaff (
        inputLineNumber);
  } // for

  // reset measure length high tide
  setPartMeasureLengthHighTide (
    fInputLineNumber,
    rational (0, 1));

  gIndenter--;
}

void msrPart::setPartInstrumentNamesMaxLengthes ()
{
  S_msrScore
    score =
      fPartPartGroupUplink->
        getPartGroupScoreUplink ();
        
  int partInstrumentNameLength =
    fPartInstrumentName.size ();
  
  if (
    partInstrumentNameLength
      >
    score->getScoreInstrumentNamesMaxLength ()) {
    score->
      setScoreInstrumentNamesMaxLength (
        partInstrumentNameLength);
  }
      
  int partInstrumentAbbreviationLength =
    fPartInstrumentAbbreviation.size ();
  
  if (
    partInstrumentAbbreviationLength
      >
    score->getScoreInstrumentAbbreviationsMaxLength ()) {
    score->
      setScoreInstrumentAbbreviationsMaxLength (
        partInstrumentAbbreviationLength);
  }
}

void msrPart::finalizePart (
  int inputLineNumber)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Finalizing part " <<
      getPartCombinedName () <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  gIndenter++;
  
  if (! fPartStavesMap.size ()) {
    stringstream s;

    s <<
      "Part " <<
      getPartCombinedName () <<
      " appears in the part list, but doesn't contain any stave";

    msrMusicXMLWarning (
      gGeneralOptions->fInputSourceName,
      inputLineNumber,
      s.str ());
  }
  
  else {
    // finalize registered staves
    for (
      map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
      i != fPartStavesMap.end ();
      i++) {
      (*i).second->
        finalizeStaff (
          inputLineNumber);
    } // for
  }

  // set score instrument names max lengthes if relevant
  setPartInstrumentNamesMaxLengthes ();

  gIndenter--;
}

void msrPart::finalizePartClone (
  int inputLineNumber)
{
#ifdef TRACE_OPTIONS
  if (gGeneralOptions->fTraceParts) {
    gLogIOstream <<
      "Finalizing part clone " <<
      getPartCombinedName () <<
      ", line " << inputLineNumber <<
      endl;
  }
#endif

  // set score instrument names max lengthes if relevant
  setPartInstrumentNamesMaxLengthes ();
}

void msrPart::acceptIn (basevisitor* v)
{
  if (gMsrOptions->fTraceMsrVisitors) {
    gLogIOstream <<
      "% ==> msrPart::acceptIn ()" <<
      endl;
  }
      
  if (visitor<S_msrPart>*
    p =
      dynamic_cast<visitor<S_msrPart>*> (v)) {
        S_msrPart elem = this;
        
        if (gMsrOptions->fTraceMsrVisitors) {
          gLogIOstream <<
            "% ==> Launching msrPart::visitStart ()" <<
            endl;
        }
        p->visitStart (elem);
  }
}

void msrPart::acceptOut (basevisitor* v)
{
  if (gMsrOptions->fTraceMsrVisitors) {
    gLogIOstream <<
      "% ==> msrPart::acceptOut ()" <<
      endl;
  }

  if (visitor<S_msrPart>*
    p =
      dynamic_cast<visitor<S_msrPart>*> (v)) {
        S_msrPart elem = this;
      
        if (gMsrOptions->fTraceMsrVisitors) {
          gLogIOstream <<
            "% ==> Launching msrPart::visitEnd ()" <<
            endl;
        }
        p->visitEnd (elem);
  }
}

void msrPart::browseData (basevisitor* v)
{
  if (gMsrOptions->fTraceMsrVisitors) {
    gLogIOstream <<
      "% ==> msrPart::browseData ()" <<
      endl;
  }

  // browse all non figured bass staves
  for (
    map<int, S_msrStaff>::const_iterator i = fPartStavesMap.begin ();
    i != fPartStavesMap.end ();
    i++) {
    S_msrStaff
      staff =
        (*i).second;

    if (staff != fPartFiguredBassStaff) {
      // browse the staff
      msrBrowser<msrStaff> browser (v);
      browser.browse (*staff);
    }
  } // for

  // browse the part figured bass only now if any,
  // to place it after the corresponding part
  if (fPartFiguredBassStaff) {
    msrBrowser<msrStaff> browser (v);
    browser.browse (*fPartFiguredBassStaff);
  }
      
}

string msrPart::asString () const
{
  stringstream s;

  s <<
    "Part \"" <<
    getPartName () <<
    "\", line " << fInputLineNumber;

  return s.str ();
}

void msrPart::print (ostream& os)
{
  os <<
    "Part" << " " << fPartMsrName <<
    " (" <<
    singularOrPlural (
      fPartStavesMap.size (), "staff", "staves") <<
    ")" <<
    ", line " << fInputLineNumber <<
    endl;
    
  gIndenter++;
  
  const int fieldWidth = 27;

  os << left <<
    setw (fieldWidth) <<
    "partGroupUplink" << " : ";
  if (fPartPartGroupUplink) {
    // it may be empty
    os <<
      fPartPartGroupUplink->getPartGroupCombinedName ();
  }
  else {
    os << "none";
  }
  os <<
    endl;
  
  os << left <<
    setw (fieldWidth) <<
    "partID" << " : \"" <<
    fPartID << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partMsrName" << " : \"" <<
    fPartMsrName << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partAbsoluteNumber" << " : " <<
    fPartAbsoluteNumber <<
    endl <<
    
    setw (fieldWidth) <<
    "partName" << " : \"" <<
    fPartName << "\"" <<
    endl <<
    setw (fieldWidth) <<
    "partNameDisplayText" << " : \"" <<
    fPartNameDisplayText << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partAbbrevation" << " : \"" <<
    fPartAbbreviation << "\"" <<
    endl <<
    setw (fieldWidth) <<
    "partAbbreviationDisplayText" << " : \"" <<
    fPartAbbreviationDisplayText << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partInstrumentName" << " : \"" <<
    fPartInstrumentName << "\"" <<
    endl <<    
    setw (fieldWidth) <<
    "partInstrumentAbbreviation" << " : \"" <<
    fPartInstrumentAbbreviation << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partNumberOfMeasures" << " : " <<
    fPartNumberOfMeasures <<
    endl <<
    endl;

  // print the registered staves
  if (fPartStavesMap.size ()) {
    gIndenter++;
    
    map<int, S_msrStaff>::const_iterator
      iBegin = fPartStavesMap.begin (),
      iEnd   = fPartStavesMap.end (),
      i      = iBegin;
          
    for ( ; ; ) {
      S_msrStaff
        staff =
          (*i).second;
          
      msrStaff::msrStaffKind
        staffKind =
          staff->getStaffKind ();

      switch (staffKind) { // JMI
        case msrStaff::kStaffRegular:
          os <<
            staff;
          break;
          
        case msrStaff::kStaffTablature:
          os <<
            staff;
          break;
          
        case msrStaff::kStaffHarmony:
    // JMI      if (gMsrOptions->fShowHarmonyVoices) {}
          os <<
            staff;
          break;
          
        case msrStaff::kStaffFiguredBass:
    // JMI      if (gMsrOptions->fShowFiguredBassVoices) {}
          os <<
            staff;
          break;

        case msrStaff::kStaffDrum:
          os <<
            staff;
          break;
          
        case msrStaff::kStaffRythmic:
          os <<
            staff;
          break;          
      } // switch
      
      if (++i == iEnd) break;
      
      os <<
        endl;
    } // for

    gIndenter--;
  }

  gIndenter--;
}

void msrPart::printSummary (ostream& os)
{
  os <<
    "Part" << " " << fPartMsrName <<
    " (" <<
    singularOrPlural (
      fPartStavesMap.size (), "staff", "staves") <<
    ", " <<
    singularOrPlural (
      fPartNumberOfMeasures, "measure", "measure") <<
    ", length high tide " << fPartMeasureLengthHighTide <<
    ")" <<
    endl;
    
  gIndenter++;
  
  const int fieldWidth = 27;

  os << left <<
    setw (fieldWidth) <<
    "partID" << " : \"" <<
    fPartID << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partMsrName" << " : \"" <<
    fPartMsrName << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partName" << " : \"" <<
    fPartName << "\"" <<
    endl <<

    setw (fieldWidth) <<
    "partAbsoluteNumber" << " : " <<
    fPartAbsoluteNumber <<
    endl <<

    setw (fieldWidth) <<
    "partNameDisplayText" << " : \"" <<
    fPartNameDisplayText << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partAbbrevation" << " : \"" <<
    fPartAbbreviation << "\"" <<
    endl <<
    setw (fieldWidth) <<
    "partAbbreviationDisplayText" << " : \"" <<
    fPartAbbreviationDisplayText << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partInstrumentName" << " : \"" <<
    fPartInstrumentName << "\"" <<
    endl <<    
    setw (fieldWidth) <<
    "partInstrumentAbbreviation" << " : \"" <<
    fPartInstrumentAbbreviation << "\"" <<
    endl <<
    
    setw (fieldWidth) <<
    "partNumberOfMeasures" << " : " <<
    fPartNumberOfMeasures <<
    endl <<
    endl;

  // print the staves
  if (fPartStavesMap.size ()) {
    os << 
      "partStavesMap" <<
      endl;
              
    gIndenter++;
  
    map<int, S_msrStaff>::const_iterator
      iBegin = fPartStavesMap.begin (),
      iEnd   = fPartStavesMap.end (),
      i      = iBegin;
          
    for ( ; ; ) {
      (*i).second->printSummary (os);
      if (++i == iEnd) break;
      os << endl;
    } // for

    gIndenter--;
  }

  // print the figured bass staff if any // JMI
  if (fPartFiguredBassStaff) {
    os <<
"partFiguredBassStaff" <<
      endl;
            
    gIndenter++;
    os <<
      fPartFiguredBassStaff;
    gIndenter--;
  }

  gIndenter--;
}

ostream& operator<< (ostream& os, const S_msrPart& elt)
{
  elt->print (os);
  return os;
}


}
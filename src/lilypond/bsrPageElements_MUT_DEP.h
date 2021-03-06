/*
  This file is to be included only by bsrMutualDependencies.h,
  to satisfy declarations mutual dependencies.
*/

//______________________________________________________________________________
class bsrPageElement : public bsrElement
{
  public:

    // creation
    // ------------------------------------------------------

/*
    static SMARTP<bsrPageElement> create (
      int inputLineNumber);
*/

  protected:

    // constructors/destructor
    // ------------------------------------------------------

    bsrPageElement (
      int inputLineNumber);

    virtual ~bsrPageElement ();

  public:

    // set and get
    // ------------------------------------------------------

    void                  setBsrPageUpLink (
                            S_bsrPage bsrPageUpLink);

    S_bsrPage             getBsrPageUpLink () const;

    void                  setSpacesBefore (int value)
                              { fSpacesBefore = value; }

    int                   getSpacesBefore () const
                              { return fSpacesBefore; }

/*
    int                   getSpacesAfter () const
                              { return fSpacesAfter; }
*/

  public:

    // services
    // ------------------------------------------------------

    virtual int           fetchLineContentsNumber () const  // JMI ???
                              { return 0; }

  public:

    // visitors
    // ------------------------------------------------------

    virtual void          acceptIn  (basevisitor* v);
    virtual void          acceptOut (basevisitor* v);

    virtual void          browseData (basevisitor* v);

  public:

    // print
    // ------------------------------------------------------

    virtual std::string   asString () const;

    virtual std::string   asShortString () const;

    virtual void          print (ostream& os) const;

  protected:

    // fields
    // ------------------------------------------------------

    S_bsrPage             fBsrPageUpLink;

    int                   fSpacesBefore;
 // JMI   int                   fSpacesAfter;
};
typedef SMARTP<bsrPageElement> S_bsrPageElement;
EXP ostream& operator<< (ostream& os, const S_bsrPageElement& elt);


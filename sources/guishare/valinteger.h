//
// FILE: valinteger.h -- declaration of validator specialization to integers
//
// $Id$
//

#ifndef VALINTEGER_H
#define VALINTEGER_H

#ifdef __GNUG__
#pragma interface "valinteger.h"
#endif  // __GNUG__

#include "wx/validate.h"

class gIntegerValidator : public wxValidator {
protected:
  wxString *m_stringValue;
  bool m_hasMin, m_hasMax;
  int m_minValue, m_maxValue;
  
  // Event handlers
  void OnChar(wxKeyEvent &);

public:
  gIntegerValidator(wxString *);
  gIntegerValidator(wxString *, int);
  gIntegerValidator(wxString *, int, int);
  gIntegerValidator(const gIntegerValidator &);
  virtual ~gIntegerValidator() { }

  virtual wxObject *Clone(void) const { return new gIntegerValidator(*this); }
  bool Copy(const gIntegerValidator &);

  virtual bool Validate(wxWindow *parent);
  virtual bool TransferToWindow(void);
  virtual bool TransferFromWindow(void);

  DECLARE_EVENT_TABLE()
};


#endif  // VALNUMBER_H
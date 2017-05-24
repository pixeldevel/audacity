/**********************************************************************

  Audacity: A Digital Audio Editor

  SelectionBar.h

  Dominic Mazzoni

**********************************************************************/

#ifndef __AUDACITY_SELECTION_BAR__
#define __AUDACITY_SELECTION_BAR__

#include <wx/defs.h>

#include "ToolBar.h"

class wxBitmap;
class wxCheckBox;
class wxChoice;
class wxComboBox;
class wxCommandEvent;
class wxDC;
class wxRadioButton;
class wxSizeEvent;
class wxStaticText;

class SelectionBarListener;
class NumericTextCtrl;

enum
{
   numSelectionBarButtons = 1,
};

class SelectionBar final : public ToolBar {

 public:
   SelectionBar();
   virtual ~SelectionBar();

   void Create(wxWindow *parent);

   void Populate() override;
   void Repaint(wxDC * WXUNUSED(dc)) override {};
   void EnableDisableButtons() override {};
   void UpdatePrefs() override;

   void SetTimes(double start, double end, double audio);
   void SetField(const wxChar *msg, int fieldNum);
   void SetSnapTo(int);
   void SetSelectionFormat(const wxString & format);
   void SetRate(double rate);
   void SetListener(SelectionBarListener *l);
   void RegenerateTooltips() override;
   void OnButton(wxCommandEvent & event);

 private:
   wxRadioButton * AddRadioButton( const wxString & Name, int id, 
      wxSizer * pSizer, long style);
   wxStaticText * AddTitle( const wxString & Title, 
      wxSizer * pSizer );
   NumericTextCtrl * AddTime( const wxString Name, int id, wxSizer * pSizer );

   void SetSelectionMode(int mode);
   void ShowHideControls(int mode);
   void SetDrivers( int driver1, int driver2 );
   void ValuesToControls();
   void OnUpdate(wxCommandEvent &evt);
   void OnChangedTime(wxCommandEvent &evt);
   void OnTitleClicked(int newDriver );
   void OnStartTitleClicked(wxMouseEvent & event);
   void OnCenterTitleClicked(wxMouseEvent & event);
   void OnLengthTitleClicked(wxMouseEvent & event);
   void OnEndTitleClicked(wxMouseEvent & event);
   void OnRate(wxCommandEvent & event);
   void OnSnapTo(wxCommandEvent & event);
   void OnFocus(wxFocusEvent &event);
   void OnCaptureKey(wxCommandEvent &event);
   void OnSize(wxSizeEvent &evt);
   void OnFieldChoice(wxCommandEvent &event);

   void ModifySelection(int newDriver, bool done = false);
   void UpdateRates();
   void SelectionModeUpdated();

   SelectionBarListener * mListener;
   double mRate;
   double mStart, mEnd, mLength, mCenter,  mAudio;
   wxString mField[10];

   // Only used if we use radio buttons.
   bool mbUseNativeRadioButton;

   // These two numbers say which two controls 
   // drive the other two.
   int mDrive1;
   int mDrive2;

   int mSelectionMode;

   NumericTextCtrl   *mStartTime;
   NumericTextCtrl   *mCenterTime;
   NumericTextCtrl   *mLengthTime;
   NumericTextCtrl   *mEndTime;
   NumericTextCtrl   *mAudioTime;

   wxStaticText * mStartTitle;
   wxStaticText * mCenterTitle;
   wxStaticText * mLengthTitle;
   wxStaticText * mEndTitle;

   wxStaticText * mProxy;
   wxStaticText * mStartEndProxy;
   wxStaticText * mStartLengthProxy;
   wxStaticText * mLengthEndProxy;
   wxStaticText * mLengthCenterProxy;

   wxRadioButton * mStartEndRadBtn;
   wxRadioButton * mStartLengthRadBtn;
   wxRadioButton * mLengthEndRadBtn;
   wxRadioButton * mLengthCenterRadBtn;

   wxComboBox     *mRateBox;
   wxChoice       *mSnapTo;

   wxWindow       *mRateText;

   AButton * mButtons[numSelectionBarButtons];

 public:

   DECLARE_CLASS(SelectionBar)
   DECLARE_EVENT_TABLE()
};

#endif


/**********************************************************************

  Audacity: A Digital Audio Editor

  LV2Effect.h

  Audacity(R) is copyright (c) 1999-2013 Audacity Team.
  License: GPL v2 or later.  See License.txt.

*********************************************************************/

#ifndef __AUDACITY_LV2_EFFECT__
#define __AUDACITY_LV2_EFFECT__

#if USE_LV2

class wxArrayString;

#include <optional>
#include <vector>

#include <wx/event.h> // to inherit
#include <wx/timer.h>
#include <wx/weakref.h>
#include <wx/window.h>
#include <wx/windowptr.h>

#include "LV2UIFeaturesList.h"
#include "LV2Ports.h"
#include "../../ShuttleGui.h"
#include "SampleFormat.h"
#include "../StatefulPerTrackEffect.h"

#include "NativeWindow.h"

using LilvInstancePtr = Lilv_ptr<LilvInstance, lilv_instance_free>;
using LilvUIsPtr = Lilv_ptr<LilvUIs, lilv_uis_free>;
using SuilInstancePtr = Lilv_ptr<SuilInstance, suil_instance_free>;

// We use deprecated LV2 interfaces to remain compatible with older
// plug-ins, so disable warnings
LV2_DISABLE_DEPRECATION_WARNINGS

class wxSlider;
class wxTextCtrl;
class NumericTextCtrl;

#define LV2EFFECTS_VERSION wxT("1.0.0.0")
/* i18n-hint: abbreviates
   "Linux Audio Developer's Simple Plugin API (LADSPA) version 2" */
#define LV2EFFECTS_FAMILY XO("LV2")

// DECLARE_LOCAL_EVENT_TYPE(EVT_SIZEWINDOW, -1);

class LV2EffectMeter;
class LV2Validator;
class LV2Wrapper;

class LV2Effect final : public StatefulPerTrackEffect
{
public:
   LV2Effect(const LilvPlugin &plug);
   virtual ~LV2Effect();

   // ComponentInterface implementation

   PluginPath GetPath() const override;
   ComponentInterfaceSymbol GetSymbol() const override;
   VendorSymbol GetVendor() const override;
   wxString GetVersion() const override;
   TranslatableString GetDescription() const override;

   // EffectDefinitionInterface implementation

   EffectType GetType() const override;
   EffectFamilySymbol GetFamily() const override;
   bool IsInteractive() const override;
   bool IsDefault() const override;
   RealtimeSince RealtimeSupport() const override;
   bool SupportsAutomation() const override;

   bool SaveSettings(
      const EffectSettings &settings, CommandParameters & parms) const override;
   bool LoadSettings(
      const CommandParameters & parms, EffectSettings &settings) const override;

   bool LoadUserPreset(
      const RegistryPath & name, EffectSettings &settings) const override;
   bool SaveUserPreset(
      const RegistryPath & name, const EffectSettings &settings) const override;

   RegistryPaths GetFactoryPresets() const override;
   bool LoadFactoryPreset(int id, EffectSettings &settings) const override;

   unsigned GetAudioInCount() const override;
   unsigned GetAudioOutCount() const override;

   int GetMidiInCount() const override;
   int GetMidiOutCount() const override;

   int ShowClientInterface(wxWindow &parent, wxDialog &dialog,
      EffectUIValidator *pValidator, bool forceModal) override;

   bool InitializePlugin();

   // EffectUIClientInterface implementation

   std::shared_ptr<EffectInstance> MakeInstance() const override;
   std::shared_ptr<EffectInstance> DoMakeInstance();
   std::unique_ptr<EffectUIValidator> PopulateUI(
      ShuttleGui &S, EffectInstance &instance, EffectSettingsAccess &access)
   override;
   bool CloseUI() override;

   bool CanExportPresets() override;
   void ExportPresets(const EffectSettings &settings) const override;
   void ImportPresets(EffectSettings &settings) override;

   bool HasOptions() override;
   void ShowOptions() override;

   // LV2Effect implementation

private:
   void InitializeSettings(const LV2Ports &ports, LV2EffectSettings &settings);

   bool LoadParameters(
      const RegistryPath & group, EffectSettings &settings) const;
   bool SaveParameters(
      const RegistryPath & group, const EffectSettings &settings) const;

private:
   const LilvPlugin &mPlug;
   const LV2FeaturesList mFeatures{ mPlug };

   const LV2Ports mPorts{ mPlug };
   LV2EffectSettings mSettings;

   //! This ignores its argument while we transition to statelessness
   //! but will later be rewritten as a static member function
   LV2EffectSettings &GetSettings(EffectSettings &) const {
      return const_cast<LV2EffectSettings &>(mSettings);
   }
   //! This ignores its argument while we transition to statelessness
   //! but will later be rewritten as a static member function
   const LV2EffectSettings &GetSettings(const EffectSettings &) const {
      return mSettings;
   }

   bool mWantsOptionsInterface{ false };
   bool mWantsStateInterface{ false };

   size_t mFramePos{};

   FloatBuffers mCVInBuffers;
   FloatBuffers mCVOutBuffers;

   double mLength{};

   wxWindow *mParent{};

   // Mutable cache fields computed once on demand
   mutable bool mFactoryPresetsLoaded{ false };
   mutable RegistryPaths mFactoryPresetNames;
   mutable wxArrayString mFactoryPresetUris;
};

class LV2Instance;

class LV2Validator final : public EffectUIValidator
   , wxEvtHandler
   , LV2UIFeaturesList::UIHandler
{
public:
   LV2Validator(EffectBase &effect,
      const LilvPlugin &plug, LV2Instance &instance,
      EffectSettingsAccess &access, double sampleRate,
      const LV2FeaturesList &features,
      const LV2Ports &ports, wxWindow *parent, bool useGUI);
   ~LV2Validator() override;

   bool ValidateUI() override;
   bool UpdateUI() override;
   bool IsGraphicalUI() override;

   // TODO static or non-member function
   LV2EffectSettings &GetSettings(EffectSettings &settings) const;
   // TODO static or non-member function
   const LV2EffectSettings &GetSettings(const EffectSettings &settings) const;

   int ui_resize(int width, int height) override;
   void ui_closed() override;

#if defined(__WXGTK__)
   static void size_request(GtkWidget *widget, GtkRequisition *requisition,
      LV2Validator *pValidator);
   void SizeRequest(GtkWidget *widget, GtkRequisition *requisition);
#endif

   bool BuildFancy(const LV2Wrapper &wrapper, const EffectSettings &settings);
   bool BuildPlain(EffectSettingsAccess &access);

   void suil_port_write(uint32_t port_index,
      uint32_t buffer_size, uint32_t protocol, const void *buffer) override;
   uint32_t suil_port_index(const char *port_symbol) override;

   void OnTrigger(wxCommandEvent & evt);
   void OnToggle(wxCommandEvent & evt);
   void OnChoice(wxCommandEvent & evt);
   void OnText(wxCommandEvent & evt);
   void OnSlider(wxCommandEvent & evt);

   void OnIdle(wxIdleEvent & evt);
   void OnSize(wxSizeEvent & evt);

   static std::shared_ptr<SuilHost> GetSuilHost();

   const LilvPlugin &mPlug;
   const EffectType mType;
   LV2Instance &mInstance;
   const double mSampleRate;
   const LV2Ports &mPorts;
   std::optional<const LV2UIFeaturesList> mUIFeatures;
   LV2PortUIStates mPortUIStates;

   std::shared_ptr<SuilHost> mSuilHost;
   wxWindow *const mParent;
   bool mUseGUI{};

   // UI
   struct PlainUIControl {
      wxTextCtrl *mText{};
      //! Discriminate this union according to corresponding port's properties
      union {
         wxButton *button;
         wxCheckBox *checkbox;
         wxChoice *choice;
         LV2EffectMeter *meter;
         wxSlider *slider;
      };
   };
   //! Array in correspondence with the control ports
   std::vector<PlainUIControl> mPlainUIControls;
   void SetSlider(const LV2ControlPortState &state, const PlainUIControl &ctrl);

   SuilInstancePtr mSuilInstance;

   //! Destroy before mSuilInstance
   wxWindowPtr<NativeWindow> mNativeWin{};
   wxSize mNativeWinInitialSize{ wxDefaultSize };
   wxSize mNativeWinLastSize{ wxDefaultSize };
   bool mResizing{ false };
#if defined(__WXGTK__)
   bool mResized{ false };
#endif

   wxWeakRef<wxDialog> mDialog;
   bool mExternalUIClosed{ false };

   //! This must be destroyed before mSuilInstance
   struct Timer : wxTimer {
      LV2_External_UI_Widget* mExternalWidget{};
      void Notify() override;
   } mTimer;

   const LV2UI_Idle_Interface *mUIIdleInterface{};
   const LV2UI_Show_Interface *mUIShowInterface{};
   NumericTextCtrl *mDuration{};

   DECLARE_EVENT_TABLE()
};

#endif
#endif

/**********************************************************************

  Audacity: A Digital Audio Editor

  Effect.h

  Dominic Mazzoni
  Vaughan Johnson

  Paul Licameli split from Effect.h

**********************************************************************/

#ifndef __AUDACITY_STATEFUL_EFFECT_BASE__
#define __AUDACITY_STATEFUL_EFFECT_BASE__

#include "EffectPlugin.h"

//! A mix-in class for effects that are not yet migrated to statelessness.
//! To be eliminated when all effects are migrated
class AUDACITY_DLL_API StatefulEffectBase {
public:
   //! Calls through to members of StatefulEffectBase
   class AUDACITY_DLL_API Instance : public virtual EffectInstance {
   public:
      explicit Instance(StatefulEffectBase &effect);
      ~Instance() override;

      bool Init() override;

      void SetSampleRate(double rate) override;
   
      size_t GetBlockSize() const override;
      size_t SetBlockSize(size_t maxBlockSize) override;
   
      bool RealtimeInitialize(EffectSettings &settings, double sampleRate)
         override;
      bool RealtimeAddProcessor(EffectSettings &settings,
         unsigned numChannels, float sampleRate) override;
      bool RealtimeSuspend() override;
      bool RealtimeResume() override;
      bool RealtimeProcessStart(EffectSettings &settings) override;
      size_t RealtimeProcess(size_t group, EffectSettings &settings,
         const float *const *inBuf, float *const *outBuf, size_t numSamples)
      override;
      bool RealtimeProcessEnd(EffectSettings &settings) noexcept override;
      bool RealtimeFinalize(EffectSettings &settings) noexcept override;
   protected:
      StatefulEffectBase &mEffect;
      StatefulEffectBase &GetEffect() const { return mEffect; }
   };

   /*!
     @copydoc EffectInstance::Init()
     Default implementation does nothing, returns true
   */
   virtual bool Init();

   /*!
    @copydoc EffectInstance::Process
    */
   virtual bool Process(EffectInstance &instance, EffectSettings &settings) = 0;

   /*!
     @copydoc EffectInstance::SetSampleRate()
     Default implementation assigns mSampleRate
   */
   virtual void SetSampleRate(double rate);

   /*!
     @copydoc StatefulEffectBase::Instance::RealtimeInitialize()
     Default implementation does nothing, returns false
   */
   virtual bool RealtimeInitialize(EffectSettings &settings, double sampleRate);

   /*!
     @copydoc StatefulEffectBase::Instance::RealtimeAddProcessor()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeAddProcessor(
      EffectSettings &settings, unsigned numChannels, float sampleRate);

   /*!
     @copydoc StatefulEffectBase::Instance::RealtimeSuspend()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeSuspend();

   /*!
     @copydoc StatefulEffectBase::Instance::RealtimeResume()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeResume();

   /*!
     @copydoc StatefulEffectBase::Instance::RealtimeProcessStart()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeProcessStart(EffectSettings &settings);

   /*!
     @copydoc StatefulEffectBase::Instance::RealtimeProcess()
     Default implementation does nothing, returns 0
   */
   virtual size_t RealtimeProcess(size_t group, EffectSettings &settings,
      const float *const *inBuf, float *const *outBuf, size_t numSamples);

   /*!
     @copydoc StatefulEffectBase::Instance::RealtimeProcessEnd()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeProcessEnd(EffectSettings &settings) noexcept;

   /*!
     @copydoc StatefulEffectBase::Instance::RealtimeFinalize()
     Default implementation does nothing, returns false
   */
   virtual bool RealtimeFinalize(EffectSettings &settings) noexcept;

   /*!
     @copydoc StatefulEffectBase::Instance::SetBlockSize()
     Default implementation assigns mEffectBlockSize, returns it
   */
   virtual size_t SetBlockSize(size_t maxBlockSize);

   /*!
     @copydoc StatefulEffectBase::Instance::GetBlockSize()
     Default implementation returns mEffectBlockSize
   */
   virtual size_t GetBlockSize() const;

protected:

   double         mSampleRate{};
private:

   size_t mEffectBlockSize{ 0 };
};

#endif

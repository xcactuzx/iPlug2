#include "IPlugEffect.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "IVTabbedPagesControl.h"

IPlugEffect::IPlugEffect(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->AttachBubbleControl();
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds().GetPadded(-10);
    
    auto resizeFunc = [](IContainerBase* pCaller, const IRECT& r) {
      auto innerBounds = r.GetPadded(-10);
      pCaller->GetChild(0)->SetTargetAndDrawRECTs(innerBounds.SubRectHorizontal(3, 0));
    };
    
    pGraphics->AttachControl(new IVTabbedPagesControl(b.FracRectVertical(0.5, true),
    {
      {"1", new IVTabbedPageBase([](IContainerBase* pCaller, const IRECT& r) {
                                  pCaller->AddChildControl(new IVKnobControl(IRECT(), kGain, "Knob"))
                                  ->SetActionFunction(ShowBubbleHorizontalActionFunc);
                                }, resizeFunc)},
      {"2", new IVTabbedPageBase([](IContainerBase* pCaller, const IRECT& r) {
                                  pCaller->AddChildControl(new IVSliderControl(IRECT(), kGain, "Slider"))
                                  ->SetActionFunction(ShowBubbleHorizontalActionFunc);
                                }, resizeFunc)},
      {"3", new IVTabbedPageBase([](IContainerBase* pCaller, const IRECT& r) {
                                  pCaller->AddChildControl(new IVButtonControl(IRECT(), DefaultClickActionFunc, "Button"));
                                }, resizeFunc)}
    }, ""));
  };
#endif
}

#if IPLUG_DSP
void IPlugEffect::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s] * gain;
    }
  }
}
#endif

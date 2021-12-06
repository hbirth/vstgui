// This file is part of VSTGUI. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#include "win32viewlayer.h"
#include "win32directcomposition.h"
#include "win32factory.h"
#include "direct2d/d2ddrawcontext.h"

//------------------------------------------------------------------------
namespace VSTGUI {

//------------------------------------------------------------------------
Win32ViewLayer::Win32ViewLayer (const DirectComposition::VisualPtr& visual,
								IPlatformViewLayerDelegate* inDelegate)
: visual (visual), delegate (inDelegate)
{
}

//------------------------------------------------------------------------
Win32ViewLayer::~Win32ViewLayer () noexcept
{
	getPlatformFactory ().asWin32Factory ()->getDirectCompositionFactory ()->removeVisual (visual);
}

//------------------------------------------------------------------------
void Win32ViewLayer::fire ()
{
	drawInvalidRects ();
	timer = nullptr;
}

//------------------------------------------------------------------------
void Win32ViewLayer::drawInvalidRects ()
{
	for (const auto& r : invalidRectList)
	{
		visual->update (r, [&] (auto deviceContext, auto updateRect, auto offsetX, auto offsetY) {
			D2DDrawContext drawContext (deviceContext, viewSize);
			drawContext.setClipRect (updateRect);
			CGraphicsTransform tm;
			tm.translate (offsetX - updateRect.left, offsetY - updateRect.top);
			CDrawContext::Transform transform (drawContext, tm);
			{
				drawContext.saveGlobalState ();
				drawContext.clearRect (updateRect);
				delegate->drawViewLayer (&drawContext, updateRect);
				drawContext.restoreGlobalState ();
			}
		});
	}
	lastDrawTime = getPlatformFactory ().getTicks ();
	invalidRectList.clear ();
	visual->commit ();
}

//------------------------------------------------------------------------
void Win32ViewLayer::invalidRect (const CRect& size)
{
	auto r = size;
	r.normalize ();
	r.makeIntegral ();
	r.bound (viewSize);
	invalidRectList.add (r);
	if (!timer)
	{
		auto ticks = getPlatformFactory ().getTicks () - lastDrawTime;
		if (ticks > 15)
			ticks = 0;
		timer = makeOwned<WinTimer> (this);
		timer->start (static_cast<uint32_t> (ticks));
	}
}

//------------------------------------------------------------------------
void Win32ViewLayer::setSize (const CRect& size)
{
	auto r = size;
	r.normalize ();
	r.makeIntegral ();
	visual->setPosition (static_cast<uint32_t> (r.left), static_cast<uint32_t> (size.top));
	visual->resize (static_cast<uint32_t> (r.getWidth ()), static_cast<uint32_t> (r.getHeight ()));
	viewSize = r;
	viewSize.originize ();
	invalidRect (viewSize);
}

//------------------------------------------------------------------------
void Win32ViewLayer::setZIndex (uint32_t zIndex)
{
	visual->setZIndex (zIndex);
}

//------------------------------------------------------------------------
void Win32ViewLayer::setAlpha (float alpha)
{
	visual->setOpacity (alpha);
	visual->commit ();
}

//------------------------------------------------------------------------
void Win32ViewLayer::draw (CDrawContext* context, const CRect& updateRect) {}

//------------------------------------------------------------------------
void Win32ViewLayer::onScaleFactorChanged (double newScaleFactor) {}

//------------------------------------------------------------------------
const DirectComposition::VisualPtr& Win32ViewLayer::getVisual () const
{
	return visual;
}

//------------------------------------------------------------------------
} // VSTGUI

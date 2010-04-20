//-----------------------------------------------------------------------------
// VST Plug-Ins SDK
// VSTGUI: Graphical User Interface Framework for VST plugins : 
//
// Version 4.0
//
//-----------------------------------------------------------------------------
// VSTGUI LICENSE
// (c) 2010, Steinberg Media Technologies, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation 
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this 
//     software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#include "coptionmenu.h"
#include "../cbitmap.h"
#include "../cframe.h"

#include "../platform/iplatformoptionmenu.h"

namespace VSTGUI {

//------------------------------------------------------------------------
// CMenuItem
//------------------------------------------------------------------------
/*! @class CMenuItem
Defines an item of a VSTGUI::COptionMenu
*/
//------------------------------------------------------------------------
/**
 * CMenuItem constructor.
 * @param inTitle title of item
 * @param inFlags CMenuItem::Flags of item
 * @param inKeycode keycode of item
 * @param inKeyModifiers keymodifiers of item
 * @param inIcon icon of item
 */
//------------------------------------------------------------------------
CMenuItem::CMenuItem (const char* inTitle, const char* inKeycode, long inKeyModifiers, CBitmap* inIcon, long inFlags)
: title (0)
, flags (inFlags)
, keycode (0)
, keyModifiers (0)
, submenu (0)
, icon (0)
, tag (-1)
{
	setTitle (inTitle);
	setKey (inKeycode, inKeyModifiers);
	setIcon (inIcon);
}

//------------------------------------------------------------------------
/**
 * CMenuItem constructor.
 * @param inTitle title of item
 * @param inSubmenu submenu of item
 * @param inIcon icon of item
 */
//------------------------------------------------------------------------
CMenuItem::CMenuItem (const char* inTitle, COptionMenu* inSubmenu, CBitmap* inIcon)
: title (0)
, flags (0)
, keycode (0)
, keyModifiers (0)
, submenu (0)
, icon (0)
, tag (-1)
{
	setTitle (inTitle);
	setSubmenu (inSubmenu);
	setIcon (inIcon);
}

//------------------------------------------------------------------------
/**
 * CMenuItem constructor.
 * @param inTitle title of item
 * @param inTag tag of item
 */
//------------------------------------------------------------------------
CMenuItem::CMenuItem (const char* inTitle, long inTag)
: title (0)
, flags (0)
, keycode (0)
, keyModifiers (0)
, submenu (0)
, icon (0)
, tag (-1)
{
	setTitle (inTitle);
	setTag (inTag);
}

//------------------------------------------------------------------------
/**
 * CMenuItem copy constructor.
 * @param item item to copy
 */
//------------------------------------------------------------------------
CMenuItem::CMenuItem (const CMenuItem& item)
: title (0)
, flags (item.flags)
, keycode (0)
, keyModifiers (0)
, submenu (0)
, icon (0)
{
	setTitle (item.getTitle ());
	setIcon (item.getIcon ());
	setKey (item.getKeycode (), item.getKeyModifiers ());
	setTag (item.getTag ());
	*submenu = *item.getSubmenu ();
}

//------------------------------------------------------------------------
CMenuItem::~CMenuItem ()
{
	setIcon (0);
	setSubmenu (0);
	setTitle (0);
	setKey (0);
}

//------------------------------------------------------------------------
void CMenuItem::setTitle (const char* inTitle)
{
	if (title)
		free (title);
	title = 0;
	if (inTitle)
	{
		title = (char*)malloc (strlen (inTitle) + 1);
		strcpy (title, inTitle);
	}
}

//------------------------------------------------------------------------
void CMenuItem::setKey (const char* inKeycode, long inKeyModifiers)
{
	if (keycode)
		free (keycode);
	keycode = 0;
	if (inKeycode)
	{
		keycode = (char*)malloc (strlen (inKeycode) + 1);
		strcpy (keycode, inKeycode);
	}
	keyModifiers = inKeyModifiers;
}

//------------------------------------------------------------------------
void CMenuItem::setSubmenu (COptionMenu* inSubmenu)
{
	if (submenu)
		submenu->forget ();
	submenu = inSubmenu;
	if (submenu)
		submenu->remember ();
}

//------------------------------------------------------------------------
void CMenuItem::setIcon (CBitmap* inIcon)
{
	if (icon)
		icon->forget ();
	icon = inIcon;
	if (icon)
		icon->remember ();
}

//------------------------------------------------------------------------
void CMenuItem::setTag (long t)
{
	tag = t;
}

//------------------------------------------------------------------------
void CMenuItem::setEnabled (bool state)
{
	if (state)
		flags &= ~kDisabled;
	else
		flags |= kDisabled;
}

//------------------------------------------------------------------------
void CMenuItem::setChecked (bool state)
{
	if (state)
		flags |= kChecked;
	else
		flags &= ~kChecked;
}

//------------------------------------------------------------------------
void CMenuItem::setIsTitle (bool state)
{
	if (state)
		flags |= kTitle;
	else
		flags &= ~kTitle;
}

//------------------------------------------------------------------------
void CMenuItem::setIsSeparator (bool state)
{
	if (state)
		flags |= kSeparator;
	else
		flags &= ~kSeparator;
}

//------------------------------------------------------------------------
// COptionMenu
//------------------------------------------------------------------------
/*! @class COptionMenu
Define a rectangle view where a text-value can be displayed with a given font and color.
The text-value is centered in the given rect.
A bitmap can be used as background, a second bitmap can be used when the option menu is popuped.
There are 2 styles with or without a shadowed text. When a mouse click occurs, a popup menu is displayed.
*/
//------------------------------------------------------------------------
/**
 * COptionMenu constructor.
 * @param size the size of this view
 * @param listener the listener
 * @param tag the control tag
 * @param background the background bitmap
 * @param bgWhenClick the background bitmap if the option menu is displayed
 * @param style the style of the display (see CParamDisplay for styles)
 */
//------------------------------------------------------------------------
COptionMenu::COptionMenu (const CRect& size, CControlListener* listener, long tag, CBitmap* background, CBitmap* bgWhenClick, const long style)
: CParamDisplay (size, background, style)
, bgWhenClick (bgWhenClick)
, nbItemsPerColumn (-1)
, prefixNumbers (0)
, inPopup (false)
{
	this->listener = listener;
	this->tag = tag;

	currentIndex = -1;
	lastButton = kRButton;
	lastResult = -1;
	lastMenu = 0;
	
	if (bgWhenClick)
		bgWhenClick->remember ();

	menuItems = new CMenuItemList;
	setWantsFocus (true);
}

//------------------------------------------------------------------------
COptionMenu::COptionMenu ()
: CParamDisplay (CRect (0, 0, 0, 0))
, currentIndex (-1)
, bgWhenClick (0)
, lastButton (0)
, nbItemsPerColumn (-1)
, lastResult (-1)
, prefixNumbers (0)
, lastMenu (0)
, inPopup (false)
{
	menuItems = new CMenuItemList;
	setWantsFocus (true);
}

//------------------------------------------------------------------------
COptionMenu::COptionMenu (const COptionMenu& v)
: CParamDisplay (v)
, currentIndex (-1)
, bgWhenClick (v.bgWhenClick)
, lastButton (0)
, nbItemsPerColumn (v.nbItemsPerColumn)
, lastResult (-1)
, prefixNumbers (0)
, lastMenu (0)
, menuItems (new CMenuItemList (*v.menuItems))
, inPopup (false)
{
	if (bgWhenClick)
		bgWhenClick->remember ();

	CMenuItemIterator it = menuItems->begin ();
	while (it != menuItems->end ())
	{
		(*it)->remember ();
		it++;
	}
	setWantsFocus (true);
}

//------------------------------------------------------------------------
COptionMenu::~COptionMenu ()
{
	removeAllEntry ();

	if (bgWhenClick)
		bgWhenClick->forget ();

	delete menuItems;
}

//------------------------------------------------------------------------
long COptionMenu::onKeyDown (VstKeyCode& keyCode)
{
	if (keyCode.modifier == 0 && keyCode.character == 0)
	{
		if (keyCode.virt == VKEY_RETURN)
		{
			if (bgWhenClick)
				invalid ();
			popup ();
			if (bgWhenClick)
				invalid ();
			return 1;
		}
		if (!(style & (kMultipleCheckStyle & ~kCheckStyle)))
		{
			if (keyCode.virt == VKEY_UP)
			{
				long value = (long)getValue ()-1;
				if (value >= 0)
				{
					CMenuItem* entry = getEntry (value);
					while (entry && (entry->isSeparator () || entry->isTitle () || !entry->isEnabled () || entry->getSubmenu ()))
						entry = getEntry (--value);
					if (entry)
					{
						beginEdit ();
						setValue ((float)value);
						lastResult = (long)getValue ();
						valueChanged ();
						endEdit ();
						invalid ();
					}
				}
				return 1;
			}
			if (keyCode.virt == VKEY_DOWN)
			{
				long value = (long)getValue ()+1;
				if (value < getNbEntries ())
				{
					CMenuItem* entry = getEntry (value);
					while (entry && (entry->isSeparator () || entry->isTitle () || !entry->isEnabled () || entry->getSubmenu ()))
						entry = getEntry (++value);
					if (entry)
					{
						beginEdit ();
						setValue ((float)value);
						lastResult = (long)getValue ();
						valueChanged ();
						endEdit ();
						invalid ();
					}
				}
				return 1;
			}
		}
	}
	return CParamDisplay::onKeyDown (keyCode);
}

//------------------------------------------------------------------------
bool COptionMenu::popup ()
{
	bool popupResult = false;
	if (!getFrame ())
		return popupResult;

	inPopup = true;

	beginEdit ();

	lastResult = -1;
	lastMenu = 0;

	IPlatformOptionMenu* platformMenu = getFrame ()->getPlatformFrame ()->createPlatformOptionMenu ();
	if (platformMenu)
	{
		PlatformOptionMenuResult platformPopupResult = platformMenu->popup (this);
		if (platformPopupResult.menu != 0)
		{
			lastMenu = platformPopupResult.menu;
			lastResult = platformPopupResult.index;
			lastMenu->value = lastResult;
			valueChanged ();
			invalid ();
			popupResult = true;
		}
		platformMenu->forget ();
	}

	endEdit ();
	inPopup = false;
	return popupResult;
}

//------------------------------------------------------------------------
bool COptionMenu::popup (CFrame* frame, const CPoint& frameLocation)
{
	if (frame == 0)
		return false;
	if (isAttached ())
		return false;
	CView* oldFocusView = frame->getFocusView ();
	CRect size (frameLocation, CPoint (0, 0));
	setViewSize (size);
	frame->addView (this);
	popup ();
	frame->removeView (this, false);
	frame->setFocusView (oldFocusView);
	long index;
	if (getLastItemMenu (index))
		return true;
	return false;
}

//------------------------------------------------------------------------
void COptionMenu::setPrefixNumbers (long preCount)
{
	if (preCount >= 0 && preCount <= 4)
		prefixNumbers = preCount;
}

/**
 * @param item menu item to add. Takes ownership of item.
 * @param index position of insertation. -1 appends the item
 */
//-----------------------------------------------------------------------------
CMenuItem* COptionMenu::addEntry (CMenuItem* item, long index)
{
	if (index == -1)
		menuItems->push_back (item);
	else
	{
		CMenuItemIterator it = menuItems->begin ();
		for (int i = 0; i < index && it != menuItems->end (); i++, it++);
		menuItems->insert (it, item);
	}
	return item;
}

//-----------------------------------------------------------------------------
CMenuItem* COptionMenu::addEntry (COptionMenu* submenu, const char* title)
{
	CMenuItem* item = new CMenuItem (title, submenu);
	return addEntry (item);
}

//-----------------------------------------------------------------------------
CMenuItem* COptionMenu::addEntry (const char* title, long index, long itemFlags)
{
	if (title && strcmp (title, "-") == 0)
		return addSeparator ();
	CMenuItem* item = new CMenuItem (title, 0, 0, 0, itemFlags);
	return addEntry (item, index);
}

//-----------------------------------------------------------------------------
CMenuItem* COptionMenu::addSeparator ()
{
	CMenuItem* item = new CMenuItem ("", 0, 0, 0, CMenuItem::kSeparator);
	return addEntry (item);
}

//-----------------------------------------------------------------------------
CMenuItem* COptionMenu::getCurrent () const
{
	return getEntry (currentIndex);
}

//-----------------------------------------------------------------------------
CMenuItem* COptionMenu::getEntry (long index) const
{
	if (menuItems->empty())
		return 0;
	
	CMenuItemIterator it = menuItems->begin ();
	for (int i = 0; i < index && it != menuItems->end (); i++, it++);
	if (it == menuItems->end ())
		return 0;
	return (*it);
}

//-----------------------------------------------------------------------------
long COptionMenu::getNbEntries () const
{
	return (long) menuItems->size ();
}

//------------------------------------------------------------------------
COptionMenu* COptionMenu::getSubMenu (long idx) const
{
	CMenuItem* item = getEntry (idx);
	if (item)
		return item->getSubmenu ();
	return 0;
}

//------------------------------------------------------------------------
long COptionMenu::getCurrentIndex (bool countSeparator) const
{
	if (countSeparator)
		return currentIndex;
	long i = 0;
	long numSeparators = 0;
	CMenuItemIterator it = menuItems->begin ();
	while (it != menuItems->end ())
	{
		if ((*it)->isSeparator ())
			numSeparators++;
		if (i == currentIndex)
			break;
		it++;
		i++;
	}
	return currentIndex - numSeparators;
}

//------------------------------------------------------------------------
bool COptionMenu::setCurrent (long index, bool countSeparator)
{
	CMenuItem* item = 0;
	if (countSeparator)
	{
		item = getEntry (index);
		if (!item || (item && item->isSeparator ()))
			return false;
		currentIndex = index;
	}
	else
	{
		long i = 0;
		CMenuItemIterator it = menuItems->begin ();
		while (it != menuItems->end ())
		{
			if (i > index)
				break;
			if ((*it)->isSeparator ())
				index++;
			it++;
			i++;
		}
		currentIndex = index;
		item = getEntry (currentIndex);
	}
	if (item && style & (kMultipleCheckStyle & ~kCheckStyle))
		item->setChecked (!item->isChecked ());
	
	// to force the redraw
	setDirty ();

	return true;
}

//------------------------------------------------------------------------
bool COptionMenu::removeEntry (long index)
{
	CMenuItem* item = getEntry (index);
	if (item)
	{
		menuItems->remove (item);
		item->forget ();
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
bool COptionMenu::removeAllEntry ()
{
	for (CMenuItemIterator it = menuItems->begin(); it != menuItems->end(); ++it)
	{
		(*it)->forget();
	}
	menuItems->clear ();
	return true;
}

//------------------------------------------------------------------------
bool COptionMenu::checkEntry (long index, bool state)
{
	CMenuItem* item = getEntry (index);
	if (item)
	{
		item->setChecked (state);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
bool COptionMenu::checkEntryAlone (long index)
{
	CMenuItemIterator it = menuItems->begin ();
	long pos = 0;
	while (it != menuItems->end ())
	{
		(*it)->setChecked (pos == index);
		it++;
		pos++;
	}
	return true;
}

//------------------------------------------------------------------------
bool COptionMenu::isCheckEntry (long index) const
{
	CMenuItem* item = getEntry (index);
	if (item && item->isChecked ())
		return true;
	return false;
}

//------------------------------------------------------------------------
void COptionMenu::draw (CDrawContext *pContext)
{
	CMenuItem* item = getEntry (currentIndex);
	drawBack (pContext, inPopup ? bgWhenClick : 0);
	drawText (pContext, item ? item->getTitle () : 0);
}

//------------------------------------------------------------------------
CMouseEventResult COptionMenu::onMouseDown (CPoint& where, const long& buttons)
{
	lastButton = buttons;
	if (lastButton & (kLButton|kRButton|kApple))
	{
		if (bgWhenClick)
			invalid ();
		getFrame ()->setFocusView (this);
		popup ();
		if (bgWhenClick)
			invalid ();
		return kMouseDownEventHandledButDontNeedMovedOrUpEvents;
	}
	return kMouseEventNotHandled;
}

//------------------------------------------------------------------------
COptionMenu *COptionMenu::getLastItemMenu (long &idxInMenu) const
{
	idxInMenu = lastMenu ? (long)lastMenu->getValue (): -1;
	return lastMenu;
}

//------------------------------------------------------------------------
void COptionMenu::setValue (float val, bool updateSubListeners)
{
	if ((long)val < 0 || (long)val >= getNbEntries ())
		return;
	
	currentIndex = (long)val;
	if (style & (kMultipleCheckStyle & ~kCheckStyle))
	{
		CMenuItem* item = getCurrent ();
		if (item)
			item->setChecked (!item->isChecked ());
	}
	CParamDisplay::setValue (val, updateSubListeners);
	
	// to force the redraw
	setDirty ();
}

//------------------------------------------------------------------------
void COptionMenu::takeFocus ()
{
	CParamDisplay::takeFocus ();
}

//------------------------------------------------------------------------
void COptionMenu::looseFocus ()
{	
	CView* receiver = pParentView ? pParentView : pParentFrame;
	while (receiver)
	{
		if (receiver->notify (this, kMsgLooseFocus) == kMessageNotified)
			break;
		receiver = receiver->getParentView ();
	}
}

} // namespace
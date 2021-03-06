#pragma once

/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <imagine/config/defs.hh>
#include "glIncludes.h"
#include "defs.hh"
#include <imagine/gfx/defs.hh>
#include <imagine/base/baseDefs.hh>
#include <imagine/base/CustomEvent.hh>
#include <imagine/time/Time.hh>
#include <imagine/util/DelegateFuncSet.hh>

namespace Base
{
class Window;
class Screen;
}

namespace Gfx
{

class Renderer;

class GLDrawableHolder
{
public:
	void makeDrawable(Renderer &r, Base::Window &win);
	void destroyDrawable(Renderer &r);
	Drawable drawable() const { return drawable_; };
	void notifyOnFrame();

protected:
	Base::Screen *screen{};
	Drawable drawable_;
	Base::ResumeDelegate onResume;
	Base::ExitDelegate onExit;
	Base::CustomEvent drawFinishedEvent{"GLDrawableHolder::drawFinishedEvent"};
	DelegateFuncSet<Base::OnFrameDelegate> onFrame{};
	FrameTime lastTimestamp{};
};

using DrawableHolderImpl = GLDrawableHolder;

}

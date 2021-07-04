module;
#include <string>
export module Vitro.App.WindowBase;

import Vitro.Math.Rectangle;
import Vitro.Math.Vector;

namespace vt
{
	export class WindowBase
	{
	public:
		virtual void open()							  = 0;
		virtual void close()						  = 0;
		virtual void maximize()						  = 0;
		virtual void minimize()						  = 0;
		virtual void enableCursor()					  = 0;
		virtual void disableCursor()				  = 0;
		virtual Rectangle getViewport() const		  = 0;
		virtual Rectangle getSize() const			  = 0;
		virtual void setSize(Rectangle size)		  = 0;
		virtual Int2 getPosition() const			  = 0;
		virtual void setPosition(Int2 position)		  = 0;
		virtual std::string getTitle() const		  = 0;
		virtual void setTitle(std::string_view title) = 0;
		virtual void* handle()						  = 0;

		bool cursorEnabled() const
		{
			return isCursorEnabled;
		}

	protected:
		bool isCursorEnabled = true;
	};
}

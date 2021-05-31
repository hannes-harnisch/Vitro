export module Vitro.App.WindowBase;

import Vitro.Math.Rectangle;

export class WindowBase
{
public:
	virtual void open()					  = 0;
	virtual void close()				  = 0;
	virtual void maximize()				  = 0;
	virtual void minimize()				  = 0;
	virtual void enableCursor()			  = 0;
	virtual void disableCursor()		  = 0;
	virtual Rectangle getViewport() const = 0;
	virtual void* handle()				  = 0;

	bool cursorEnabled() const
	{
		return isCursorEnabled;
	}

protected:
	bool isCursorEnabled = true;
};

export module Vitro.App.MouseCode;

export enum class MouseCode : unsigned char {
	None   = 0,
	Mouse1 = 1, // Primary mouse button.
	Mouse2 = 2, // Secondary mouse button.
	Wheel  = 3,
	Extra1 = 4,
	Extra2 = 5
};

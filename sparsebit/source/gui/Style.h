#pragma once

namespace gui
{
	enum BorderStyle
	{
		NONE,
		SOLID
	};

	class Style
	{
	public:
		int _borderStyle;

		float _innerBorderLeft;
		float _innerBorderRight;
		float _innerBorderTop;
		float _innerBorderBottom;

		float _outerBorderLeft;
		float _outerBorderRight;
		float _outerBorderTop;
		float _outerBorderBottom;

		Style(const Style &parent)
		{
			_borderStyle = parent._borderStyle;

			_innerBorderLeft = parent._innerBorderLeft;
			_innerBorderRight = parent._innerBorderRight;
			_innerBorderTop = parent._innerBorderTop;
			_innerBorderBottom = parent._outerBorderBottom;

			_outerBorderLeft = parent._outerBorderLeft;
			_outerBorderRight = parent._outerBorderRight;
			_outerBorderTop = parent._outerBorderTop;
			_outerBorderBottom = parent._outerBorderBottom;
		}

		Style()
		{
			_borderStyle = BorderStyle::NONE;

			_innerBorderLeft = 0.0f;
			_innerBorderRight = 0.0f;
			_innerBorderTop = 0.0f;
			_innerBorderBottom = 0.0f;

			_outerBorderLeft = 0.0f;
			_outerBorderRight = 0.0f;
			_outerBorderTop = 0.0f;
			_outerBorderBottom = 0.0f;
		}

		void operator=(const Style& parent)
		{
			_borderStyle = parent._borderStyle;

			_innerBorderLeft = parent._innerBorderLeft;
			_innerBorderRight = parent._innerBorderRight;
			_innerBorderTop = parent._innerBorderTop;
			_innerBorderBottom = parent._outerBorderBottom;

			_outerBorderLeft = parent._outerBorderLeft;
			_outerBorderRight = parent._outerBorderRight;
			_outerBorderTop = parent._outerBorderTop;
			_outerBorderBottom = parent._outerBorderBottom;
		}

		bool operator==(const Style& other)
		{
			return (_borderStyle == other._borderStyle &&

					_innerBorderLeft == other._innerBorderLeft &&
					_innerBorderRight == other._innerBorderRight &&
					_innerBorderTop == other._innerBorderTop &&
					_innerBorderBottom == other._outerBorderBottom &&

					_outerBorderLeft == other._outerBorderLeft &&
					_outerBorderRight == other._outerBorderRight &&
					_outerBorderTop == other._outerBorderTop &&
					_outerBorderBottom == other._outerBorderBottom);
		}
	};
}
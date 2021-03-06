#include <string>

#include <util/Logger.h>
#include "TextView.h"

logger::LogChannel textviewlog("textviewlog", "[TextView] ");

namespace gui {

TextView::TextView(std::string text) :
	_text(text),
	_dirty(true) {

	registerOutput(_painter, "painter");

	_painter.registerCallback(&TextView::onUpdate, this);
	_painter.registerSlot(_modified);
	_painter.registerSlot(_sizeChanged);
}

void
TextView::setText(std::string text) {

	_text  = text;
	_dirty = true;

	_modified();
}

const std::string&
TextView::getText() {

	return _text;
}

void
TextView::onUpdate(const pipeline::Update& /*signal*/) {

	LOG_ALL(textviewlog) << "got an update signal" << std::endl;

	if (_dirty) {

		LOG_ALL(textviewlog) << "I'm dirty, resetting my text" << std::endl;

		if (!_painter)
			_painter = new TextPainter();

		_painter->setText(_text);
		_dirty = false;

		LOG_ALL(textviewlog) << "sending size changed signal" << std::endl;

		_sizeChanged();
	}
}

} // namespace gui

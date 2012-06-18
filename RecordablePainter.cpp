#include <util/Logger.h>
#include "RecordablePainter.h"

namespace gui {

static logger::LogChannel recodablepainterlog("recodablepainterlog", "[RecordablePainter] ");

void
RecordablePainter::draw(
		const util::rect<double>&  roi,
		const util::point<double>& res) {

	if (!glIsList(_displayList)) {

		LOG_ALL(recodablepainterlog) << "trying to draw without initialisation of display list" << std::endl;

		return;
	}

	LOG_ALL(recodablepainterlog) << "redrawing" << std::endl;

	glCallList(_displayList);
}

void
RecordablePainter::startRecording() {

	// create a new display list, if needed
	if (!glIsList(_displayList)) {

		_displayList = glGenLists(1);

		if (!glIsList(_displayList))
			throw OpenGlError() << error_message("Couldn't create display list");
	}

	glCheck(glNewList(_displayList, GL_COMPILE));
}

void
RecordablePainter::stopRecording() {

	glCheck(glEndList());
}

} // namespace gui

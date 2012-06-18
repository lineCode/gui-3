#include <gui/OpenGl.h>
#include <util/Logger.h>
#include "RotatePainter.h"

namespace gui {

logger::LogChannel rotatepainterlog("rotatepainterlog", "[RotatePainter] ");

void
RotatePainter::setContent(boost::shared_ptr<Painter> content) {

	_content = content;

	updateSize();
}

void
RotatePainter::setRotation(double x, double y, double z, double w) {

	_x = x;
	_y = y;
	_z = z;
	_w = w;
}

void
RotatePainter::draw(const util::rect<double>& roi, const util::point<double>& resolution) {

	OpenGl::Guard guard;

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	glRotated(_w, _x, _y, _z);
	glTranslated(-_centerX, -_centerY, -_centerZ);

	_content->draw(roi, resolution);

	glPopMatrix();
}

void
RotatePainter::updateSize() {

	util::rect<double> size = _content->getSize();

	LOG_ALL(rotatepainterlog) << "content size is " << size << std::endl;

	_centerX = 0.5*(_content->getSize().minX + _content->getSize().maxX);
	_centerY = 0.5*(_content->getSize().minY + _content->getSize().maxY);
	_centerZ = 0;

	LOG_ALL(rotatepainterlog) << "center is at " << _centerX << ", " << _centerY << std::endl;

	// move the center to (0, 0, 0)
	size.minX -= _centerX;
	size.maxX -= _centerX;
	size.minY -= _centerY;
	size.maxY -= _centerY;

	LOG_ALL(rotatepainterlog) << "new size is " << size << std::endl;

	// the reported size should be the size of the original content
	setSize(size);
}

} // namespace gui


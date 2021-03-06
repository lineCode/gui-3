#include <util/Logger.h>
#include "ZoomView.h"

namespace gui {

static logger::LogChannel zoomviewlog("zoomviewlog", "[ZoomView] ");

ZoomView::ZoomView(bool autoscale) :
		_zoomed(new ZoomPainter()),
		_zoomStep(1.1),
		_dragging(false),
		_autoscale(autoscale) {

	registerInput(_content, "painter");
	registerOutput(_zoomed, "painter");

	_content.registerSlot(_keyDown);
	_content.registerSlot(_keyUp);
	_content.registerCallback(&ZoomView::onInputSet, this);
	_content.registerCallback(&ZoomView::onContentChanged, this);
	_content.registerCallback(&ZoomView::onSizeChanged, this);

	_zoomed.registerSlot(_contentChanged);
	_zoomed.registerSlot(_sizeChanged);
	_zoomed.registerCallback(&ZoomView::onKeyUp, this);
	_zoomed.registerCallback(&ZoomView::onKeyDown, this);
	_zoomed.registerCallback(&ZoomView::onMouseDown, this);
	_zoomed.registerCallback(&ZoomView::onMouseMove, this);

	// establish pointer signal filter
	PointerSignalFilter::filterBackward(_zoomed, _content, this);

	// establish window signal filter
	WindowSignalFilter::filterForward(_content, _zoomed, this);

	if (_autoscale) {

		_zoomed->setAutoscale();
		_zoomed.registerCallback(&ZoomView::onResize, this);
	}
}

ZoomView::ZoomView(const util::rect<double>& desiredSize) :
		_zoomed(boost::make_shared<ZoomPainter>()),
		_zoomStep(1.1),
		_dragging(false),
		_autoscale(true) {

	registerInput(_content, "painter");
	registerOutput(_zoomed, "painter");

	_content.registerSlot(_keyDown);
	_content.registerSlot(_keyUp);
	_content.registerCallback(&ZoomView::onInputSet, this);
	_content.registerCallback(&ZoomView::onContentChanged, this);
	_content.registerCallback(&ZoomView::onSizeChanged, this);

	_zoomed.registerSlot(_contentChanged);
	_zoomed.registerSlot(_sizeChanged);
	_zoomed.registerCallback(&ZoomView::onKeyUp, this);
	_zoomed.registerCallback(&ZoomView::onKeyDown, this);
	_zoomed.registerCallback(&ZoomView::onMouseDown, this);
	_zoomed.registerCallback(&ZoomView::onMouseMove, this);

	// establish pointer signal filter
	PointerSignalFilter::filterBackward(_zoomed, _content, this);

	// establish window signal filter
	WindowSignalFilter::filterForward(_content, _zoomed, this);

	_zoomed->setAutoscale();
	_zoomed->setDesiredSize(desiredSize);
}

void
ZoomView::updateOutputs() {

	LOG_ALL(zoomviewlog) << "\"updating\" output..." << std::endl;

	_zoomed->setContent(_content);
	_zoomed->updateScaleAndShift();
}
 
bool
ZoomView::filter(PointerSignal& signal) {

	signal.position = _zoomed->invert(signal.position);
	return true;
}

void
ZoomView::onInputSet(const pipeline::InputSet<Painter>& /*signal*/) {

	LOG_ALL(zoomviewlog) << "got a new painter" << std::endl;

	setDirty(_zoomed);

	_contentChanged();
}

void
ZoomView::onContentChanged(const ContentChanged& /*signal*/) {

	_contentChanged();
}

void
ZoomView::onSizeChanged(const SizeChanged& /*signal*/) {

	_sizeChanged(SizeChanged(_zoomed->getSize()));

	setDirty(_zoomed);
}

void
ZoomView::onResize(const Resize& signal) {

	LOG_ALL(zoomviewlog) << "got a resize signal, change desired size to " << signal.getSize() << std::endl;

	util::rect<double> oldSize = _zoomed->getSize();

	_zoomed->setDesiredSize(signal.getSize());

	util::rect<double> newSize = _zoomed->getSize();

	if (oldSize != newSize) {

		_sizeChanged(newSize);
		setDirty(_zoomed);
	}
}

void
ZoomView::onKeyUp(const KeyUp& signal) {

	// pass on the signal
	_keyUp(signal);
}

void
ZoomView::onKeyDown(KeyDown& signal) {

	LOG_ALL(zoomviewlog) << "a key was pressed" << std::endl;

	if (signal.key == keys::R) {

		LOG_ALL(zoomviewlog) << "resetting scale and shift" << std::endl;

		_zoomed->reset();

		setDirty(_zoomed);

		signal.processed = true;

	} else {

		_keyDown(signal);
	}
}

void
ZoomView::onMouseDown(MouseDown& signal) {

	LOG_ALL(zoomviewlog) << "a button was pressed" << std::endl;

	if (!(signal.modifiers & keys::ControlDown))
		return;

	// that is for us!
	signal.processed = true;

	util::point<double> position = signal.position;

	LOG_ALL(zoomviewlog) << "mouse button " << signal.button << " down, position is " << position << std::endl;

	if (signal.button == buttons::Left) {

		LOG_ALL(zoomviewlog) << "it's the left mouse button -- start dragging mode" << std::endl;

		_dragging = true;
		_buttonDown = position;

		return;
	}

	// if shift is pressed, increase zoom speed
	double zoomStep = _zoomStep;
	if (signal.modifiers & keys::ShiftDown)
		zoomStep *= 2;

	// mouse wheel up
	if (signal.button == buttons::WheelUp) {

		LOG_ALL(zoomviewlog) << "it's the left wheel up" << std::endl;

		_zoomed->zoom(zoomStep, position);
	}

	// mouse wheel down
	if (signal.button == buttons::WheelDown) {

		LOG_ALL(zoomviewlog) << "it's the left wheel down" << std::endl;

		_zoomed->zoom(1.0/zoomStep, position);
	}

	setDirty(_zoomed);
}

void
ZoomView::onMouseMove(MouseMove& signal) {

	LOG_ALL(zoomviewlog) << "the mouse is moved" << std::endl;

	if (!(signal.modifiers & keys::ControlDown))
		return;

	if (!_dragging) {

		return;
	}

	// that is for us!
	signal.processed = true;

	LOG_ALL(zoomviewlog) << "I am in dragging mode" << std::endl;

	double amp = 1.0;
	if (signal.modifiers & keys::ShiftDown)
		amp = 10.0;

	// mouse is dragged
	if (signal.modifiers & buttons::LeftDown) {

		LOG_ALL(zoomviewlog) << "left button is still pressed" << std::endl;

		util::point<double> moved = signal.position - _buttonDown;

		_zoomed->drag(moved*amp);

		_buttonDown = signal.position;

		setDirty(_zoomed);

	} else {

		LOG_ALL(zoomviewlog) << "left button released -- stop dragging" << std::endl;

		_dragging = false;
	}
}

} // namespace gui

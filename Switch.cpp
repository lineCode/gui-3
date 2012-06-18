#include <boost/make_shared.hpp>

#include "Switch.h"

namespace gui {

Switch::Switch(std::string name) :
	_textView(boost::make_shared<TextView>(name + ":")),
	_switchImpl(boost::make_shared<SwitchImpl>()),
	_container(boost::make_shared<ContainerView<HorizontalPlacing> >()) {

	// setup internal pipeline connections
	_container->addInput(_textView->getOutput());
	_container->addInput(_switchImpl->getOutput("painter"));

	registerOutput(_switchImpl->getOutput("value"), "value");
	registerOutput(_container->getOutput(), "painter");
}

} // namespace gui


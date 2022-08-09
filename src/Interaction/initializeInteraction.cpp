#include "initializeInteraction.h"

#include "NodeFactory.h"

#include "PickerNode.h"

namespace dyno 
{
	InteractionInitializer::InteractionInitializer()
	{
		initializeNodeCreators();
	}

	void InteractionInitializer::initializeNodeCreators()
	{
		NodeFactory* factory = NodeFactory::instance();

		auto group = factory->addGroup(
			"Interaction", 
			"Interaction", 
			"ToolBarIco/Interaction/Interaction.png");

		group->addAction(
			"Picker",
			"ToolBarIco/Interaction/Picker.png",//48px-Image-x-generic.png
			[=]()->std::shared_ptr<Node> { return std::make_shared<PickerNode<DataType3f>>(); });
	}
}
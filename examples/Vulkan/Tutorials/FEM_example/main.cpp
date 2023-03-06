#include <GlfwApp.h>

#include <SceneGraph.h>
#include <GLPointVisualModule.h>
#include "../Dynamics/Vulkan/FEM/ElasticBody.h"
#include "PointVisualModule.h"
#include "SurfaceVisualModule.h"
using namespace dyno;


int main(int, char**)
{
	VkSystem::instance()->initialize();

	GlfwApp window;
	window.initialize(1024, 768);

	auto scene = std::make_shared<SceneGraph>();

	auto elastic = scene->addNode(std::make_shared<dyno::ElasticBody>());

	elastic->loadTetGenFile(getAssetPath() + "smesh/cube.1", Vec3f(0, 1, 0));

	auto sRender = std::make_shared<SurfaceVisualModule>();
	elastic->statePosition()->connect(sRender->inPosition());
	sRender->m_triangles.assign(elastic->mIndex);
	elastic->graphicsPipeline()->pushModule(sRender);

	auto ptRender = std::make_shared<PointVisualModule>();
	elastic->statePosition()->connect(ptRender->inPosition());
	
	elastic->graphicsPipeline()->pushModule(ptRender);

	window.setSceneGraph(scene);

	window.mainLoop();

	return 0;
}

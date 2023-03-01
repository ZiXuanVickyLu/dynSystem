#include <GlfwApp.h>

#include <SceneGraph.h>

#include <HeightField/CapillaryWave.h>

#include <Mapping/HeightFieldToTriangleSet.h>

#include <GLRenderEngine.h>
#include <GLPointVisualModule.h>
#include <GLSurfaceVisualModule.h>

using namespace std;
using namespace dyno;

std::shared_ptr<SceneGraph> createScene()
{
	std::shared_ptr<SceneGraph> scn = std::make_shared<SceneGraph>();

	auto root = scn->addNode(std::make_shared<CapillaryWave<DataType3f>>());

	auto mapper = std::make_shared<HeightFieldToTriangleSet<DataType3f>>();
	root->stateTopology()->connect(mapper->inHeightField());
	root->graphicsPipeline()->pushModule(mapper);

// 	mapper->varScale()->setValue(0.1);
// 	mapper->varTranslation()->setValue(Vec3f(-2, 0.2, -2));

	auto sRender = std::make_shared<GLSurfaceVisualModule>();
	sRender->setColor(Vec3f(0, 0.2, 1.0));
	mapper->outTriangleSet()->connect(sRender->inTriangleSet());
	root->graphicsPipeline()->pushModule(sRender);

	return scn;
}

int main()
{
	GlfwApp app;
	app.initialize(1024, 768);

	app.setSceneGraph(createScene());
	app.renderWindow()->getCamera()->setUnitScale(20);

	app.mainLoop();

	return 0;
}
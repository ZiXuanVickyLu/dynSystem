#include <QtApp.h>
using namespace dyno;

#include "Node.h"
#include "Vector.h"

/**
 * @brief This example demonstrates how to show Chinese for both the node and fields
 */

class ChineseNode : public Node
{
	DECLARE_CLASS(ChineseNode);
public:
	ChineseNode() {
		this->varScalar()->setObjectName("����");
		this->varVector()->setObjectName("ʸ��");

		this->stateTimeStep()->setObjectName("ʱ�䲽��");
		this->stateElapsedTime()->setObjectName("ʱ��");
		this->stateFrameNumber()->setObjectName("��ǰ֡");
	};
	~ChineseNode() {};

	std::string caption() override {
		return "��������";
	}

	std::string description() override {
		return "����һ�����Ľڵ�";
	}

	std::string getNodeType() override {
		return "���Ľڵ�";
	}

	DEF_VAR(float, Scalar, 1.0f, "Define a scalar");

	DEF_VAR(Vec3f, Vector, 0.0f, "Define a vector");
};

IMPLEMENT_CLASS(ChineseNode);

int main()
{
	std::shared_ptr<SceneGraph> scn = std::make_shared<SceneGraph>();
	auto nickname = scn->addNode(std::make_shared<ChineseNode>());

	QtApp app;
	app.setSceneGraph(scn);
	app.initialize(1366, 800);
	app.mainLoop();

	return 0;
}
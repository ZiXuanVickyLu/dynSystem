#include <glad/glad.h>

#include "POpenGLWidget.h"
#include "PSimulationThread.h"

#include <SceneGraph.h>
#include <OrbitCamera.h>

//Qt
#include <QMouseEvent>
#include <QGuiApplication>
#include <QScreen>

#include "QtImGui.h"
#include <ImWidget.h>

#include "SceneGraphFactory.h"

#include <GLRenderEngine.h>
#include "QtApp.h"

namespace dyno
{
	POpenGLWidget::POpenGLWidget()
		: RenderWindow()
	{
		QSurfaceFormat format;
		format.setDepthBufferSize(24);
		format.setMajorVersion(4);
		format.setMinorVersion(4);
		format.setSamples(4);
		format.setSwapInterval(1);
		format.setProfile(QSurfaceFormat::CoreProfile);
		setFormat(format);

		// Update at 60 fps
		QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
		timer.start(16);

		setFocusPolicy(Qt::StrongFocus);
	}

	POpenGLWidget::~POpenGLWidget()
	{
		timer.stop();
		//delete mRenderEngine;
		
		makeCurrent();
		this->getRenderEngine()->terminate();
		doneCurrent();
	}

	void POpenGLWidget::initializeGL()
	{
		if (!gladLoadGL()) {
			//SPDLOG_CRITICAL("Failed to load GLAD!");
			exit(-1);
		}

		initializeOpenGLFunctions();
		QtImGui::initialize(this);

		// initialize render engine
		if (this->getRenderEngine() == 0) {
			auto engine = std::make_shared<GLRenderEngine>();
			this->setRenderEngine(engine);
			engine->initialize();
		}

		// Get Context scale
		float scale = QGuiApplication::primaryScreen()->logicalDotsPerInchX() / 96.0;
		mImWindow.initialize(scale);

		auto scn = SceneGraphFactory::instance()->active();

		if (scn != nullptr)
		{
			scn->reset();
			scn->updateGraphicsContext();
		}
	}

	void POpenGLWidget::paintGL()
	{
		//QtImGui
		QtImGui::newFrame();

		// Draw scene		
		auto engine = this->getRenderEngine();
		auto camera = this->getCamera();
		auto& rparams = this->getRenderParams();
		auto scene = SceneGraphFactory::instance()->active();

		rparams.proj = camera->getProjMat();
		rparams.view = camera->getViewMat();


		// Jian SHI: hack for unit scaling...
		float planeScale = rparams.planeScale;
		float rulerScale = rparams.rulerScale;
		rparams.planeScale *= this->getCamera()->unitScale();
		rparams.rulerScale *= this->getCamera()->unitScale();

		engine->draw(scene.get(), rparams);

		rparams.planeScale = planeScale;
		rparams.rulerScale = rulerScale;

		// Draw ImGui
		mImWindow.draw(this);
		// Draw widgets
// 		// TODO: maybe move into mImWindow...
// 		for (auto widget : mWidgets)
// 		{
// 			widget->update();
// 			widget->paint();
// 		}

		ImGui::Render();
		// Do QtImgui Render After Glfw Render
		QtImGui::render();
	}

	void POpenGLWidget::resizeGL(int w, int h)
	{
		this->setWindowSize(w, h);
	}

	PButtonType mappingMouseButton(QMouseEvent* event)
	{
		if (event->buttons().testFlag(Qt::LeftButton))
		{
			return PButtonType::BT_LEFT;
		}
		else if (event->buttons().testFlag(Qt::MidButton))
		{
			return PButtonType::BT_MIDDLE;
		}
		else if (event->buttons().testFlag(Qt::MidButton))
		{
			return PButtonType::BT_RIGHT;
		}
	}

	PModifierBits mappingModifierBits(Qt::KeyboardModifiers mods)
	{
		if (mods == Qt::ControlModifier)
		{
			return PModifierBits::MB_CONTROL;
		}
		else if (mods == Qt::ShiftModifier)
		{
			return PModifierBits::MB_SHIFT;
		}
		else if (mods == Qt::AltModifier)
		{
			return PModifierBits::MB_ALT;
		}
		else
			return PModifierBits::MB_NO_MODIFIER;
	}

	void POpenGLWidget::mousePressEvent(QMouseEvent *event)
	{
		auto camera = this->getCamera();
		camera->registerPoint(event->x(), event->y());
		mButtonState = QButtonState::QBUTTON_DOWN;

		PMouseEvent mouseEvent;
		mouseEvent.ray = camera->castRayInWorldSpace((float)event->x(), (float)event->y());
		mouseEvent.buttonType = mappingMouseButton(event);
		mouseEvent.actionType = PActionType::AT_PRESS;
		mouseEvent.mods = mappingModifierBits(event->modifiers());
		mouseEvent.camera = camera;
		mouseEvent.x = (float)event->x();
		mouseEvent.y = (float)event->y();

		auto activeScene = SceneGraphFactory::instance()->active();

		activeScene->onMouseEvent(mouseEvent);

		mImWindow.mousePressEvent(mouseEvent);

		updateGrpahicsContext();
	}

	void POpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
	{
		auto camera = this->getCamera();

		mButtonState = QButtonState::QBUTTON_UP;

		PMouseEvent mouseEvent;
		mouseEvent.ray = camera->castRayInWorldSpace((float)event->x(), (float)event->y());
		mouseEvent.buttonType = mappingMouseButton(event);
		mouseEvent.actionType = PActionType::AT_RELEASE;
		mouseEvent.mods = mappingModifierBits(event->modifiers());
		mouseEvent.camera = camera;
		mouseEvent.x = (float)event->x();
		mouseEvent.y = (float)event->y();

		auto activeScene = SceneGraphFactory::instance()->active();

		activeScene->onMouseEvent(mouseEvent);

		mImWindow.mouseReleaseEvent(mouseEvent);

		updateGrpahicsContext();
	}

	void POpenGLWidget::mouseMoveEvent(QMouseEvent *event)
	{
		auto camera = this->getCamera();

		if (event->buttons().testFlag(Qt::LeftButton) &&
			mButtonState == QBUTTON_DOWN &&
			event->modifiers() == Qt::AltModifier &&
			!mImWindow.cameraLocked())
		{
			camera->rotateToPoint(event->x(), event->y());
		}
		else if (event->buttons().testFlag(Qt::RightButton) &&
			mButtonState == QBUTTON_DOWN &&
			event->modifiers() == Qt::AltModifier &&
			!mImWindow.cameraLocked())
		{
			camera->translateToPoint(event->x(), event->y());
		}

		PMouseEvent mouseEvent;
		mouseEvent.ray = camera->castRayInWorldSpace((float)event->x(), (float)event->y());
		mouseEvent.buttonType = mappingMouseButton(event);
		mouseEvent.actionType = PActionType::AT_REPEAT;
		mouseEvent.mods = mappingModifierBits(event->modifiers());
		mouseEvent.camera = camera;
		mouseEvent.x = (float)event->x();
		mouseEvent.y = (float)event->y();

		auto activeScene = SceneGraphFactory::instance()->active();

		activeScene->onMouseEvent(mouseEvent);

		mImWindow.mouseMoveEvent(mouseEvent);

		updateGrpahicsContext();
	}

	void POpenGLWidget::wheelEvent(QWheelEvent *event)
	{
		if(!mImWindow.cameraLocked())
			this->getCamera()->zoom(-0.001*event->angleDelta().x());

		update();
	}

	void POpenGLWidget::updateGrpahicsContext()
	{
		makeCurrent();

		PSimulationThread::instance()->startUpdatingGraphicsContext();

		SceneGraphFactory::instance()->active()->updateGraphicsContext();

		PSimulationThread::instance()->stopUpdatingGraphicsContext();

		update();

		doneCurrent();
	}

	void POpenGLWidget::updateGraphicsContext(Node* node)
	{
		makeCurrent();

		PSimulationThread::instance()->startUpdatingGraphicsContext();

		node->graphicsPipeline()->forceUpdate();

		PSimulationThread::instance()->stopUpdatingGraphicsContext();

		update();

		doneCurrent();
	}

}
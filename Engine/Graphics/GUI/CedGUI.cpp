#include "CedGUI.h"
#include "../GraphicsManager.hpp"
#include <GLFW\glfw3.h>
#include "../../tools/Log.hpp"
#include "../../TimeManager.hpp"

CEGUI::OpenGL3Renderer* CedGUI::mp_renderer = nullptr;
CedGUI* CedGUI::mpInstance = nullptr;

CedGUI* CedGUI::getInstance() 
{
	if (mpInstance == nullptr)
	{
		mpInstance = new CedGUI;
	}

	return mpInstance;
}

void CedGUI::init(std::string resourceDirectory)
{
	// Check if the renderer and system are already initialized
	if (mp_renderer == nullptr)
	{
		mp_renderer = &CEGUI::OpenGL3Renderer::bootstrapSystem();

		CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

		rp->setResourceGroupDirectory("imagesets", resourceDirectory + "/imagesets/");
		rp->setResourceGroupDirectory("schemes", resourceDirectory + "/schemes/");
		rp->setResourceGroupDirectory("fonts", resourceDirectory + "/fonts/");
		rp->setResourceGroupDirectory("layouts", resourceDirectory + "/layouts/");
		rp->setResourceGroupDirectory("looknfeels", resourceDirectory + "/looknfeel/");
		rp->setResourceGroupDirectory("lua_scripts", resourceDirectory + "/lua_scripts/");

		CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
		CEGUI::WindowManager::setDefaultResourceGroup("layouts");
		CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	}

	mp_context = &CEGUI::System::getSingleton().createGUIContext(mp_renderer->getDefaultRenderTarget());
	mp_root = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
	mp_context->setRootWindow(mp_root);

	// mouse setup
	EventManager* pEventManager = EventManager::getInstance();
	static sEventType_t mMouseEventsLoc = MOUSE_FREEMOVE;
	pEventManager->registerObject(&mMouseEventsLoc, this);

	// keyboard setup
	for (int i = 32; i < GLFW_KEY_LAST; ++i)
	{
		this->registerKey(i);
	}	
	pEventManager->registerObject(this);
}

void CedGUI::destroy()
{
	CEGUI::System::getSingleton().destroyGUIContext(*mp_context);
}

void CedGUI::update()
{
	mp_context->injectTimePulse(TimeManager::getInstance()->elapsed());
}

void CedGUI::draw()
{
	mp_renderer->beginRendering();
	mp_context->draw();
	mp_renderer->endRendering();
	// Bug here
	glDisable(GL_SCISSOR_TEST);
}

void CedGUI::setMouseCursor(std::string imageFile)
{
	mp_context->getMouseCursor().setDefaultImage(imageFile);
}

void CedGUI::showMouseCursor()
{
	mp_context->getMouseCursor().show();
}

void CedGUI::hideMouseCursor()
{
	mp_context->getMouseCursor().hide();
}

void CedGUI::loadScheme(std::string filePath)
{
	CEGUI::SchemeManager::getSingleton().createFromFile(filePath);
}

CEGUI::Window* CedGUI::createWidget(std::string type, const glm::vec4& destRectPerc, const glm::vec4& destRectPix, std::string name)
{
	CEGUI::Window* newWindow = CEGUI::WindowManager::getSingleton().createWindow(type, name);
	mp_root->addChild(newWindow);
	setWidgetDestRect(newWindow, destRectPerc, destRectPix);

	return newWindow;
}

void CedGUI::setFont(std::string filePath)
{
	CEGUI::FontManager::getSingleton().createFromFile(filePath + ".font");
	mp_context->setDefaultFont(filePath);
}

void CedGUI::setWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destRectPerc, const glm::vec4& destRectPix)
{
	float x, y;

	x = destRectPerc.x / GraphicsManager::getInstance()->getRenderWidth();
	y = destRectPerc.y / GraphicsManager::getInstance()->getRenderHeight();

	widget->setPosition(CEGUI::UVector2(CEGUI::UDim(x, destRectPix.x), CEGUI::UDim(y, destRectPix.y)));
	widget->setSize(CEGUI::USize(CEGUI::UDim(destRectPerc.z, destRectPix.z), CEGUI::UDim(destRectPerc.w, destRectPix.w)));
}

void CedGUI::onHover(const glm::vec2& pos)
{
	mp_context->injectMousePosition(pos.x, GraphicsManager::getInstance()->getRenderHeight() - pos.y);
}

void CedGUI::onLeftClick(const glm::vec2&)
{
	mp_context->injectMouseButtonDown(CEGUI::MouseButton::LeftButton);
}

void CedGUI::onLeftDrag(const glm::vec2& pos)
{
	//mp_context->injectMouseMove(pos.x, GraphicsManager::getInstance()->getRenderHeight() - pos.y);
}

void CedGUI::onLeftRelease(const glm::vec2&)
{
	mp_context->injectMouseButtonUp(CEGUI::MouseButton::LeftButton);
}

void CedGUI::onMidClick(const glm::vec2&)
{
	mp_context->injectMouseButtonDown(CEGUI::MouseButton::MiddleButton);
}

void CedGUI::onMidRelease(const glm::vec2&)
{
	mp_context->injectMouseButtonUp(CEGUI::MouseButton::MiddleButton);
}

void CedGUI::onRightClick(const glm::vec2&)
{
	mp_context->injectMouseButtonDown(CEGUI::MouseButton::RightButton);
}

void CedGUI::onRightRelease(const glm::vec2&)
{
	mp_context->injectMouseButtonUp(CEGUI::MouseButton::RightButton);
}

void CedGUI::onScroll(float x, float y)
{
	mp_context->injectMouseWheelChange(y);
}

void CedGUI::keyboardDown(int key)
{
	mp_context->injectKeyDown((GlfwToCeguiKey(key)));
	if (key >= 'A' && key <= 'Z') mp_context->injectChar(key + 32);
	if ((key >= '0' && key <= '9') || key == ' ') mp_context->injectChar(key);
}

void CedGUI::keyboardUp(int key)
{
	mp_context->injectKeyUp((GlfwToCeguiKey(key)));
}

void CedGUI::KeyboardString(unsigned int code)
{
	mp_context->injectChar(code);
}

CEGUI::Key::Scan CedGUI::GlfwToCeguiKey(int glfwKey)
{
	switch (glfwKey)
	{
	case GLFW_KEY_UNKNOWN: return CEGUI::Key::Unknown;
	case GLFW_KEY_ESCAPE: return CEGUI::Key::Escape;
	case GLFW_KEY_F1: return CEGUI::Key::F1;
	case GLFW_KEY_F2: return CEGUI::Key::F2;
	case GLFW_KEY_F3: return CEGUI::Key::F3;
	case GLFW_KEY_F4: return CEGUI::Key::F4;
	case GLFW_KEY_F5: return CEGUI::Key::F5;
	case GLFW_KEY_F6: return CEGUI::Key::F6;
	case GLFW_KEY_F7: return CEGUI::Key::F7;
	case GLFW_KEY_F8: return CEGUI::Key::F8;
	case GLFW_KEY_F9: return CEGUI::Key::F9;
	case GLFW_KEY_F10: return CEGUI::Key::F10;
	case GLFW_KEY_F11: return CEGUI::Key::F11;
	case GLFW_KEY_F12: return CEGUI::Key::F12;
	case GLFW_KEY_F13: return CEGUI::Key::F13;
	case GLFW_KEY_F14: return CEGUI::Key::F14;
	case GLFW_KEY_F15: return CEGUI::Key::F15;
	case GLFW_KEY_UP: return CEGUI::Key::ArrowUp;
	case GLFW_KEY_DOWN: return CEGUI::Key::ArrowDown;
	case GLFW_KEY_LEFT: return CEGUI::Key::ArrowLeft;
	case GLFW_KEY_RIGHT: return CEGUI::Key::ArrowRight;
	case GLFW_KEY_LEFT_SHIFT: return CEGUI::Key::LeftShift;
	case GLFW_KEY_RIGHT_SHIFT: return CEGUI::Key::RightShift;
	case GLFW_KEY_LEFT_CONTROL: return CEGUI::Key::LeftControl;
	case GLFW_KEY_RIGHT_CONTROL: return CEGUI::Key::RightControl;
	case GLFW_KEY_LEFT_ALT: return CEGUI::Key::LeftAlt;
	case GLFW_KEY_RIGHT_ALT: return CEGUI::Key::RightAlt;
	case GLFW_KEY_TAB: return CEGUI::Key::Tab;
	case GLFW_KEY_ENTER: return CEGUI::Key::Return;
	case GLFW_KEY_BACKSPACE: return CEGUI::Key::Backspace;
	case GLFW_KEY_INSERT: return CEGUI::Key::Insert;
	case GLFW_KEY_DELETE: return CEGUI::Key::Delete;
	case GLFW_KEY_PAGE_UP: return CEGUI::Key::PageUp;
	case GLFW_KEY_PAGE_DOWN: return CEGUI::Key::PageDown;
	case GLFW_KEY_HOME: return CEGUI::Key::Home;
	case GLFW_KEY_END: return CEGUI::Key::End;
	case GLFW_KEY_KP_ENTER: return CEGUI::Key::NumpadEnter;
	default: return CEGUI::Key::Unknown;
	}
}

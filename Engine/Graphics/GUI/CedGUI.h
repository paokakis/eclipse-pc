#pragma once
#include <CEGUI\CEGUI.h>
#include <CEGUI\RendererModules\OpenGL\GL3Renderer.h>
#include <glm.hpp>
#include "../../Input/Keyboard.h"
#include "../../Input/Mouse.h"

class CedGUI : public MouseEventObject, KeyboardEventObject
{
public:

	static CedGUI* getInstance();

	void init(std::string resourceDirectory);
	void destroy();

	void draw();
	void update();

	void setMouseCursor(std::string imageFile);
	void showMouseCursor();
	void hideMouseCursor();
	
	void loadScheme(std::string filePath);
	CEGUI::Window* createWidget(std::string type, const glm::vec4& destRectPerc, const glm::vec4& destRectPix, std::string name = "");
	void setFont(std::string filePath);
	static void setWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destRectPerc, const glm::vec4& destRectPix);

	// Getters
	static CEGUI::OpenGL3Renderer* getRenderer() { return mp_renderer; }
	const CEGUI::GUIContext* getContext() const { return mp_context; }

protected:
	void onHover(const glm::vec2&);
	void onLeftClick(const glm::vec2&);
	void onLeftDrag(const glm::vec2&);
	void onLeftRelease(const glm::vec2&);
	void onMidClick(const glm::vec2&);
	void onMidRelease(const glm::vec2&);
	void onRightClick(const glm::vec2&);
	void onRightRelease(const glm::vec2&);
	void onScroll(float x, float y);

	void keyboardDown(int key);
	void keyboardUp(int key);
	void KeyboardString(unsigned int code);

private:
	static CedGUI* mpInstance;
	static CEGUI::OpenGL3Renderer* mp_renderer;
	CEGUI::GUIContext* mp_context = nullptr;
	CEGUI::Window* mp_root = nullptr;

	glm::vec2 mMousePosition;

private:
	explicit CedGUI() {}
	explicit CedGUI(CedGUI&) {}
	CEGUI::Key::Scan GlfwToCeguiKey(int glfwKey);
};


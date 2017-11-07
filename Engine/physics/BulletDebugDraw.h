#pragma once
#include "../Graphics/GraphicsManager.hpp"
#include "../Graphics/GUI/Primitives/Primates.h"
#include "../physics/BulletPhysicsManager.h"
#include "../Graphics/Text2D.h"
#include "../tools/Log.hpp"

class BulletDebugDraw : public btIDebugDraw
{
public:
	BulletDebugDraw(GraphicsManager& graphMan) : mGraphicsManager(graphMan), mText(graphMan), mPrimitives(graphMan)
	{
		Log::info("Debug draw constructor");
		mText.initialize(30);
	}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		glm::vec3 vfrom;
		glm::vec3 vto;
		glm::vec3 vcolor;

		vfrom.x = from.x();
		vfrom.y = from.y();
		vfrom.z = from.z();

		vto.x = to.x();
		vto.y = to.y();
		vto.z = to.z();

		vcolor.r = color.x();
		vcolor.g = color.y();
		vcolor.b = color.z();
		vcolor.z = 1;
		mPrimitives.addLine(vfrom, vto, vcolor);
	}

	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		btVector3 to = PointOnB + normalOnB * distance;
		const btVector3&from = PointOnB;
		glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);   

		drawLine(from, to, color);
	}

	void reportErrorWarning(const char* warningString)
	{
		Log::error("%s", warningString);
	}

	void draw3dText(const btVector3& location, const char* textString)
	{
		mText.RenderText(textString, glm::vec2(location.x(), location.y()), 1.f, glm::vec3(1, 1, 1));
	}

	void setDebugMode(int debugMode)
	{
		mDebugMode = debugMode;
	}

	int		getDebugMode() const
	{
		return mDebugMode;
	}

private:
	GraphicsManager& mGraphicsManager;
	Primates mPrimitives;
	Text2D mText;
	int mDebugMode;
};
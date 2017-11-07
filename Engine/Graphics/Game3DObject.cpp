#include "Game3DObject.h"


status Game3DObject::load()
{
	mGraphicsManager.glErrorCheck();

	return STATUS_OK;
}

void Game3DObject::update()
{
	for (objIter = objects.begin(); objIter != objects.end(); ++objIter)
	{
		(*objIter)->pGraphObj->setPosition((*objIter)->pMoveableBody->getPosition());
		(*objIter)->pGraphObj->setRotation((*objIter)->pMoveableBody->getRotation());

		(*objIter)->pGraphObj->update();
		(*objIter)->pMoveableBody->update();
	}
}

void Game3DObject::draw()
{
	for (objIter = objects.begin(); objIter != objects.end(); ++objIter)
	{
		(*objIter)->pGraphObj->draw();
	}
}

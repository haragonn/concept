#include "../../h/Texture/Mask.h"
#include "../../h/Framework/GraphicManager.h"

bool Mask::DrawMask(bool bVisible)
{
	return GraphicManager::Instance().DrawMask(bVisible);
}

bool Mask::DrawAnd()
{
	return GraphicManager::Instance().DrawAnd();
}

bool Mask::DrawXor()
{
	return GraphicManager::Instance().DrawXor();
}

bool Mask::EndMask()
{
	return GraphicManager::Instance().EndMask();
}

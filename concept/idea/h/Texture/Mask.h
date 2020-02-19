#ifndef INCLUDE_IDEA_MASK_H
#define INCLUDE_IDEA_MASK_H

class Mask{
public:
	bool DrawMask(bool bVisible = false);
	bool DrawAnd();
	bool DrawXor();
	bool EndMask();
};

#endif	// #ifndef INCLUDE_IDEA_MASK_H
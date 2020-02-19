/*==============================================================================
	[Scene.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SCENE_H
#define INCLUDE_IDEA_SCENE_H

//------------------------------------------------------------------------------
// �N���X���@�FScene
// �N���X�T�v�F�Q�[���V�[���N���X�̊��ƂȂ�N���X
//------------------------------------------------------------------------------
class Scene{
public:
	Scene() : bAsync_(false){}	// �R���X�g���N�^
	virtual ~Scene(){}			// �f�X�g���N�^

	virtual void Init(){}			// ������
	virtual void UnInit(){}			// �I������

	virtual Scene* Update() = 0;	// �X�V

	virtual void Draw() = 0;		// �`��

private:
	friend class Sequence;
	friend class AsyncLoadScene;

	bool bAsync_;	// �񓯊��ǂݍ��݃t���O
};

#endif	// #ifndef INCLUDE_IDEA_SCENE_H
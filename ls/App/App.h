#pragma once

class ResourceManager;
class EffectManager;
class SceneManager;
class BGMManager;
class SaveDataManager;
class CollideSystem;


///////////////////////////////////////////////////////////////////////////////
//
//	�A�v���P�[�V����
//
///////////////////////////////////////////////////////////////////////////////
class App
{
public:
	App( void );
	~App();

	void					Init( void );
	void					Update( void );

	bool					IsEnd( void )			const;

	void					OnException( pstr pDirPath );

	ResourceManager*		GetResMng( void )				{ return m_pResourceManager; }
	EffectManager*			GetEffectMng( void )			{ return m_pEffectManager; }
	BGMManager*				GetBGMMng( void )				{ return m_pBGMManager; }
	SaveDataManager*		GetSaveDataMng( void )			{ return m_pSaveDataManager; }

private:
	ResourceManager*		m_pResourceManager;
	SceneManager*			m_pSceneManager;
	EffectManager*			m_pEffectManager;
	BGMManager*				m_pBGMManager;
	SaveDataManager*		m_pSaveDataManager;
};



//-----------------------------------------------------------------------------
//	�C���X�^���X�Ǘ�
//-----------------------------------------------------------------------------
void						InitApp( void );
void						ExitApp( void );
App*						GetApp( void );

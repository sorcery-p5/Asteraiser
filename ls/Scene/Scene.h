#pragma once

class SceneManager;


///////////////////////////////////////////////////////////////////////////////
//
//	ÉVÅ[ÉìäÓíÍ
//
///////////////////////////////////////////////////////////////////////////////
class Scene
{
public:
	Scene( Label Name );
	virtual ~Scene()														{}

	virtual void			OnInit( void )									{}
	virtual void			Update( void )									{}
	
	virtual void			OnException( pstr pDirPath )					{}

	Label					GetName( void )							const	{ return m_Name; }
	void					SetSceneManager( SceneManager* pMng )			{ m_pSceneManager = pMng; }

protected:
	void					Exit( void );
	void					ChangeScene( Scene* pScene );

private:
	SceneManager*			m_pSceneManager;
	Label					m_Name;
};
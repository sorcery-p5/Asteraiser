#pragma once

class Scene;


///////////////////////////////////////////////////////////////////////////////
//
//	ÉVÅ[Éìä«óù
//
///////////////////////////////////////////////////////////////////////////////
class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void				Update( void );

	void				ChangeScene( Scene* pScene );
	void				Exit( void );

	void				OnException( pstr pDirPath );

	bool				IsExit( void )			const	{ return m_bExit; }

private:
	Scene*				m_pCurScene;
	Scene*				m_pNextScene;
	bool				m_bExit;
};
#pragma once

class World;
class Player;
class PlayerData;


////////////////////////////////////////////////////////////////////////////////
//
//	ÉvÉåÉCÉÑÅ[ä«óù
//
////////////////////////////////////////////////////////////////////////////////
class PlayerManager
{
public:
	PlayerManager( World* pWorld );
	~PlayerManager();

	void				Update( void );
	void				Draw( void );

	void				AddPlayer( const PlayerData* pData, Point Pos );
	void				ChangePlayer( const PlayerData* pData );
	void				OnContinue( void );

	Player*				GetPlayer( void )						{ return m_vPlayer.front(); }
	Player*				GetPlayer( int PlayerID )				{ return m_vPlayer[PlayerID]; }
	const Player*		GetPlayer( int PlayerID )		const	{ return m_vPlayer[PlayerID]; }
	int					GetPlayerNum( void )			const	{ return (int)m_vPlayer.size(); }
	World*				GetWorld( void )						{ return m_pWorld; }
	Rand&				GetRand( void )							{ return m_Rand; }

private:
	typedef std::vector< Player* >		VecPlayer;

private:
	World*				m_pWorld;
	VecPlayer			m_vPlayer;
	XORRand				m_Rand;
};
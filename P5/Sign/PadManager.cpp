#include "P5.h"
#include "PadManager.h"


namespace P5
{

//-----------------------------------------------------------------------------
//	�񋓃R�[���o�b�N
//-----------------------------------------------------------------------------
static BOOL CALLBACK EnumJoysticks( const DIDEVICEINSTANCE* pdidInstance, void* pContext );

///////////////////////////////////////////////////////////////////////////////
//
//	�p�b�h�Ǘ�
//
///////////////////////////////////////////////////////////////////////////////
PadManager::PadManager( void )
{
	m_pDinput	= NULL;
}
PadManager::~PadManager()
{
	DeleteVec( m_vPad );
	SafeRelease( m_pDinput );
}

//*****************************************************************************
//	������
//*****************************************************************************
bool PadManager::Init( HINSTANCE Hinst )
{
	Log( "�@�@���p�b�h�̏�����" );

	HRESULT hr;

	//	DInput������
	hr = DirectInput8Create( Hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDinput, NULL ); 
	if( FAILED(hr) || !m_pDinput )
	{
		Log( "�@�@�~DirectInput�̏��������s" );
		return false;
	}

	//	�W���C�X�e�B�b�N�̐��𒲂ׂ�
	uint Num = joyGetNumDevs();
	if( Num == JOYERR_UNPLUGGED )
	{
		Log( "�@�@���p�b�h�𖢌��o" );
		return true;
	}

	// �W���C�X�e�B�b�N��񋓂���
	hr = m_pDinput->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumJoysticks, this, DIEDFL_ATTACHEDONLY );
	if( FAILED(hr) )
	{
		Log( "�@�@���p�b�h�̗񋓂Ɏ��s" );
		return false;
	}

	Log( "�@�@���p�b�h�̏���������" );
	Log( "�@�@----------------------------------------" );

	return true;
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
bool PadManager::Update( void )
{
	for( VecPad::iterator it = m_vPad.begin(); it != m_vPad.end(); ++it )
	{
		if( !(*it)->Update() ) return false;
	}

	return true;
}


//-----------------------------------------------------------------------------
//	�W���C�X�e�B�b�N�񋓃R�[���o�b�N
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticks( const DIDEVICEINSTANCE* pdidInstance, void* pContext )
{
	PadManager* pPadMng = (PadManager*)(pContext);

	Pad* pPad = new Pad( pPadMng );
	if( pPad )
	{
		if( pPad->Init( pdidInstance ) )
		{
			//	�p�b�h�ǉ�
			pPadMng->AddPad( pPad );
		}
		else
		{
			SafeDelete( pPad );
		}
	}

	return DIENUM_CONTINUE;
}

}
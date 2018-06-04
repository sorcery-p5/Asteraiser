#include "P5.h"

#include "DrawDevice.h"
#include "DrawItem/DrawItemManager.h"

#include "System/Timer.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�O���t�B�N�X
//
///////////////////////////////////////////////////////////////////////////////
Graphix::Graphix( void )
{
	m_pDevice		= NULL;
	m_pDrawItemMng	= NULL;
	m_pTimer		= NULL;

	m_FrameSkip			= 0;
	m_FrameSkipCount	= 0;
}

Graphix::~Graphix()
{
	SafeDelete( m_pTimer );
	SafeDelete( m_pDrawItemMng );
	SafeDelete( m_pDevice );
}

//*****************************************************************************
//	������
//*****************************************************************************
bool Graphix::Init( Window* pWindow )
{
	if( !pWindow ) return false;

	Log( "�@���O���t�B�b�N�X������" );

	//	�f�o�C�X������
	m_pDevice = new DrawDevice;
	if( !m_pDevice ) return false;	
	if( !m_pDevice->Init( pWindow ) ) return false;

	//	�X�v���C�g�Ǘ�
	m_pDrawItemMng = new DrawItemManager;
	if( !m_pDrawItemMng ) return false;

	//	�^�C�}�[
	m_pTimer = new Timer;
	if( !m_pTimer ) return false;

	Log( "�@���O���t�B�b�N�X����������" );
	Log( "�@----------------------------------------" );

	return true;
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
bool Graphix::Update( void )
{
	bool bRes = true;

	if( m_FrameSkip <= 1 || m_FrameSkipCount == 0 )
	{
		if( m_pDevice->Begin() )
		{
			//	�`��A�C�e���`��
			m_pDrawItemMng->Render();
		}
		bRes = m_pDevice->End();

		//	�^�C�}�[�ŃE�F�C�g���Ƃ�
		if( m_pDevice->IsTimerWait() ) m_pTimer->Wait();
	}
	else
	{
		//	�X�L�b�v
		m_pDrawItemMng->ClearDrawItem();
	}

	//	�t���[���X�L�b�v
	if( m_FrameSkip > 1 )
	{
		m_FrameSkipCount++;
		if( m_FrameSkipCount >= m_FrameSkip ) m_FrameSkipCount = 0;
	}

	return bRes;
}

//*****************************************************************************
//	�f�o�C�X�̃��Z�b�g
//*****************************************************************************
void Graphix::Reset( void )
{
	//	���\�[�X�����
	for( VecResetListener::iterator it = m_vResetListener.begin(); it != m_vResetListener.end(); ++it )
	{
		(*it)->OnDrawDeviceReset();
	}

	//	�f�o�C�X�����Z�b�g
	m_pDevice->Reset();

	//	���\�[�X�𕜋A
	for( VecResetListener::iterator it = m_vResetListener.begin(); it != m_vResetListener.end(); ++it )
	{
		(*it)->OnDrawDeviceRestore();
	}
}

//******************************************************************************
//	���Z�b�g�ʒm�C���^�t�F�[�X�̒ǉ�
//******************************************************************************
void Graphix::AddResetListener( DeviceResetListener* pListener )
{
	if( !pListener ) return;

	m_vResetListener.push_back( pListener );
}

//******************************************************************************
//	���Z�b�g�ʒm�C���^�t�F�[�X�̍폜
//******************************************************************************
void Graphix::RemoveResetListener( DeviceResetListener* pListener )
{
	VecResetListener::iterator it = StdFind( m_vResetListener, pListener );

	if( it != m_vResetListener.end() )
	{
		m_vResetListener.erase( it );
	}
}

//******************************************************************************
//	��������u�����h���[�h�֕ϊ�
//******************************************************************************
BLEND_MODE Graphix::StrToBlendMode( pstr pStr )
{
	pstr aBlend[] =
	{
		"NONE",
		"ALPHA",
		"ADD",
		"SUB",
		"MUL",
		"REV",
	};

	return BLEND_MODE( StrToInt( pStr, aBlend, ArrayNum(aBlend), BLEND_ALPHA ) );
}

}
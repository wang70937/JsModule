#include "JsTcpClient.h"

namespace hpsocket{
	CJsTcpClient::CJsTcpClient(CCLientListener* pListener)
	{
		m_pTcp = NULL;
		m_pTcp = new CTcpClient(pListener);
		pListener->SetTcpClient(this);
	}


	CJsTcpClient::~CJsTcpClient()
	{
	}

	void CJsTcpClient::Create(CCLientListener* pListener)
	{
		m_pTcp = new CTcpClient(pListener);
		pListener->SetTcpClient(this);
	}

	void CJsTcpClient::Start(LPCTSTR pszHost, UINT nPort)
	{
		m_pTcp->Start(pszHost, nPort);
	}

	void CJsTcpClient::Send(const BYTE* p, int nLenght)
	{
		m_pTcp->Send(p, nLenght);
	}

}//namespace hpsocket
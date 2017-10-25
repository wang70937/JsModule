#pragma once
#include "src/SocketInterface.h"
#include "src/TcpClient.h"
#include "v8.h"

namespace hpsocket{

	////////////
	enum EnAppStateA
	{
		ST_STARTINGA, ST_STARTEDA, ST_CONNECTINGA, ST_CONNECTEDA, ST_STOPPINGA, ST_STOPPEDA
	};

	class CCLientListener;

	//////////////////////////////////////
	class CJsTcpClient
	{
	public:
		CJsTcpClient(CCLientListener* pListener);
		~CJsTcpClient();

	private:
		CTcpClient*		m_pTcp;
		std::string		m_strCallbackOnSend;
		v8::Isolate*	m_isolate;

	public:
		void Create(CCLientListener* pListener);

		void Start(LPCTSTR pszHost, UINT nPort);

		void Send(const BYTE* p, int nLenght);


		void SetCallbackOnSend(v8::Local<v8::Function> js_callback)
		{
			v8::Local<v8::Value> vFuncName = js_callback->GetName();
			v8::String::Utf8Value vStrName(vFuncName);
			const char* strName = *vStrName;
			m_strCallbackOnSend = strName;

			m_isolate = js_callback->GetIsolate();
		}

		void CallbackOnSend(int iLength)
		{
			/*v8::Isolate* isolate = m_isolate;
			v8::Local<v8::Context> context = isolate->GetCurrentContext();

			v8::Local<v8::String> funName = v8::String::NewFromUtf8(isolate, m_strCallbackOnSend.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

			v8::Context::Scope context_scope(context);

			v8::Local<v8::Object> gObj = context->Global();
			v8::Local<v8::Value> value = gObj->Get(funName);
			v8::Local<v8::Function> fun_execute = v8::Local<v8::Function>::Cast(value);

			//
			v8::Local<v8::Value> args[2];

			v8::Local<v8::Number> numNow = v8::Number::New(isolate, iLength);
			v8::Local<v8::Number> numTotal = v8::Number::New(isolate, iLength);

			args[0] = numNow;
			args[1] = numTotal;
			fun_execute->Call(gObj, 2, args);*/
		}
	};


	class CCLientListener : public CTcpClientListener
	{
	public:
		CCLientListener(){};
		~CCLientListener(){};

		void SetTcpClient(CJsTcpClient* pClient)
		{
			m_pTcpClient = pClient;
		}

		virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{

			return HR_OK;
		}
		virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{
			

			return HR_OK;
		}
		virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
		{
			m_enState = ST_STOPPEDA;
			return HR_OK;
		}
		virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID)
		{
			m_pTcpClient->CallbackOnSend(dwConnID);
			return HR_OK;
		}
		virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID)
		{
			m_enState = ST_STARTEDA;
			//std::string str = "asdfwer2342342342342wefrsdf";
			//pSender->Send((const BYTE*)str.c_str(), str.length());
			return HR_OK;
		}

	private:
		EnAppStateA		m_enState;
		CJsTcpClient*	m_pTcpClient;
	};

}//namspace hpsocket
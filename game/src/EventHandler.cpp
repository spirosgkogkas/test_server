#include "stdafx.h"
#include "EventHandler.h"

CEvent::CEvent(boost::asio::io_context& context, std::uint64_t id) : m_timer{ context }, m_id{ id }, 
																	 m_time{ boost::posix_time::seconds(0) }, m_isRunning{ false },
																	 m_pData{ nullptr }, m_isAlwaysRunning{ false },
																	 m_callBack{nullptr}
{
}

CEvent::~CEvent()
{
	if (m_pData)
		m_pData = nullptr;

	Cancel();
}

std::uint64_t CEvent::GetID() const
{
	return m_id;
}

void CEvent::SetDurationTime(boost::posix_time::time_duration time)
{
	m_time = time;
}

void CEvent::SetCallback(TEventCallBackType f)
{
	m_callBack = f;
}

void CEvent::Start()
{
	assert(m_callBack);

	m_timer.expires_from_now(m_time);
	// Start an asynchronous wait.
	m_timer.async_wait(std::bind(&CEvent::OnTimeOut, shared_from_this(), std::placeholders::_1));
	m_isRunning = true;
}

bool CEvent::IsRunning()
{
	return m_isRunning;
}

void CEvent::OnTimeOut(const boost::system::error_code& error)
{
	if (!error)
	{
		if (m_isAlwaysRunning)
		{
			if (m_callBack)
			{
				if (m_callBack(shared_from_this()))
					Start();
				else
					ForceStop();
			}
			else
				ForceStop();
		}
		else
		{
			if (m_callBack)
				m_callBack(shared_from_this());

			ForceStop();
		}
	}
	else
	{
		ForceStop();
	}
}

void CEvent::SetAlwaysRunning()
{
	m_isAlwaysRunning = true;
}

void CEvent::SetData(std::shared_ptr<SEventBaseInfo> pData)
{
	m_pData = pData;
}

void CEvent::ForceStop()
{
	m_isRunning = false;
	CEventHandler::instance().DestroyEvent(m_id);

}

void CEvent::Cancel()
{
	if (!m_isRunning) return;

	std::size_t c = m_timer.cancel();

	if (c > 0)
		m_isRunning = false;
}

CEventHandler::CEventHandler(boost::asio::io_context& context) : m_context(context), m_allocID{ 0 }
{
	m_eventMap.clear();
}

CEventHandler::~CEventHandler()
{
	StopAllEvents();
}

void CEventHandler::Destroy()
{
	m_eventMap.clear();
}

void CEventHandler::DestroyEvent(CEventPtr pEvent)
{
	if (pEvent)
		DestroyEvent(pEvent->GetID());
}

void CEventHandler::DestroyEvent(std::uint64_t id)
{
	auto it = m_eventMap.find(id);
	if (it == m_eventMap.end())
		return;

	if (it->second->IsRunning())
		it->second->Cancel();

	m_eventMap.erase(it);
}

void CEventHandler::StopAllEvents()
{
	m_eventMap.clear();
}

CEventHandler::CEventPtr CEventHandler::FindEvent(std::uint64_t id)
{
	auto it = m_eventMap.find(id);
	if (it == m_eventMap.end())
		return nullptr;

	return it->second;
}

std::uint64_t CEventHandler::MakeEvent(boost::posix_time::time_duration when, CEvent::TEventCallBackType f, std::shared_ptr< SEventBaseInfo> pData, bool isAlwaysRun)
{
	auto pEvent = std::make_shared<CEvent>(m_context, ++m_allocID);
	pEvent->SetDurationTime(when);
	pEvent->SetData(pData);
	pEvent->SetCallback(f);
	if (isAlwaysRun)
		pEvent->SetAlwaysRunning();

	m_eventMap[pEvent->GetID()] = pEvent;
	pEvent->Start();

	return pEvent->GetID();
}
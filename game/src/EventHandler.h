#pragma once
#include <boost/asio.hpp>

struct SEventBaseInfo
{
public:
	SEventBaseInfo() {}
	virtual ~SEventBaseInfo() {}
};

class CEvent : public std::enable_shared_from_this<CEvent>
{
public:
	CEvent(boost::asio::io_context& context, std::uint64_t id);
	~CEvent();
	typedef std::function<bool(const std::shared_ptr<CEvent>&)> TEventCallBackType;

public:
	std::uint64_t GetID() const;
	void SetDurationTime(boost::posix_time::time_duration time);
	void SetCallback(TEventCallBackType f);

	template <class T> void SetCallback(T&& func)
	{
		m_callBack = func;
	}

	template <class T> void SetCallback(T& func)
	{
		m_callBack = func;
	}
	void Start();

	void OnTimeOut(const boost::system::error_code& error);

	bool IsRunning();
	void SetAlwaysRunning();

	void Cancel();
	void SetData(std::shared_ptr<SEventBaseInfo> pData);

	template <class T> std::shared_ptr<T> GetData() {
		if (!m_pData) return nullptr;

		return std::dynamic_pointer_cast<T>(m_pData);
	}

	void ForceStop();
private:
	std::uint64_t m_id;
	boost::asio::deadline_timer m_timer;
	boost::posix_time::time_duration m_time;
	bool m_isRunning;
	bool m_isAlwaysRunning;
	TEventCallBackType m_callBack;
	std::shared_ptr<SEventBaseInfo> m_pData;
};

class CEventHandler : public singleton<CEventHandler>
{

public:
	CEventHandler(boost::asio::io_context& context);
	~CEventHandler();

	typedef std::shared_ptr<CEvent> CEventPtr;
	typedef std::unordered_map<std::uint64_t, CEventPtr> TEventMap;

	void Destroy();
	void DestroyEvent(CEventPtr pEvent);
	void DestroyEvent(std::uint64_t id);
	void StopAllEvents();

	std::uint64_t MakeEvent(boost::posix_time::time_duration when, CEvent::TEventCallBackType f, std::shared_ptr<SEventBaseInfo> pData = nullptr, bool isAlwaysRun = false);
	
	template <class T> std::uint64_t MakeEvent(boost::posix_time::time_duration when, T&& f, std::shared_ptr<SEventBaseInfo> pData = nullptr, bool isAlwaysRun = false)
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

	template <class T> std::uint64_t MakeEvent(boost::posix_time::time_duration when, T& f, std::shared_ptr<SEventBaseInfo> pData = nullptr, bool isAlwaysRun = false)
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

	CEventPtr FindEvent(std::uint64_t id);

private:
	boost::asio::io_service& m_context;
	std::uint64_t m_allocID;
	TEventMap m_eventMap; 
};

#define EVENT_HANDLER_STRUCT(name) struct name : public SEventBaseInfo


class AZ_VampClientLife
{
	static const int LIMIT = 512;
	static int s_Sequence;
	static string s_Run;
	static int s_Attempt;
	static bool s_AttemptOpen;
	static bool s_AttemptEnded;
	static int s_Events;
	static int s_LostAt = -1;
	static bool s_LostKnown;
	static bool s_Lost;
	static bool s_QueueSeen;
	static int s_QueueStartedAt;
	static bool s_AdmissionSeen;
	static int s_QueueAliveAt = -1;
	static bool s_QueueCancelSeen;
	static int s_CompletedEvents;
	static const int BRACKET_CAP = 3;
	static int s_CountdownCancelCalls;
	static int s_PreloadCalls;
	static int s_StoreLoginCalls;

	static bool CanRecord(int count, bool terminal)
	{
		if (count >= 512)
		{
			return false;
		}
		return terminal || count < 480;
	}

	static bool ThrottleReady(int now, int last, int interval)
	{
		return last == -1 || now < last || now - last >= interval;
	}

	static bool EventUnseen(int seen, int flag)
	{
		return (seen & flag) == 0;
	}

	static void Mark(string phase, string detail = "", bool terminal = false)
	{
		if (!GetGame() || GetGame().IsDedicatedServer() || !CanRecord(s_Sequence, terminal))
			return;
		if (s_Run == "")
		{
			int y, mo, d, h, mi, sec;
			GetYearMonthDayUTC(y, mo, d);
			GetHourMinuteSecondUTC(h, mi, sec);
			s_Run = y.ToString() + "-" + mo + "-" + d + "T" + h + "-" + mi + "-" + sec + "Z_" + GetGame().GetTime();
			Mark("run_start", "scope=local_only");
		}
		s_Sequence++;
		string line = "[AZ_UI] run=" + s_Run + " attempt=" + s_Attempt + " seq=" + s_Sequence;
		line += " ms=" + GetGame().GetTime();
		line += " phase=" + phase;
		if (!terminal && s_Sequence == 480)
			line += " routineBudgetExhausted=true";
		if (s_Sequence == LIMIT)
			line += " limitReached=true";
		Print(line + " " + detail);
	}

	static bool OpensAttempt(bool opened, bool ended)
	{
		return !opened || ended;
	}

	static void BeginAttempt()
	{
		if (!OpensAttempt(s_AttemptOpen, s_AttemptEnded))
			return;
		s_Attempt++;
		s_AttemptOpen = true;
		s_AttemptEnded = false;
		s_Events = 0;
		s_LostAt = -1;
		s_LostKnown = false;
		s_QueueSeen = false;
		s_QueueStartedAt = 0;
		s_AdmissionSeen = false;
		s_QueueAliveAt = -1;
		s_QueueCancelSeen = false;
		s_CompletedEvents = 0;
		s_CountdownCancelCalls = 0;
		s_PreloadCalls = 0;
		s_StoreLoginCalls = 0;
		Mark("attempt_context", "route=unknown");
	}



	static void ObserveQueuePosition(int position, bool isStatic)
	{
		if (!GetGame() || GetGame().IsDedicatedServer() || s_QueueSeen || s_AdmissionSeen || position <= 0)
			return;
		s_QueueSeen = true;
		s_QueueStartedAt = GetGame().GetTime();
		s_QueueAliveAt = s_QueueStartedAt;
		Mark("queue_enter", "position=" + position + " ui=" + UiKind(isStatic));
	}

	// Which vanilla UI object carried the callback: the -connect/static dialog or the mission/menu-created one.
	// This is not a launcher-versus-browser-versus-invite classification; that stays route=unknown.
	static string UiKind(bool isStatic)
	{
		if (isStatic)
			return "static";
		return "menu";
	}

	// Uses the vanilla SetPosition polling path: no new timer or network operation.
	static void ObserveQueueLiveness(int position)
	{
		if (!GetGame() || GetGame().IsDedicatedServer() || !s_QueueSeen || s_AdmissionSeen || s_AttemptEnded || position <= 0)
			return;
		int now = GetGame().GetTime();
		if (!ThrottleReady(now, s_QueueAliveAt, 30000))
			return;
		s_QueueAliveAt = now;
		Mark("queue_alive", "position=" + position + " waitMs=" + QueueWaitMs(now));
	}

	static bool BeginQueueCancellation()
	{
		if (s_QueueCancelSeen)
			return false;
		s_QueueCancelSeen = true;
		Mark("queue_cancel_enter", "scope=script_handler", true);
		return true;
	}

	// Final countdown -> preload -> login data window. Each bracket is bounded per attempt and
	// returns its call index (0 = not traced) so enter and exit records pair up.
	static int BeginCountdownCancellation()
	{
		if (s_CountdownCancelCalls >= BRACKET_CAP)
			return 0;
		s_CountdownCancelCalls++;
		Mark("countdown_cancel_enter", "scope=script_handler call=" + s_CountdownCancelCalls, true);
		return s_CountdownCancelCalls;
	}

	static int BeginPreload()
	{
		if (s_PreloadCalls >= BRACKET_CAP)
			return 0;
		s_PreloadCalls++;
		Mark("preload_enter", "scope=script_handler call=" + s_PreloadCalls, true);
		return s_PreloadCalls;
	}

	// Routine, not terminal: it is nested inside the preload bracket, so a preload exit already proves it returned.
	static int BeginStoreLogin()
	{
		if (s_StoreLoginCalls >= BRACKET_CAP)
			return 0;
		s_StoreLoginCalls++;
		Mark("store_login_enter", "scope=script_handler call=" + s_StoreLoginCalls);
		return s_StoreLoginCalls;
	}

	static int CompletionFlag(EventType eventTypeId)
	{
		switch (eventTypeId)
		{
			case MPSessionFailEventTypeID: return 4;
			case MPSessionEndEventTypeID: return 8;
			case MPSessionPlayerReadyEventTypeID: return 16;
			case ConnectingAbortEventTypeID: return 32;
			case MPConnectionCloseEventTypeID: return 64;
		}
		return 0;
	}

	static void CompleteEvent(EventType eventTypeId, int observedAttempt)
	{
		int flag = CompletionFlag(eventTypeId);
		if (flag == 0 || observedAttempt != s_Attempt || !EventUnseen(s_CompletedEvents, flag))
			return;
		s_CompletedEvents = s_CompletedEvents | flag;
		Mark("event_handler_return", "eventFlag=" + flag + " scope=script_handler", true);
	}

	static int QueueWaitMs(int now)
	{
		if (!s_QueueSeen || now < s_QueueStartedAt)
			return -1;
		return now - s_QueueStartedAt;
	}

	static bool BeginQueueAdmission(int loginTime)
	{
		if (!GetGame() || GetGame().IsDedicatedServer() || s_AdmissionSeen)
			return false;
		s_AdmissionSeen = true;
		int wait = QueueWaitMs(GetGame().GetTime());
		Mark("queue_admission_enter", "waitMs=" + wait + " countdownSec=" + loginTime, true);
		return true;
	}

	static void ObserveEvent(EventType eventTypeId, Param params)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		int flag;
		string phase;
		string detail;
		bool terminal;
		switch (eventTypeId)
		{
			case ConnectingStartEventTypeID:
				BeginAttempt();
				flag = 1;
				phase = "connecting_start";
				break;
			case MPSessionStartEventTypeID:
				BeginAttempt();
				flag = 2;
				phase = "session_start";
				break;
			case MPSessionFailEventTypeID:
				flag = 4;
				phase = "session_fail";
				terminal = true;
				break;
			case MPSessionEndEventTypeID:
				flag = 8;
				phase = "session_end";
				terminal = true;
				break;
			case MPSessionPlayerReadyEventTypeID:
				flag = 16;
				phase = "player_ready";
				break;
			case ConnectingAbortEventTypeID:
				flag = 32;
				phase = "connecting_abort";
				terminal = true;
				break;
			case MPConnectionCloseEventTypeID:
				flag = 64;
				phase = "connection_close_callback";
				terminal = true;
				MPConnectionCloseEventParams closed;
				if (Class.CastTo(closed, params))
					detail = "kickCode=" + closed.param1;
				else
					detail = "kickCodeUnavailable=true";

				break;
			case MPConnectionLostEventTypeID:
				MPConnectionLostEventParams lost;
				if (Class.CastTo(lost, params))
				{
					bool isLost = lost.param1 >= 0;
					int now = GetGame().GetTime();
					if (!s_LostKnown || isLost != s_Lost || (isLost && ThrottleReady(now, s_LostAt, 10000)))
					{
						s_LostKnown = true;
						s_Lost = isLost;
						s_LostAt = now;
						Mark("connection_state", "lost=" + isLost);
					}
				}
				return;
			default:
				return;
		}
		if (!EventUnseen(s_Events, flag))
			return;
		s_Events = s_Events | flag;
		Mark(phase, detail, terminal);
		if (terminal)
			s_AttemptEnded = true;
	}

}

modded class DayZGame
{
	protected int m_AZLifeAppActive = -1;

	override void CancelLoginQueue()
	{
		bool traceCancellation = AZ_VampClientLife.BeginQueueCancellation();
		int diagnosticAttempt = AZ_VampClientLife.s_Attempt;
		super.CancelLoginQueue();
		if (traceCancellation && diagnosticAttempt == AZ_VampClientLife.s_Attempt)
			AZ_VampClientLife.Mark("queue_cancel_exit", "scope=script_handler", true);
	}

	override void CancelLoginTimeCountdown()
	{
		int cancelCall = AZ_VampClientLife.BeginCountdownCancellation();
		int diagnosticAttempt = AZ_VampClientLife.s_Attempt;
		super.CancelLoginTimeCountdown();
		if (cancelCall > 0 && diagnosticAttempt == AZ_VampClientLife.s_Attempt)
			AZ_VampClientLife.Mark("countdown_cancel_exit", "scope=script_handler call=" + cancelCall, true);
	}

	override void OnPreloadEvent(vector pos)
	{
		int preloadCall = AZ_VampClientLife.BeginPreload();
		int diagnosticAttempt = AZ_VampClientLife.s_Attempt;
		super.OnPreloadEvent(pos);
		if (preloadCall > 0 && diagnosticAttempt == AZ_VampClientLife.s_Attempt)
			AZ_VampClientLife.Mark("preload_exit", "scope=script_handler call=" + preloadCall, true);
	}

	override void StoreLoginDataPrepare()
	{
		int storeCall = AZ_VampClientLife.BeginStoreLogin();
		int diagnosticAttempt = AZ_VampClientLife.s_Attempt;
		super.StoreLoginDataPrepare();
		if (storeCall > 0 && diagnosticAttempt == AZ_VampClientLife.s_Attempt)
			AZ_VampClientLife.Mark("store_login_exit", "scope=script_handler call=" + storeCall);
	}


	override void OnLoginTimeEvent(int loginTime)
	{
		bool traceAdmission = AZ_VampClientLife.BeginQueueAdmission(loginTime);
		int diagnosticAttempt = AZ_VampClientLife.s_Attempt;
		super.OnLoginTimeEvent(loginTime);
		if (traceAdmission && diagnosticAttempt == AZ_VampClientLife.s_Attempt)
			AZ_VampClientLife.Mark("queue_admission_exit", "", true);
	}

	override void OnEvent(EventType eventTypeId, Param params)
	{
		AZ_VampClientLife.ObserveEvent(eventTypeId, params);
		int diagnosticAttempt = AZ_VampClientLife.s_Attempt;
		super.OnEvent(eventTypeId, params);
		AZ_VampClientLife.CompleteEvent(eventTypeId, diagnosticAttempt);
	}

	override void OnActivateMessage()
	{
		if (m_AZLifeAppActive != 1)
		{
			m_AZLifeAppActive = 1;
			AZ_VampClientLife.Mark("app_activate_callback");
		}
		super.OnActivateMessage();
	}

	override void OnDeactivateMessage()
	{
		if (m_AZLifeAppActive != 0)
		{
			m_AZLifeAppActive = 0;
			AZ_VampClientLife.Mark("app_deactivate_callback");
		}
		super.OnDeactivateMessage();
	}
}


modded class LoginScreenBase
{
	protected int m_AZLifeCancelAttempt = -1;

	override protected void Leave()
	{
		if (m_AZLifeCancelAttempt != AZ_VampClientLife.s_Attempt)
		{
			m_AZLifeCancelAttempt = AZ_VampClientLife.s_Attempt;
			AZ_VampClientLife.Mark("login_cancel_intent", "", true);

			AZ_VampClientLife.s_AttemptEnded = true;
		}
		super.Leave();
	}
}

modded class LoginQueueBase
{
	protected int m_AZLifeQueueAttempt = -1;
	protected int m_AZLifeQueueStart = -1;
	protected int m_AZLifeQueueLogAt = -1;
	protected int m_AZLifeQueuePosition = -999;

	void AZLifeQueueAttempt()
	{
		if (m_AZLifeQueueAttempt == AZ_VampClientLife.s_Attempt)
			return;
		m_AZLifeQueueAttempt = AZ_VampClientLife.s_Attempt;
		m_AZLifeQueueStart = -1;
		m_AZLifeQueueLogAt = -1;
		m_AZLifeQueuePosition = -999;
	}

	override void SetPosition(int position)
	{
		AZ_VampClientLife.ObserveQueuePosition(position, IsStatic());
		AZ_VampClientLife.ObserveQueueLiveness(position);
		AZLifeQueueAttempt();
		int now = GetGame().GetTime();
		if (m_AZLifeQueueStart == -1 || now < m_AZLifeQueueStart)
			m_AZLifeQueueStart = now;
		if (position != m_AZLifeQueuePosition && AZ_VampClientLife.ThrottleReady(now, m_AZLifeQueueLogAt, 5000))
		{
			m_AZLifeQueuePosition = position;
			m_AZLifeQueueLogAt = now;
			AZ_VampClientLife.Mark("queue_position", "position=" + position + " waitMs=" + (now - m_AZLifeQueueStart));
		}
		super.SetPosition(position);
	}
	override Widget Init()
	{
		AZ_VampClientLife.Mark("queue_init_enter");
		Widget root = super.Init();
		AZ_VampClientLife.Mark("queue_init_exit", "rootPresent=" + (root != null));
		return root;
	}

	override void Show()
	{
		AZLifeQueueAttempt();
		if (m_AZLifeQueueStart == -1)
			m_AZLifeQueueStart = GetGame().GetTime();
		AZ_VampClientLife.Mark("queue_show_enter");
		super.Show();
		AZ_VampClientLife.Mark("queue_show_exit");
	}

	override void Hide()
	{
		int now = GetGame().GetTime();
		int wait = -1;
		if (m_AZLifeQueueStart != -1 && now >= m_AZLifeQueueStart)
			wait = now - m_AZLifeQueueStart;
		AZ_VampClientLife.Mark("queue_hide_enter", "waitMs=" + wait, true);
		super.Hide();
		AZ_VampClientLife.Mark("queue_hide_exit");
		m_AZLifeQueueStart = -1;
		m_AZLifeQueueLogAt = -1;
		m_AZLifeQueuePosition = -999;
	}
}

modded class LoginTimeBase
{
	protected int m_AZLifeLastCountdown = -999;

	override Widget Init()
	{
		AZ_VampClientLife.Mark("countdown_init_enter");
		Widget root = super.Init();
		AZ_VampClientLife.Mark("countdown_init_exit", "rootPresent=" + (root != null));
		return root;
	}

	override void Show()
	{
		m_AZLifeLastCountdown = -999;
		AZ_VampClientLife.Mark("countdown_show_enter", "ui=" + AZ_VampClientLife.UiKind(IsStatic()));
		super.Show();
		AZ_VampClientLife.Mark("countdown_show_exit");
	}

	override void Hide()
	{
		AZ_VampClientLife.Mark("countdown_hide_enter");
		super.Hide();
		AZ_VampClientLife.Mark("countdown_hide_exit");
	}

	override void SetTime(int time)
	{
		bool nearJoin = time >= 0 && time <= 5 && time != m_AZLifeLastCountdown;
		if (nearJoin)
		{
			m_AZLifeLastCountdown = time;
			AZ_VampClientLife.Mark("countdown_step_enter", "remainingSec=" + time);
		}
		super.SetTime(time);
		if (nearJoin)
			AZ_VampClientLife.Mark("countdown_step_exit", "remainingSec=" + time);
	}
}

modded class LoadingScreen
{
	override void Show()
	{
		AZ_VampClientLife.Mark("loading_show_enter");
		super.Show();
		AZ_VampClientLife.Mark("loading_show_exit");
	}

	override void Hide(bool force)
	{
		AZ_VampClientLife.Mark("loading_hide_enter", "force=" + force);
		super.Hide(force);
		AZ_VampClientLife.Mark("loading_hide_exit");
	}

	override void EndLoading()
	{
		AZ_VampClientLife.Mark("loading_end_enter");
		super.EndLoading();
		AZ_VampClientLife.Mark("loading_end_exit");
	}
}

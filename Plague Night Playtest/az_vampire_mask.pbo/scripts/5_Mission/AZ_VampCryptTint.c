



class AZ_PPECryptTint extends PPERequester_GameplayBase
{
	static const float FADE_TIME = 2.5;
	static const float VIGNETTE = 0.0;
	static const int L_TINT = 24;
	static const int L_VIGNETTE = 398;
	static const int L_RAYS = 300;
	protected float m_T;
	protected bool m_In;

	void FadeIn()
	{
		m_In = true;
		m_T = 0;
		Start();
	}

	void FadeOut()
	{
		m_In = false;
		m_T = 0;
		SetRequesterUpdating(true);
	}

	override protected void OnUpdate(float delta)
	{
		super.OnUpdate(delta);
		if (m_T > FADE_TIME)
		{
			if (!m_In)
				Stop();
			return;
		}
		if (m_T == 0)
			Print("[AZ_Vamp] crypt tint " + m_In);
		m_T += delta;
		float k = Math.Clamp(m_T / FADE_TIME, 0, 1);
		if (!m_In)
			k = 1 - k;
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, {1.0 - 0.03 * k, 1.0 - 0.08 * k, 1.0 - 0.14 * k, 0.0}, L_TINT, PPOperators.MULTIPLICATIVE);
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTE, false, VIGNETTE * k, L_VIGNETTE, PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTECOLOR, {0.0, 0.0, 0.0, 0.0}, L_VIGNETTE, PPOperators.SET);


		SetTargetValueFloat(PostProcessEffectType.GodRays, PPEGodRays.PARAM_INTENSITY, false, 0, L_RAYS, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.GodRays, PPEGodRays.PARAM_OVERBURNINTENSITY, false, 0, L_RAYS, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.GodRays, PPEGodRays.PARAM_SUNVISIBLE, false, 0, L_RAYS, PPOperators.SET);
	}



	void Revalidate()
	{
		m_Valid = PPERequesterBank.VerifyRequester(this);
		Print("[AZ_Vamp] crypt tint requester valid " + m_Valid);
	}

	static AZ_PPECryptTint Get()
	{
		AZ_PPECryptTint r = AZ_PPECryptTint.Cast(PPERequesterBank.GetRequester(AZ_PPECryptTint));
		if (!r)
		{
			PPERequesterBank.RegisterRequester(AZ_PPECryptTint);
			r = AZ_PPECryptTint.Cast(PPERequesterBank.GetRequester(AZ_PPECryptTint));
			if (r)
				r.Revalidate();
		}
		return r;
	}
}

modded class MissionGameplay
{
	protected float m_AZCryptCheck;
	protected bool m_AZCryptTint;




	protected int m_AZTrigLogs;
	protected float m_AZTrigT;

	void AZTriggerCheck()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || AZ_VampCryptLook.s_In || m_AZTrigLogs >= 8 || !AZ_VampCrypt.Inside(p.GetPosition()))
			return;
		m_AZTrigT -= 0.5;
		if (m_AZTrigT > 0)
			return;
		m_AZTrigT = 5;
		m_AZTrigLogs++;
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D("7624 33 8840", 60, near, null);
		int trig = 0;
		int carriers = 0;
		foreach (Object o : near)
		{
			if (UndergroundTrigger.Cast(o))
				trig++;
			if (o.IsKindOf("UndergroundTriggerCarrier"))
				carriers++;
		}
		Print("[AZ_Vamp] crypt trigger check: at " + p.GetPosition() + " presence " + p.m_UndergroundPresence + " triggers " + trig + " carriers " + carriers + " loader data " + (UndergroundAreaLoader.m_JsonData != null) + " vampire " + p.m_AZVampire);
	}




	protected UndergroundTrigger m_AZCryptTrig;
	protected float m_AZTrigWait;

	void AZTriggerFallback()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (m_AZCryptTrig || !p || AZ_VampCryptLook.s_In || !AZ_VampCrypt.Inside(p.GetPosition()))
		{
			m_AZTrigWait = 0;
			return;
		}
		m_AZTrigWait += 0.5;
		if (m_AZTrigWait < 3 || !UndergroundAreaLoader.m_JsonData || !UndergroundAreaLoader.m_JsonData.Triggers)
			return;
		foreach (JsonUndergroundAreaTriggerData d : UndergroundAreaLoader.m_JsonData.Triggers)
		{
			vector dc = d.GetPosition();
			if (d.Comment != "az-vamp-crypt" && vector.Distance(Vector(dc[0], 0, dc[2]), Vector(7624, 0, 8840)) > 3)
				continue;
			m_AZCryptTrig = UndergroundTrigger.Cast(GetGame().CreateObjectEx("UndergroundTrigger", d.GetPosition(), ECE_LOCAL));
			if (m_AZCryptTrig)
			{
				m_AZCryptTrig.SetOrientation(d.GetOrientation());
				m_AZCryptTrig.Init(d);
			}
			Print("[AZ_Vamp] crypt trigger fallback: built locally " + (m_AZCryptTrig != null) + " at " + d.GetPosition() + " presence was " + p.m_UndergroundPresence);
			return;
		}
		m_AZTrigWait = -60;
		Print("[AZ_Vamp] crypt trigger fallback: no az-vamp-crypt entry in the synced cfgundergroundtriggers");
	}



	protected string m_AZAuditLabel = "";

	void AZLightAudit()
	{
		if (!AZ_VAMP_HALLOWEEN_TEST || AZ_VampShotRpc.s_Label == m_AZAuditLabel || AZ_VampShotRpc.s_Cam == vector.Zero)
			return;
		m_AZAuditLabel = AZ_VampShotRpc.s_Label;
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(AZ_VampShotRpc.s_Cam, 30, near, null);
		map<string, int> per = new map<string, int>;
		int n = 0;
		foreach (Object o : near)
		{
			ScriptedLightBase sl = ScriptedLightBase.Cast(o);
			if (!sl)
				continue;
			n++;
			per.Set(sl.GetType(), per.Get(sl.GetType()) + 1);

			float dl = vector.Distance(sl.GetPosition(), AZ_VampShotRpc.s_Cam);
			if (dl < 12)
				Print("[AZ_Vamp] light audit " + m_AZAuditLabel + " light " + sl.GetType() + " at " + dl + " m enabled " + sl.IsEnabled() + " radius " + sl.GetRadius() + " daylight " + sl.IsVisibleDuringDaylight() + " diffuse " + sl.GetDiffuseColorV() + " a " + sl.GetDiffuseAlpha());
		}
		vector hl = AZ_VampCrypt.HallLocal(AZ_VampShotRpc.s_Cam);
		PlayerBase ap = PlayerBase.Cast(GetGame().GetPlayer());
		string pr = "none";
		if (ap)
			pr = "" + AZ_VampCrypt.HallLocal(ap.GetPosition()) + " presence " + ap.m_UndergroundPresence;
		Print("[AZ_Vamp] light audit " + m_AZAuditLabel + " gates: cam hall " + hl + " inside " + AZ_VampCrypt.Inside(AZ_VampShotRpc.s_Cam) + " ante " + AZ_VampCrypt.AnteOn() + " gallery " + AZ_VampCryptGallery.InGallery(AZ_VampShotRpc.s_Cam) + " player hall " + pr + " crypt look " + AZ_VampCryptLook.s_In);
		string line = "";
		foreach (string k, int c : per)
			line = line + " " + k + "=" + c;
		Print("[AZ_Vamp] light audit " + m_AZAuditLabel + ": " + n + " script lights within 30 m:" + line);
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		m_AZCryptCheck -= timeslice;
		if (m_AZCryptCheck > 0)
			return;
		m_AZCryptCheck = 0.5;
		AZ_VampCryptGallery.ClientTick(PlayerBase.Cast(GetGame().GetPlayer()));
		AZTriggerCheck();
		AZTriggerFallback();
		AZLightAudit();
		bool on = AZ_VampCryptLook.s_In && !AZ_VampBand.Is("B");
		if (on == m_AZCryptTint)
			return;
		m_AZCryptTint = on;
		AZ_PPECryptTint r = AZ_PPECryptTint.Get();
		Print("[AZ_Vamp] crypt tint request " + on + " requester " + r);
		if (!r)
			return;
		if (on)
			r.FadeIn();
		else
			r.FadeOut();
	}
}

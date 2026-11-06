








class AZ_VampCryptLights
{
	static const bool REVERT = false;
	static const int TICK_MS = 500;

	static const float TRAP_OFF_F = 5.0;
	static const float TRAP_ON_F = 3.0;
	static const float NAVE_OFF_F = -4.0;
	static const float NAVE_ON_F = -2.0;

	static ref array<ScriptedLightBase> s_Trap;
	static ref array<ScriptedLightBase> s_Nave;
	static bool s_TrapOn = true;
	static bool s_NaveOn = true;
	static bool s_Ticking;
	static int s_LogAt;

	static void Add(ScriptedLightBase l, bool trap)
	{
		if (!l || GetGame().IsDedicatedServer())
			return;
		if (!s_Trap)
		{
			s_Trap = new array<ScriptedLightBase>;
			s_Nave = new array<ScriptedLightBase>;
		}
		if (trap)
			s_Trap.Insert(l);
		else
			s_Nave.Insert(l);
		if (!s_Ticking && !REVERT)
		{
			s_Ticking = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, true);
		}
	}



	static void AddEntry(ScriptedLightBase l)
	{
		if (!l)
			return;
		if (!REVERT && !AZ_VampCrypt.EntryOn())
		{
			l.SetBrightnessTo(0);
			l.SetEnabled(false);
			return;
		}
		Add(l, true);
	}



	static bool InCryptBox(vector pos)
	{
		if (!AZ_VampCrypt.Site())
			return false;
		vector l = AZ_VampCrypt.HallLocal(pos);
		return l[0] > -AZ_VampCrypt.HALF_LEN - 3 && l[0] < AZ_VampCrypt.APSE_F + 13 && Math.AbsFloat(l[1]) < 14.5 && l[2] > -AZ_VampCrypt.LEDGE_H - 2.5 && l[2] < 17.5 - AZ_VampCrypt.LEDGE_H;
	}

	static const bool SLAB_LIGHT_MOVE = false;
	static ScriptedLightBase s_Slab;
	static const bool SLAB_FREE = false;
	static const float SLAB_LIGHT_F = 56.0;
	static const float SLAB_LIGHT_R = -1.6;

	static const float FEED_SQ_MIN_F = 40.0;
	static const float FEED_SQ_MAX_F = 57.0;


	static void AddBrazier(AZ_VampBrazierLight l)
	{
		if (!l)
			return;
		vector at = l.GetPosition();
		if (l.m_Parent)
			at = l.m_Parent.GetPosition();
		float f = AZ_VampCrypt.HallLocal(at)[0];





		if (f > FEED_SQ_MIN_F && f < FEED_SQ_MAX_F)
		{
			if (AZ_VampCrypt.HallLocal(at)[1] > 0 && f < 50)
			{
				l.AZ_SetRest(7.0, 3.0);
				l.m_AZF = f;
				l.m_AZBase = l.AZ_Rest();
				Add(l, false);
				Print("[AZ_Vamp] crypt feed square brazier light at f " + f + " r " + l.m_Radius + " b " + l.AZ_Rest());
			}
			else
			{
				l.m_AZOff = true;
				l.SetBrightnessTo(0);
				l.SetEnabled(false);
			}
			return;
		}


		if (!REVERT && f > 0 && f < 6)
			l.AZ_SetRest(11.0, 4.2);


		if (!REVERT && f < -1)
			l.AZ_SetRest(7.0, 4.2);


		if (f > 40)
		{
			if (!s_Dais)
				s_Dais = new array<ScriptedLightBase>;
			s_Dais.Insert(l);
		}
		if (!REVERT && f > 40)
			l.AZ_SetRest(8.5, 2.4);



		vector hl = AZ_VampCrypt.HallLocal(at);


		if (SLAB_LIGHT_MOVE && !REVERT && f > 40 && hl[1] < 0)
		{
			l.DetachFromParent();
			l.SetPosition(AZ_VampCrypt.Nave(SLAB_LIGHT_F, SLAB_LIGHT_R, AZ_VampCrypt.DAIS_Y + 0.7));
			Print("[AZ_Vamp] crypt slab light (left dais brazier's point) at " + l.GetPosition());
		}


		if (SLAB_FREE && !SLAB_LIGHT_MOVE && !REVERT && f > 40 && hl[1] < 0 && !s_Slab)
		{
			s_Slab = AZ_VampLightGate.Light(AZ_VampSlabLight, AZ_VampCrypt.Nave(SLAB_LIGHT_F, SLAB_LIGHT_R, AZ_VampCrypt.DAIS_Y + 0.7));
			if (s_Slab)
			{
				l.m_AZOff = true;
				l.SetBrightnessTo(0);
				l.SetEnabled(false);
				Add(s_Slab, false);
				Print("[AZ_Vamp] crypt slab light (free) at " + s_Slab.GetPosition() + ", the left dais brazier's own light off");
				return;
			}
		}
		l.m_AZF = f;
		l.m_AZBase = l.AZ_Rest();
		Add(l, f < -1.0);
		Print("[AZ_Vamp] crypt brazier light at f " + f + " r " + l.m_Radius + " b " + l.AZ_Rest());
	}



	static const float FADE_S = 0.5;
	static ref map<ScriptedLightBase, float> s_Rest;

	static void Prune(array<ScriptedLightBase> lights)
	{
		for (int i = lights.Count() - 1; i >= 0; i--)
		{
			if (!lights[i])
				lights.Remove(i);
		}
	}

	static void Switch(array<ScriptedLightBase> lights, bool on)
	{
		if (!s_Rest)
			s_Rest = new map<ScriptedLightBase, float>;
		Prune(lights);
		foreach (ScriptedLightBase l : lights)
		{
			AZ_VampCryptPointLight pl = AZ_VampCryptPointLight.Cast(l);
			if (on)
			{
				if (pl)
					pl.m_AZOff = false;
				l.SetEnabled(true);
				float rest;
				if (s_Rest.Find(l, rest))
				{
					l.SetBrightnessTo(0);
					l.FadeBrightnessTo(rest, FADE_S);
				}
				continue;
			}
			if (l.m_Brightness > 0.01)
				s_Rest.Set(l, l.m_Brightness);
			l.FadeBrightnessTo(0.001, FADE_S);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(OffAfterFade, FADE_S * 1000 + 100, false, l);
		}
	}

	static void OffAfterFade(ScriptedLightBase l)
	{
		if (!l)
			return;
		bool on = (s_Trap.Find(l) >= 0 && s_TrapOn) || (s_Nave.Find(l) >= 0 && s_NaveOn);
		if (on)
			return;
		AZ_VampCryptPointLight pl = AZ_VampCryptPointLight.Cast(l);
		if (pl)
			pl.m_AZOff = true;
		l.SetEnabled(false);
	}



	static string s_AuditLabel = "";
	static int s_AuditMax;
	static void ViewAudit(vector cam)
	{
		vector dir = GetGame().GetCurrentCameraDirection();
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(cam, 70, near, null);
		int n = 0;
		int ours = 0;
		string what = "";
		foreach (Object o : near)
		{
			ScriptedLightBase sl = ScriptedLightBase.Cast(o);
			if (!sl || !sl.IsEnabled() || sl.m_Brightness <= 0)
				continue;
			vector at = sl.GetPosition();
			if (sl.m_Parent)
				at = sl.m_Parent.GetPosition();
			vector to = at - cam;
			float d = to.Length();
			if (d - sl.m_Radius > 45)
				continue;
			if (d > sl.m_Radius && vector.Dot(to.Normalized(), dir) < 0.42)
				continue;
			n++;
			if (s_Trap.Find(sl) >= 0 || s_Nave.Find(sl) >= 0)
				ours++;
			what = what + " " + sl.GetType();
		}
		if (AZ_VampShotRpc.s_Label != s_AuditLabel)
		{
			s_AuditLabel = AZ_VampShotRpc.s_Label;
			s_AuditMax = 0;
		}
		if (n > s_AuditMax)
		{
			s_AuditMax = n;
			Print("[AZ_Vamp] lighting view " + s_AuditLabel + ": " + n + " lights in view (" + ours + " switched), max " + s_AuditMax + ":" + what);
		}
	}




	static int s_HoldGen;
	static string s_HoldLabel;

	static void HoldStart(string label)
	{
		s_HoldGen++;
		s_HoldLabel = label;
		for (int i = 0; i < 160; i++)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(HoldLog, i * 100, false, s_HoldGen, i);
	}

	static void HoldLog(int gen, int i)
	{
		if (gen != s_HoldGen)
			return;
		vector cam = GetGame().GetCurrentCameraPosition();
		vector dir = GetGame().GetCurrentCameraDirection();
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(cam, 70, near, null);
		int n = 0;
		float sum = 0;
		string what = "";
		foreach (Object o : near)
		{
			ScriptedLightBase sl = ScriptedLightBase.Cast(o);
			if (!sl || !sl.IsEnabled() || sl.m_Brightness <= 0.01)
				continue;
			vector at = sl.GetPosition();
			if (sl.m_Parent)
				at = sl.m_Parent.GetPosition();
			vector to = at - cam;
			float d = to.Length();
			if (d - sl.m_Radius > 45)
				continue;
			if (d > sl.m_Radius && vector.Dot(to.Normalized(), dir) < 0.42)
				continue;
			n++;
			sum += sl.GetBrightness();
			what = what + " " + sl.GetType() + "@" + Math.Round(d);
		}
		string dais = "";
		if (s_Dais)
		{
			foreach (ScriptedLightBase dl : s_Dais)
			{
				if (dl)
					dais = dais + " " + dl.GetBrightness() + "/" + dl.IsEnabled();
			}
		}
		Print("[AZ_Vamp] hold " + s_HoldLabel + " t " + i + " n " + n + " sum " + sum + " dais" + dais + " :" + what);
	}




	static float RitualScale(vector pos)
	{
		if (REVERT || !AZ_VampCrypt.Inside(pos))
			return 1.0;
		if (AZ_VampLightTune.s_Data && AZ_VampLightTune.s_Data.ritual_candles && AZ_VampLightTune.s_Data.ritual_candles.brightness >= 0)
			return AZ_VampLightTune.s_Data.ritual_candles.brightness / 2.2;
		return 1.6 / 2.2;
	}

	static void RitualCandle(ScriptedLightBase l, vector pos)
	{
		if (!l || REVERT || !AZ_VampCrypt.Inside(pos))
			return;
		l.SetRadiusTo(3.5);
		if (AZ_VampLightTune.s_Data && AZ_VampLightTune.s_Data.ritual_candles && AZ_VampLightTune.s_Data.ritual_candles.radius >= 0)
			l.SetRadiusTo(AZ_VampLightTune.s_Data.ritual_candles.radius);
	}




	static ref array<ScriptedLightBase> s_Dais;
	static string s_BandSeen = "";

	static void BandTick()
	{
		if (AZ_VampBand.s_V == s_BandSeen)
			return;
		s_BandSeen = AZ_VampBand.s_V;
		bool noMoonAmb = AZ_VampBand.Is("L") || AZ_VampBand.Is("N");
		bool noDais = AZ_VampBand.Is("M") || AZ_VampBand.Is("N");
		foreach (ScriptedLightBase nl : s_Nave)
		{
			AZ_VampMoonLight ml = AZ_VampMoonLight.Cast(nl);
			if (!ml)
				continue;
			if (noMoonAmb)
				ml.SetAmbientColor(0, 0, 0);
			else
				ml.SetAmbientColor(0.06, 0.07, 0.09);
		}
		if (s_Dais)
		{
			foreach (ScriptedLightBase dl : s_Dais)
			{
				if (!dl)
					continue;
				AZ_VampCryptPointLight dp = AZ_VampCryptPointLight.Cast(dl);
				if (dp)
					dp.m_AZOff = noDais;
				dl.SetEnabled(!noDais);
			}
		}
		Print("[AZ_Vamp] lighting band " + s_BandSeen + ": moon ambient off " + noMoonAmb + ", dais braziers off " + noDais);
	}








	static const bool BOWLS_ON = false;
	static const bool LIGHT_CUT = true;
	static const bool ALL_CROWNS_LIT = false;
	static const float BOWL_F1 = 16.8;
	static const float BOWL_F2 = 31.2;
	static const float BASIN_F = 52.4;
	static const float BASIN_R = 2.85;
	static const float LAST_F = 43.2;
	static ref array<ScriptedLightBase> s_Bowls;
	static int s_BowlTries;
	static bool s_BayLit;
	static bool s_FeedLit;

	static AZ_Crypt_Fire_Bowl BowlNear(vector w)
	{
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(w, 1.2, near, null);
		foreach (Object o : near)
		{
			AZ_Crypt_Fire_Bowl bowl = AZ_Crypt_Fire_Bowl.Cast(o);
			if (bowl)
				return bowl;
		}
		return null;
	}

	static ScriptedLightBase LightBowl(AZ_Crypt_Fire_Bowl bowl, float radius)
	{
		ScriptedLightBase l;
		if (bowl.MemoryPointExists("fire"))
			l = AZ_VampLightGate.AtPoint(AZ_VampBowlLight, bowl, "fire");
		else
		{
			l = AZ_VampLightGate.Light(AZ_VampBowlLight, bowl.GetPosition() + "0 0.5 0");
			if (l)
				l.AttachOnObject(bowl, "0 0.5 0");
		}
		if (!l)
			return null;
		l.SetRadiusTo(radius);
		AZ_VampCryptPointLight bp = AZ_VampCryptPointLight.Cast(l);
		if (bp)
		{
			bp.m_AZF = AZ_VampCrypt.HallLocal(bowl.GetPosition())[0];
			bp.m_AZBase = bp.m_Brightness;
		}
		s_Bowls.Insert(l);
		Add(l, false);
		Print("[AZ_Vamp] crypt bowl light at hall " + AZ_VampCrypt.HallLocal(bowl.GetPosition()) + " r " + radius);
		return l;
	}







	static const bool RIVER_UP = false;
	static ref array<ScriptedLightBase> s_River;

	static void RiverUp()
	{
		if (!RIVER_UP || REVERT || GetGame().IsDedicatedServer() || s_River)
			return;
		s_River = new array<ScriptedLightBase>;
		float y = -AZ_VampCrypt.NAVE_DEPTH + AZ_VampCrypt.BLOOD_MIN + 0.15;
		array<vector> at = new array<vector>;
		at.Insert(Vector(15.0, -1.2, y));
		at.Insert(Vector(33.0, 1.2, y));
		at.Insert(Vector(49.5, 0, y));
		foreach (vector a : at)
		{
			ScriptedLightBase l = AZ_VampLightGate.Light(AZ_VampRiverUpLight, AZ_VampCrypt.Nave(a[0], a[1], a[2]));
			if (!l)
				continue;
			AZ_VampCryptPointLight p = AZ_VampCryptPointLight.Cast(l);
			if (p)
				p.m_AZF = a[0];
			s_River.Insert(l);
			Add(l, false);
			Print("[AZ_Vamp] crypt river glow at f " + a[0] + " " + l.GetPosition());
		}
	}

	static void RiverDown()
	{
		if (!s_River)
			return;
		foreach (ScriptedLightBase l : s_River)
		{
			if (l)
				l.Destroy();
		}
		s_River = null;
	}






	static ScriptedLightBase BowlLight(AZ_Crypt_Fire_Bowl bowl)
	{
		if (!bowl || REVERT || GetGame().IsDedicatedServer())
			return null;
		vector h = AZ_VampCrypt.HallLocal(bowl.GetPosition());
		float radius = 0;









		if (LIGHT_CUT && (Math.AbsFloat(h[0] - BOWL_F1) < 1.0 || Math.AbsFloat(h[0] - BOWL_F2) < 1.0 || Math.AbsFloat(h[0] - BASIN_F) < 1.0))
			return null;
		if (Math.AbsFloat(h[0] - BOWL_F1) < 1.0 && Math.AbsFloat(h[1] + AZ_VampCrypt.PILLAR_R) < 1.0)
			radius = 14.0;
		else if (Math.AbsFloat(h[0] - BOWL_F2) < 1.0 && Math.AbsFloat(h[1] - AZ_VampCrypt.PILLAR_R) < 1.0)
			radius = 14.0;
		else if (Math.AbsFloat(h[0] - BASIN_F) < 1.0 && Math.AbsFloat(Math.AbsFloat(h[1]) - BASIN_R) < 1.0)
			radius = 6.0;
		if (radius <= 0)
			return null;
		ScriptedLightBase l = AZ_VampLightGate.AtPoint(AZ_VampBowlLight, bowl, "fire");
		if (!l)
			return null;
		l.SetRadiusTo(radius);
		if (h[0] <= 45)
			l.SetBrightnessTo(4.6);
		AZ_VampCryptPointLight bp = AZ_VampCryptPointLight.Cast(l);
		if (bp)
		{
			bp.m_AZF = h[0];
			bp.m_AZBase = bp.m_Brightness;
		}
		Add(l, false);
		Print("[AZ_Vamp] crypt bowl light (own init) at hall " + h + " r " + radius);
		return l;
	}

	static void BowlLights()
	{
		if (!BOWLS_ON || REVERT || (s_BayLit && s_FeedLit) || s_BowlTries > 80)
			return;
		s_BowlTries++;
		if (!s_Bowls)
			s_Bowls = new array<ScriptedLightBase>;
		AZ_Crypt_Fire_Bowl b;
		if (!s_BayLit)
		{
			b = BowlNear(AZ_VampCrypt.Nave(BOWL_F1, -AZ_VampCrypt.PILLAR_R));
			if (b)
				s_BayLit = LightBowl(b, 6.0) != null;
		}
		if (!s_FeedLit)
		{
			b = BowlNear(AZ_VampCrypt.Nave(BASIN_F, BASIN_R));
			if (b)
				s_FeedLit = LightBowl(b, 6.0) != null;
			else if (s_BowlTries > 20)
			{
				b = BowlNear(AZ_VampCrypt.Nave(LAST_F, AZ_VampCrypt.PILLAR_R));
				if (b)
					s_FeedLit = LightBowl(b, 11.0) != null;
			}
		}
	}

	static void Tick()
	{
		if (!s_Trap || !AZ_VampCrypt.Site())
			return;
		BandTick();
		if (AZ_VampCrypt.Inside(GetGame().GetCurrentCameraPosition()))
			BowlLights();
		vector cam = GetGame().GetCurrentCameraPosition();
		bool inside = AZ_VampCrypt.Inside(cam);
		float f = AZ_VampCrypt.HallLocal(cam)[0];
		bool trap = s_TrapOn;
		bool nave = s_NaveOn;
		if (!inside)
		{
			trap = true;
			nave = true;
		}
		else
		{
			if (s_TrapOn && f > TRAP_OFF_F)
				trap = false;
			else if (!s_TrapOn && f < TRAP_ON_F)
				trap = true;
			if (s_NaveOn && f < NAVE_OFF_F)
				nave = false;
			else if (!s_NaveOn && f > NAVE_ON_F)
				nave = true;
		}
		if (trap != s_TrapOn)
		{
			s_TrapOn = trap;
			Switch(s_Trap, trap);
		}
		if (nave != s_NaveOn)
		{
			s_NaveOn = nave;
			Switch(s_Nave, nave);
		}
		if (inside && AZ_VampShotRpc.s_Label != "")
			ViewAudit(cam);
		if (AZ_VampShotRpc.s_Label != s_HoldLabel && AZ_VampShotRpc.s_Label.IndexOf("lt_hold") == 0)
			HoldStart(AZ_VampShotRpc.s_Label);
		int now = GetGame().GetTime();
		if (inside && now >= s_LogAt)
		{
			s_LogAt = now + 60000;
			Prune(s_Trap);
			Prune(s_Nave);
			Print("[AZ_Vamp] crypt lights: cam f " + f + " trap " + s_TrapOn + " (" + s_Trap.Count() + ") nave " + s_NaveOn + " (" + s_Nave.Count() + ")");
		}
	}
}



class AZ_VampCryptPointLight extends PointLightBase
{
	bool m_AZOff;
	float m_AZF = -999;
	float m_AZBase;

	override void CheckIfParentIsInCargo()
	{
		if (!m_AZOff)
			super.CheckIfParentIsInCargo();
	}
}


class AZ_VampBrazierLight extends AZ_VampCryptPointLight
{
	void AZ_VampBrazierLight()
	{
		SetVisibleDuringDaylight(true);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.32, 0.08);
		SetAmbientColor(0.25, 0.08, 0.02);
		SetFlickerAmplitude(0.2);
		SetFlickerSpeed(0.5);
		if (AZ_VampCryptLights.REVERT)
			AZ_SetRest(11.0, 3.6);
		else
			AZ_SetRest(10.0, 3.4);
		FadeIn(0.5);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_Register, 100, false);
	}

	protected float m_AZRest;

	void AZ_SetRest(float radius, float bright)
	{
		m_AZRest = bright;
		SetRadiusTo(radius);
		SetBrightnessTo(bright);
	}


	float AZ_Rest()
	{
		return m_AZRest;
	}

	void AZ_Register()
	{
		AZ_VampCryptLights.AddBrazier(this);
	}
}





class AZ_VampChandelierLight extends AZ_VampCryptPointLight
{
	static const bool CROWN_SHADOWS = false;

	void AZ_VampChandelierLight()
	{
		SetVisibleDuringDaylight(true);
		SetCastShadow(CROWN_SHADOWS && !AZ_VampCryptLights.REVERT);
		SetFlareVisible(false);
		SetDiffuseColor(1.3, 0.42, 0.17);
		SetAmbientColor(0.13, 0.042, 0.017);
		SetFlickerAmplitude(0.12);
		SetFlickerSpeed(0.3);
		SetDancingShadowsMovementSpeed(0.1);
		SetDancingShadowsAmplitude(0.03);
		if (AZ_VampCryptLights.REVERT)
		{
			SetRadiusTo(16.0);
			SetBrightnessTo(4.5);
		}
		else
		{
			SetRadiusTo(24.0);
			SetBrightnessTo(2.4);
		}
		FadeIn(0.5);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_Place, 100, false);
	}



	void AZ_Place()
	{
		if (AZ_VampCryptLights.REVERT)
		{
			AZ_VampCryptLights.Add(this, false);
			return;
		}
		vector at = GetPosition();
		if (m_Parent)
			at = m_Parent.GetPosition();
		float f = AZ_VampCrypt.HallLocal(at)[0];

		if (!AZ_VampCryptLights.ALL_CROWNS_LIT && f < 12.0)
		{
			m_AZOff = true;
			SetBrightnessTo(0);
			SetEnabled(false);
			Print("[AZ_Vamp] crypt chandelier light at f " + f + " dark (first bay: the ledge braziers cover it)");
			return;
		}
		float b = 2.4;


		if (f > 40)
		{
			SetRadiusTo(13.0);
			b = 3.0;
		}
		m_AZF = f;
		m_AZBase = b;
		SetBrightnessTo(b);
		AZ_VampCryptLights.Add(this, false);
		Print("[AZ_Vamp] crypt chandelier light at f " + f + " b " + b + " hall " + AZ_VampCrypt.HallLocal(GetPosition()) + " shadows " + CROWN_SHADOWS);
	}
}

class AZ_VampCrawlGlow extends PointLightBase
{
	void AZ_VampCrawlGlow()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(11.0);
		SetBrightnessTo(2.0);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.22, 0.1);
		SetAmbientColor(1.0, 0.22, 0.1);
		SetFlickerAmplitude(0.1);
		SetFlickerSpeed(0.3);
		AZ_VampCryptLights.AddEntry(this);
	}
}

class AZ_VampBallGlow extends PointLightBase
{
	void AZ_VampBallGlow()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(7.0);
		SetBrightnessTo(2.6);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.16, 0.08);
		SetAmbientColor(1.0, 0.16, 0.08);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
		AZ_VampCryptLights.AddEntry(this);
	}
}

class AZ_VampVotiveGlow extends PointLightBase
{
	void AZ_VampVotiveGlow()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(12.0);
		SetBrightnessTo(3.0);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.55, 0.25);
		SetAmbientColor(1.0, 0.55, 0.25);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
	}
}




class AZ_VampApseGlow extends SpotLightBase
{
	void AZ_VampApseGlow()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(16.0);
		SetSpotLightAngle(40);
		SetFlareVisible(false);
		SetDiffuseColor(0.86, 0.9, 1.0);
		SetAmbientColor(0.3, 0.32, 0.36);
		if (AZ_VampCryptLights.REVERT)
		{
			SetBrightnessTo(6.0);
			SetCastShadow(true);
		}
		else
		{
			SetBrightnessTo(0);
			SetCastShadow(false);
			SetEnabled(false);
		}
	}
}


class AZ_VampMoonLight extends SpotLightBase
{
	static const float AZ_BRIGHT = 3.0;

	void AZ_VampMoonLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(20.0);
		SetSpotLightAngle(16);
		SetBrightnessTo(AZ_BRIGHT);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(0.72, 0.8, 1.0);
		SetAmbientColor(0.06, 0.07, 0.09);
		AZ_VampCryptLights.Add(this, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_Log, 500, false);
	}

	void AZ_Log()
	{
		Print("[AZ_Vamp] crypt moon light at " + GetPosition() + " hall " + AZ_VampCrypt.HallLocal(GetPosition()) + " dir " + GetDirection() + " b " + m_Brightness + " r " + m_Radius);
	}
}

class AZ_VampGalleryVotive extends PointLightBase
{
	void AZ_VampGalleryVotive()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(6.5);
		SetBrightnessTo(2.4);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.55, 0.25);
		SetAmbientColor(1.0, 0.55, 0.25);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
	}
}



class AZ_VampOssuaryLight extends PointLightBase
{
	void AZ_VampOssuaryLight()
	{
		SetVisibleDuringDaylight(true);
		SetFlareVisible(false);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
		if (AZ_VampCryptLights.REVERT)
		{
			SetRadiusTo(10.5);
			SetBrightnessTo(3.4);
			SetCastShadow(false);
			SetDiffuseColor(1.0, 0.58, 0.28);
			SetAmbientColor(0.3, 0.17, 0.08);
			return;
		}
		SetRadiusTo(5.0);
		SetBrightnessTo(2.4);
		SetCastShadow(true);
		SetDiffuseColor(1.0, 0.55, 0.22);
		SetAmbientColor(0.06, 0.035, 0.015);
	}
}


class AZ_VampTombLamp extends PointLightBase
{
	void AZ_VampTombLamp()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(7.5);
		SetBrightnessTo(2.4);
		SetCastShadow(true);
		SetFlareVisible(false);
		SetDiffuseColor(0.8, 0.88, 1.0);
		SetAmbientColor(0.2, 0.22, 0.25);
	}
}


class AZ_VampFrescoPool extends PointLightBase
{
	void AZ_VampFrescoPool()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(4.0);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.6, 0.3);
		SetAmbientColor(0.4, 0.24, 0.12);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
		if (AZ_VampCryptLights.REVERT)
			SetBrightnessTo(1.8);
		else
		{
			SetBrightnessTo(0);
			SetEnabled(false);
		}
	}
}

class AZ_VampBunkPool extends PointLightBase
{
	void AZ_VampBunkPool()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(4.5);
		SetBrightnessTo(1.7);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.55, 0.25);
		SetAmbientColor(1.0, 0.55, 0.25);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
	}
}

class AZ_VampCageChainLight extends PointLightBase
{
	void AZ_VampCageChainLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(4.0);
		SetBrightnessTo(1.3);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(0.7, 0.8, 1.0);
		SetAmbientColor(0.7, 0.8, 1.0);
		AZ_VampCryptLights.Add(this, true);
	}
}



class AZ_VampBowlLight extends AZ_VampCryptPointLight
{
	void AZ_VampBowlLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(6.0);
		SetBrightnessTo(2.6);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.36, 0.1);
		SetAmbientColor(0.35, 0.12, 0.04);
		SetFlickerAmplitude(0.2);
		SetFlickerSpeed(0.5);
		FadeIn(0.5);
	}
}







class AZ_VampLightTuneGroup
{
	float radius = -1;
	float brightness = -1;
	ref array<float> color;
	float flicker = -1;
	float angle = -1;
}

class AZ_VampLightTuneData
{
	string note = "Crypt light tuner: -1 = keep. /vamp lights reload applies, /vamp lights off restores the baked values. The profile (ambient, exposure, haze in crypt_lighting.cfg) is not live: send those wishes to the lighting owner.";
	ref AZ_VampLightTuneGroup ledge_braziers;
	ref AZ_VampLightTuneGroup dais_braziers;
	ref AZ_VampLightTuneGroup candlehall_brazier;
	ref AZ_VampLightTuneGroup bank_bowls;
	ref AZ_VampLightTuneGroup feed_bowl;
	ref AZ_VampLightTuneGroup river_uplight;
	ref AZ_VampLightTuneGroup ritual_candles;
	ref AZ_VampLightTuneGroup chandeliers;
	ref AZ_VampLightTuneGroup moon;
	ref AZ_VampLightTuneGroup face_candle;
	ref AZ_VampLightTuneGroup slab;
}

class AZ_VampLightTune
{
	static const int RPC = 4702311;
	static const string PATH = "$profile:AZ_VampLightTune.json";
	static ref AZ_VampLightTuneData s_Data;


	static string Send(PlayerBase admin, string arg)
	{
		if (arg != "dump" && arg != "reload" && arg != "off")
			return "lights: dump | reload | off (client file AppData/Local/DayZ/AZ_VampLightTune.json)";
		GetGame().RPCSingleParam(admin, RPC, new Param1<string>(arg), true, admin.GetIdentity());
		return "lights " + arg + " sent to your client";
	}

	static string Group(ScriptedLightBase l)
	{
		if (AZ_VampMoonLight.Cast(l))
			return "moon";
		if (AZ_VampChandelierLight.Cast(l))
			return "chandeliers";
		AZ_VampCryptPointLight p = AZ_VampCryptPointLight.Cast(l);
		if (!p)
			return "";
		if (AZ_VampRiverUpLight.Cast(l))
			return "river_uplight";
		if (AZ_VampFaceCandleLight.Cast(l))
			return "face_candle";
		if (AZ_VampSlabLight.Cast(l))
			return "slab";
		if (AZ_VampBowlLight.Cast(l))
		{
			if (p.m_AZF > 45)
				return "feed_bowl";
			return "bank_bowls";
		}
		if (AZ_VampBrazierLight.Cast(l))
		{
			if (p.m_AZF > 40)
				return "dais_braziers";
			if (p.m_AZF < -1)
				return "candlehall_brazier";
			return "ledge_braziers";
		}
		return "";
	}

	static AZ_VampLightTuneGroup Get(AZ_VampLightTuneData d, string g)
	{
		if (!d)
			return null;
		switch (g)
		{
			case "ledge_braziers": return d.ledge_braziers;
			case "dais_braziers": return d.dais_braziers;
			case "candlehall_brazier": return d.candlehall_brazier;
			case "bank_bowls": return d.bank_bowls;
			case "feed_bowl": return d.feed_bowl;
			case "river_uplight": return d.river_uplight;
			case "chandeliers": return d.chandeliers;
			case "moon": return d.moon;
			case "face_candle": return d.face_candle;
			case "slab": return d.slab;
		}
		return null;
	}

	static void Set(AZ_VampLightTuneData d, string g, AZ_VampLightTuneGroup v)
	{
		switch (g)
		{
			case "ledge_braziers": d.ledge_braziers = v; break;
			case "dais_braziers": d.dais_braziers = v; break;
			case "candlehall_brazier": d.candlehall_brazier = v; break;
			case "bank_bowls": d.bank_bowls = v; break;
			case "feed_bowl": d.feed_bowl = v; break;
			case "river_uplight": d.river_uplight = v; break;
			case "chandeliers": d.chandeliers = v; break;
			case "moon": d.moon = v; break;
			case "face_candle": d.face_candle = v; break;
			case "slab": d.slab = v; break;
		}
	}

	static array<ScriptedLightBase> All()
	{
		array<ScriptedLightBase> all = new array<ScriptedLightBase>;
		if (AZ_VampCryptLights.s_Trap)
			all.InsertAll(AZ_VampCryptLights.s_Trap);
		if (AZ_VampCryptLights.s_Nave)
			all.InsertAll(AZ_VampCryptLights.s_Nave);
		return all;
	}

	static float Base(ScriptedLightBase l)
	{
		AZ_VampCryptPointLight p = AZ_VampCryptPointLight.Cast(l);
		if (p && p.m_AZBase > 0)
			return p.m_AZBase;
		if (AZ_VampMoonLight.Cast(l))
			return AZ_VampMoonLight.AZ_BRIGHT;
		return l.m_Brightness;
	}

	static void ApplyBrightness(ScriptedLightBase l, float b)
	{
		AZ_VampBrazierLight br = AZ_VampBrazierLight.Cast(l);
		if (br)
			br.AZ_SetRest(br.m_Radius, b);
		else
			l.SetBrightnessTo(b);
		if (AZ_VampCryptLights.s_Rest && AZ_VampCryptLights.s_Rest.Contains(l))
			AZ_VampCryptLights.s_Rest.Set(l, b);
	}

	static void Apply(bool baked)
	{
		array<ScriptedLightBase> all = All();
		map<string, float> top = new map<string, float>;
		foreach (ScriptedLightBase a : all)
		{
			if (!a)
				continue;
			string ga = Group(a);
			if (ga != "" && Base(a) > top.Get(ga))
				top.Set(ga, Base(a));
		}
		int n = 0;
		foreach (ScriptedLightBase l : all)
		{
			if (!l)
				continue;
			string g = Group(l);
			if (g == "")
				continue;
			if (baked)
			{
				ApplyBrightness(l, Base(l));
				continue;
			}
			AZ_VampLightTuneGroup t = Get(s_Data, g);
			if (!t)
				continue;
			if (t.radius >= 0)
				l.SetRadiusTo(t.radius);
			if (t.brightness >= 0 && top.Get(g) > 0)
				ApplyBrightness(l, t.brightness * Base(l) / top.Get(g));
			if (t.color && t.color.Count() >= 3)
				l.SetDiffuseColor(t.color[0], t.color[1], t.color[2]);
			if (t.flicker >= 0)
				l.SetFlickerAmplitude(t.flicker);
			SpotLightBase sp = SpotLightBase.Cast(l);
			if (sp && t.angle > 0)
				sp.SetSpotLightAngle(t.angle);
			n++;
		}
		Print("[AZ_Vamp] light tune applied to " + n + " lights (baked " + baked + ")");
	}

	static void Run(string arg)
	{
		if (arg.IndexOf("lightcfg:") == 0)
		{
			int lid = arg.Substring(9, arg.Length() - 9).ToInt();
			string lpath = AZ_VampNightLighting.NAMALSK_BRIGHT;
			if (lid == AZ_VampNightLighting.ID)
				lpath = AZ_VampNightLighting.PATH;
			else if (lid == 0)
				lpath = AZ_VampNightLighting.VANILLA_DEFAULT;
			GetGame().GetWorld().LoadNewLightingCfg(lpath);
			Print("[AZ_Vamp] lighting cfg " + lid + " loaded: " + lpath);
			return;
		}
		if (arg.IndexOf("mist:") == 0)
		{
			AZ_Crypt_Moon_Grate.AZ_MistRate(arg.Substring(5, arg.Length() - 5).ToFloat());
			return;
		}
		if (arg == "off")
		{
			s_Data = null;
			Apply(true);
			GetGame().Chat("[lights] baked values restored", "colorAction");
			return;
		}
		if (arg == "dump")
		{
			AZ_VampLightTuneData d = new AZ_VampLightTuneData;
			array<ScriptedLightBase> live = All();
			foreach (ScriptedLightBase l : live)
			{
				if (!l)
					continue;
				string g = Group(l);
				if (g == "" || Get(d, g))
					continue;
				AZ_VampLightTuneGroup v = new AZ_VampLightTuneGroup;
				v.radius = l.m_Radius;
				v.brightness = Base(l);
				vector c = l.GetDiffuseColorV();
				v.color = new array<float>;
				v.color.Insert(c[0]);
				v.color.Insert(c[1]);
				v.color.Insert(c[2]);
				if (AZ_VampMoonLight.Cast(l))
					v.angle = 16;
				Set(d, g, v);
			}
			AZ_VampLightTuneGroup rc = new AZ_VampLightTuneGroup;
			rc.radius = 2.5;
			rc.brightness = 1.1;
			d.ritual_candles = rc;
			JsonFileLoader<AZ_VampLightTuneData>.JsonSaveFile(PATH, d);
			GetGame().Chat("[lights] dumped to AppData/Local/DayZ/AZ_VampLightTune.json (do it inside the crypt so every group is loaded)", "colorAction");
			return;
		}
		if (!FileExist(PATH))
		{
			GetGame().Chat("[lights] no AZ_VampLightTune.json yet: /vamp lights dump first", "colorImportant");
			return;
		}
		AZ_VampLightTuneData r = new AZ_VampLightTuneData;
		JsonFileLoader<AZ_VampLightTuneData>.JsonLoadFile(PATH, r);
		s_Data = r;
		Apply(false);
		GetGame().Chat("[lights] reloaded and applied (ritual candles take it on their next lighting)", "colorAction");
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampLightTune.RPC || !GetGame().IsClient())
			return;
		Param1<string> d = new Param1<string>("");
		if (ctx.Read(d))
			AZ_VampLightTune.Run(d.param1);
	}
}



class AZ_VampRiverUpLight extends AZ_VampCryptPointLight
{
	void AZ_VampRiverUpLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(6.5);
		SetBrightnessTo(0.6);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.1, 0.05);
		SetAmbientColor(0.1, 0.01, 0.004);
		SetFlickerAmplitude(0.05);
		SetFlickerSpeed(0.2);
		m_AZBase = 0.6;
	}
}









class AZ_Crypt_Chandelier_Flames extends AZ_Crypt_Chandelier
{
	override bool AZ_LitInCrypt()
	{
		return AZ_VampCryptLights.ALL_CROWNS_LIT;
	}
}

class AZ_Crypt_Chandelier_Snuffed extends AZ_Crypt_Chandelier
{
	override bool AZ_LitInCrypt()
	{
		return false;
	}
}

class AZ_Crypt_Candelabrum extends AZ_VampLitDecor
{
	override bool AZ_LitInCrypt()
	{
		if (AZ_VampCryptLights.LIGHT_CUT)
			return false;
		return AZ_VampCrypt.HallLocal(GetPosition())[1] < 0;
	}

	override typename AZLightType()
	{
		return AZ_VampFaceCandleLight;
	}
}

class AZ_VampFaceCandleLight extends AZ_VampCryptPointLight
{
	void AZ_VampFaceCandleLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(7.0);
		SetBrightnessTo(2.0);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.5, 0.2);
		SetAmbientColor(0.15, 0.07, 0.03);
		SetFlickerAmplitude(0.1);
		SetFlickerSpeed(0.4);
		m_AZF = 57.9;
		m_AZBase = 2.0;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_Register, 100, false);
	}

	void AZ_Register()
	{
		AZ_VampCryptLights.Add(this, false);
		Print("[AZ_Vamp] crypt face candle light at " + GetPosition() + " (left dais candelabrum)");
	}
}


class AZ_VampSlabLight extends AZ_VampCryptPointLight
{
	void AZ_VampSlabLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(4.0);
		SetBrightnessTo(2.2);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.32, 0.08);
		SetAmbientColor(1.0, 0.3, 0.08);
		SetFlickerAmplitude(0.2);
		SetFlickerSpeed(0.5);
		m_AZF = 56.0;
		m_AZBase = 2.2;
	}
}

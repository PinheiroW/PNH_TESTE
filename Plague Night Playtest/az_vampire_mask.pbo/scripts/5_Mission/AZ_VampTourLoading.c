







class AZ_LdMoon
{
	static float Target(int v)
	{
		return 0.35;
	}


	static void SetFull()
	{
		World w = GetGame().GetWorld();
		w.SetDate(2011, 11, 10, 19, 0);
		AZ_VampProgress.FullMoonDate();
		Print("[AZ_Vamp] TEST ld moon full 19:00, intensity " + w.GetMoonIntensity());
	}

	static void Set(int v)
	{
		World w = GetGame().GetWorld();
		float want = Target(v);
		int bestD = 5;
		int bestH = 22;
		int bestM = 30;
		float bd = 10;
		for (int d = 1; d <= 30; d++)
		{
			for (int hm = 0; hm < 6; hm++)
			{
				int hh = 21 + hm / 2;
				int mm = (hm % 2) * 30;
				w.SetDate(2011, 11, d, hh, mm);
				if (w.GetSunOrMoon() > 0.5)
					continue;
				float mi = w.GetMoonIntensity();
				if (Math.AbsFloat(mi - want) < bd)
				{
					bd = Math.AbsFloat(mi - want);
					bestD = d;
					bestH = hh;
					bestM = mm;
				}
			}
		}
		w.SetDate(2011, 11, bestD, bestH, bestM);
		Print("[AZ_Vamp] TEST ld moon " + v + ": Nov " + bestD + " " + bestH + ":" + bestM + ", intensity " + w.GetMoonIntensity() + " (want " + want + ")");
	}
}

modded class MissionServer
{
	static const string LD_LABELS = "tz_ld_r1a|tz_ld_r1b|tz_ld_r1c|tz_ld_r6a|tz_ld_r6b|tz_ld_r6c|tz_ld_r2a|tz_ld_r2b|tz_ld_r2c|tz_ld_r5a|tz_ld_r5b|tz_ld_r5c|tz_ld_r4a|tz_ld_r4b|tz_ld_r4c|tz_ld_r3a|tz_ld_r3b|tz_ld_r3c|tz_ld_r3ea|tz_ld_r3eb|tz_ld_r3ec|tz_ld_r3ma|tz_ld_r3mb|tz_ld_r3mc|tz_ld_r7a|tz_ld_r7b|tz_ld_r7c|tz_ld_r8a|tz_ld_r8b|tz_ld_r8c";
	static bool s_LdDone;
	static vector s_LdA, s_LdD, s_LdB;


	string LdBase(int step)
	{
		if (step == 400 || step == 401)
			return "tz_ld_r1";
		if (step == 402 || step == 403)
			return "tz_ld_r6";
		if (step == 404 || step == 405)
			return "tz_ld_r2";
		if (step == 406 || step == 407)
			return "tz_ld_r5";
		if (step == 408 || step == 409)
			return "tz_ld_r4";
		if (step == 410 || step == 411)
			return "tz_ld_r3";
		if (step == 412 || step == 413)
			return "tz_ld_r3e";
		if (step == 414 || step == 415)
			return "tz_ld_r3m";
		if (step == 416 || step == 417)
			return "tz_ld_r7";
		if (step == 418 || step == 419)
			return "tz_ld_r8";
		return "";
	}

	string LdLabel(int step)
	{
		string b = LdBase(step);
		if (b == "")
			return "";
		return b + "a|" + b + "b|" + b + "c";
	}


	void LdFire()
	{
		Fireplace fire = TzFire();
		TzLightFire(fire);
		if (fire)
			s_TzFireAt = fire.GetPosition();
		Print("[AZ_Vamp] TEST ld fire " + fire + " at " + s_TzFireAt + " dark side " + s_TzVDir);
	}

	override string HwStepLabel(int step)
	{
		if (step >= 400 && step <= 419)
			return LdLabel(step);
		string l = super.HwStepLabel(step);
		if (step == 100 || step == 130)
			return l + "|" + LD_LABELS;
		return l;
	}

	override int HwNextStep(int step)
	{
		if (step >= 400 && step < 419)
			return step + 1;
		if (step == 419)
			return 15;
		int n = super.HwNextStep(step);
		if (n == 15 && s_HwOnly && !s_LdDone && step < 400)
		{
			s_LdDone = true;
			return 400;
		}
		return n;
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step < 400 || step > 419)
		{
			super.HwStep(p, step);
			return;
		}
		if (!p)
			return;
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		int next = LdStep(p, step);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, next, false, p, HwNextStep(step));
	}

	void LdWeather(float fog, float overcast, float snow)
	{
		Weather wt = GetGame().GetWeather();
		wt.GetFog().SetLimits(0, 1);
		wt.GetFog().Set(fog, 0, 900);
		wt.GetOvercast().SetLimits(0, 1);
		wt.GetOvercast().Set(overcast, 0, 900);
		wt.GetRain().Set(0, 0, 900);
		wt.GetSnowfall().SetLimits(0, 1);
		wt.GetSnowfall().Set(snow, 0, 900);
	}


	PlayerBase LdFlareWoman(vector at, vector faceDir)
	{
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		faceDir[1] = 0;
		PlayerBase h = PlayerBase.Cast(GetGame().CreateObject("SurvivorF_Linda", at, false, true));
		if (!h)
			return null;
		h.SetPosition(at);
		h.SetDirection(faceDir.Normalized());
		TzDress(h, 5);
		EntityAI fl = h.GetHumanInventory().CreateInHands("Roadflare");
		if (fl && fl.GetCompEM())
			fl.GetCompEM().SwitchOn();
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		s_TzActors.Insert(h);
		Print("[AZ_Vamp] TEST ld flare woman " + h + " flare " + fl);
		return h;
	}

	vector LdClearCam(vector cam, vector look)
	{
		if (AZ_TzView.Clear(look, cam))
			return cam;
		vector d = look - cam;
		d[1] = 0;
		d.Normalize();
		vector side = Vector(d[2], 0, -d[0]);
		for (int k = 1; k <= 6; k++)
		{
			for (int sg = -1; sg <= 1; sg += 2)
			{
				vector c = cam + side * (k * 0.5 * sg);
				c[1] = cam[1] - GetGame().SurfaceY(cam[0], cam[2]) + GetGame().SurfaceY(c[0], c[2]);
				if (AZ_TzView.Clear(look, c))
				{
					Print("[AZ_Vamp] TEST ld camera stepped " + (k * 0.5 * sg) + " m sideways for a clear view");
					return c;
				}
			}
		}
		Print("[AZ_Vamp] TEST ld camera: no clear spot within 3 m, kept");
		return cam;
	}


	PlayerBase LdSurvivor(string cls, vector at, vector faceTo, int outfit)
	{
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		vector fd = faceTo - at;
		fd[1] = 0;
		PlayerBase h = PlayerBase.Cast(GetGame().CreateObject(cls, at, false, true));
		if (!h)
			return null;
		h.SetPosition(at);
		h.SetDirection(fd.Normalized());
		TzDress(h, outfit);
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		s_TzActors.Insert(h);
		return h;
	}


	void LdShotV(PlayerBase p, int step, int v)
	{
		if (!p)
			return;
		if (step == 411 || step == 413 || step == 415)
			AZ_LdMoon.SetFull();
		else
			AZ_LdMoon.Set(v);
		string vl = LdBase(step) + "a";
		if (v == 1)
			vl = LdBase(step) + "b";
		if (v == 2)
			vl = LdBase(step) + "c";
		vector cam, look, cam2;
		float fov = 0.8;
		if (step == 401)
		{
			vector man = s_TzDoorAt + s_TzDoorOut * 0.6;
			vector rs1 = Vector(s_TzDoorOut[2], 0, -s_TzDoorOut[0]);
			float b1[3] = {1.5, 2.0, 1.1};
			float s1[3] = {0.45, 0.6, 0.3};
			float h1[3] = {1.6, 1.65, 1.15};
			float f1[3] = {0.9, 0.75, 1.0};
			cam = man - s_TzDoorOut * b1[v] + rs1 * s1[v];
			cam[1] = s_TzDoorAt[1] + h1[v];
			look = man + s_TzDoorOut * 16.0 - rs1 * 1.0;
			look[1] = s_TzDoorAt[1] + 1.1;
			fov = f1[v];
		}
		else if (step == 403)
		{
			vector e6 = s_TzDoorAt + s_TzDoorOut * 0.4;
			vector rs6 = Vector(s_TzDoorOut[2], 0, -s_TzDoorOut[0]);
			float b6[3] = {4.5, 7.0, 3.2};
			float s6[3] = {0.8, 1.5, 0.4};
			float h6[3] = {1.2, 1.4, 0.8};
			float f6[3] = {0.75, 0.6, 0.9};
			cam = e6 + s_TzDoorOut * b6[v] + rs6 * s6[v];
			cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + h6[v];
			look = e6 + "0 1.35 0";
			fov = f6[v];
		}
		else if (step == 405)
		{
			vector ls2 = Vector(-s_LdD[2], 0, s_LdD[0]);
			float b2[3] = {1.3, 2.4, 0.9};
			float s2[3] = {0.55, 0.9, 0.4};
			float f2[3] = {0.85, 0.7, 1.0};
			cam = s_LdA - s_LdD * b2[v] + ls2 * s2[v];
			cam[1] = s_LdA[1] + 1.55;
			look = s_LdA + s_LdD * 30.0;
			look[1] = s_LdA[1] + 1.2;
			cam = TzOutOfTrees(cam, s_LdD * -1);
			fov = f2[v];
		}
		else if (step == 407)
		{
			vector th = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
			vector tf = p.GetDirection();
			tf[1] = 0;
			tf.Normalize();
			vector rs5 = Vector(tf[2], 0, -tf[0]);
			float d5[3] = {0.8, 1.8, 3.0};
			float s5[3] = {0.35, 0.5, 0.7};
			float y5[3] = {0.12, 0.25, 0.35};
			float l5[3] = {0.18, 0.5, 0.75};
			float f5[3] = {0.5, 0.75, 0.85};
			cam = th + tf * d5[v] + rs5 * s5[v] - Vector(0, y5[v], 0);
			look = th - Vector(0, l5[v], 0);
			fov = f5[v];
		}
		else if (step == 409)
		{
			vector at9 = s_TzFireAt - s_LdD * 2.2;
			if (s_TzHero)
				at9 = s_TzHero.GetPosition();
			vector rs4 = Vector(s_LdD[2], 0, -s_LdD[0]);
			float b4[3] = {1.1, 1.8, 0.4};
			float s4[3] = {0.55, 0.9, 1.3};
			float h4[3] = {1.6, 1.65, 1.5};
			float f4[3] = {0.6, 0.7, 0.6};
			cam = at9 - s_LdD * b4[v] + rs4 * s4[v];
			cam[1] = at9[1] + h4[v];
			look = at9 + s_LdD * 0.55 + "0 1.5 0";
			fov = f4[v];
		}
		else if (step == 411 || step == 413 || step == 415)
		{
			vector rs3 = Vector(s_LdD[2], 0, -s_LdD[0]);
			float b3[3] = {6.0, 10.0, 4.0};
			float s3[3] = {1.4, 2.5, 0.9};
			float h3[3] = {0.9, 1.4, 0.6};
			float f3[3] = {0.6, 0.5, 0.75};
			cam = s_LdA - s_LdD * b3[v] - rs3 * s3[v];
			cam[1] = Math.Max(GetGame().SurfaceY(cam[0], cam[2]) + 1.0, s_LdA[1] + h3[v]);
			look = s_LdB + "0 4 0";
			fov = f3[v];
		}
		else if (step == 417)
		{
			vector rs7 = Vector(s_LdD[2], 0, -s_LdD[0]);
			if (v == 0)
			{
				cam = s_LdA - s_LdD * 1.6 + rs7 * 0.7;
				cam[1] = s_LdA[1] + 1.65;
				look = s_TzFireAt + "0 0.9 0";
				fov = 0.7;
			}
			else if (v == 1)
			{
				cam = s_LdA + s_LdD * 5.0 + rs7 * 9.0;
				cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + 1.7;
				look = s_LdA + s_LdD * 5.0 + "0 1.0 0";
				fov = 0.85;
			}
			else
			{
				cam = s_LdA - s_LdD * 0.8 - rs7 * 0.6;
				cam[1] = s_LdA[1] + 1.0;
				look = s_TzFireAt + "0 0.8 0";
				fov = 0.5;
			}
		}
		else if (step == 419)
		{
			vector rs8 = Vector(s_TzDoorOut[2], 0, -s_TzDoorOut[0]);
			float b8[3] = {18.0, 24.0, 15.0};
			float s8[3] = {1.2, 3.0, -1.0};
			float h8[3] = {1.6, 2.4, 1.0};
			float f8[3] = {0.7, 0.6, 0.8};
			cam = s_TzDoorAt + s_TzDoorOut * b8[v] + rs8 * s8[v];
			cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + h8[v];
			look = s_TzDoorAt + s_TzDoorOut * 0.6 + "0 1.4 0";
			fov = f8[v];
		}
		if (step != 407 && step != 409)
			cam = LdClearCam(cam, look);
		cam2 = cam + (look - cam).Normalized() * 0.3;
		TzShot(p, vl, cam, look, cam2, look, 4, fov);
	}



	void LdMoonNight(int hour, int minute)
	{
		GetGame().GetWorld().SetDate(2011, 11, 10, hour, minute);
		AZ_VampProgress.FullMoonDate();
	}


	vector LdMoonH()
	{
		World w = GetGame().GetWorld();
		vector up = w.GetSunOrMoonDirection() * -1;
		if (up[1] < 0)
			up = w.GetSunOrMoonDirection();
		up[1] = 0;
		Print("[AZ_Vamp] TEST ld moon dir " + up + " sunOrMoon " + w.GetSunOrMoon() + " moon " + w.GetMoonIntensity());
		if (up.Length() < 0.1 || w.GetSunOrMoon() > 0.5)
			return "0 0 1";
		return up.Normalized();
	}


	void LdTorchAt(vector at)
	{
		at[1] = GetGame().SurfaceY(at[0], at[2]) + 0.05;
		ItemBase tr = ItemBase.Cast(GetGame().CreateObjectEx("Torch", at, ECE_PLACE_ON_SURFACE));
		if (!tr)
			return;
		tr.GetInventory().CreateAttachment("Rag");
		if (tr.GetCompEM())
		{
			tr.GetCompEM().SetEnergy(tr.GetCompEM().GetEnergyMax());
			tr.GetCompEM().SwitchOn();
		}
		if (!tr.IsIgnited())
			tr.OnIgnitedThis(null);
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		s_TzActors.Insert(tr);
		Print("[AZ_Vamp] TEST ld torch at " + at + " lit " + tr.IsIgnited());
	}


	bool LdTreesNear(vector c, float r)
	{
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(c, r, near, null);
		foreach (Object o : near)
		{
			if (o && (o.IsTree() || o.IsBush()))
				return true;
		}
		return false;
	}



	void LdRidge(bool eva, bool moonAhead)
	{
		vector mh = LdMoonH();
		vector best, bestV;
		float bestScore = -1000;
		int villages = 0;
		if (AZ_VampLandmarks.s_Fires)
		{
			foreach (Fireplace vf : AZ_VampLandmarks.s_Fires)
			{
				if (!vf)
					continue;
				villages++;
				vector vc = vf.GetPosition();
				vc[1] = GetGame().SurfaceY(vc[0], vc[2]);
				for (float rx = -300; rx <= 300; rx += 15)
				{
					for (float rz = -300; rz <= 300; rz += 15)
					{
						float rr = Math.Sqrt(rx * rx + rz * rz);
						if (rr < 80 || rr > 300 || GetGame().SurfaceIsSea(vc[0] + rx, vc[2] + rz))
							continue;
						float ry = GetGame().SurfaceY(vc[0] + rx, vc[2] + rz);
						float up = ry - vc[1];
						if (up < 8)
							continue;
						vector qd = Vector(-rx, 0, -rz).Normalized();
						float score = up - rr * 0.02;
						bool ahead = vector.Dot(qd, mh) > 0;
						if (ahead == moonAhead)
							score += 6;
						if (score <= bestScore)
							continue;
						vector q = Vector(vc[0] + rx, ry + 1.7, vc[2] + rz);
						if (!AZ_TzView.Clear(vc + "0 3 0", q))
							continue;
						vector qc = q - qd * 6.0;
						qc[1] = GetGame().SurfaceY(qc[0], qc[2]) + 1.7;
						if (!AZ_TzView.Clear(vc + "0 3 0", qc) || LdTreesNear(q, 5.0))
							continue;
						bestScore = score;
						best = Vector(vc[0] + rx, ry, vc[2] + rz);
						bestV = vc;
					}
				}
			}
		}
		if (bestScore < -999)
		{
			bestV = Vector(TZ_STREET_X, 0, TZ_STREET_Z);
			bestV[1] = GetGame().SurfaceY(bestV[0], bestV[2]);
			best = bestV + "0 0 120";
			best[1] = GetGame().SurfaceY(best[0], best[2]);
		}
		int lit = 0;
		foreach (Fireplace lf : AZ_VampLandmarks.s_Fires)
		{
			if (lf && TzDistXZ(lf.GetPosition(), bestV) < 250)
			{
				TzLightFire(lf);
				lit++;
			}
		}
		s_LdA = best;
		s_LdB = bestV;
		s_LdD = bestV - best;
		s_LdD[1] = 0;
		s_LdD.Normalize();
		if (eva)
			TzEva(best, bestV);
		else
			TzElder(best, bestV, false);
		Print("[AZ_Vamp] TEST ld r3 ridge " + best + " over village " + bestV + " (" + TzDistXZ(best, bestV) + " m, " + (best[1] - bestV[1]) + " m up), score " + bestScore + " of " + villages + " villages, fires lit " + lit + ", eva " + eva + ", moon ahead " + moonAhead);
	}


	vector LdHigh(vector c, float r)
	{
		vector best = c;
		best[1] = GetGame().SurfaceY(c[0], c[2]);
		for (float x = -r; x <= r; x += 10)
		{
			for (float z = -r; z <= r; z += 10)
			{
				float y = GetGame().SurfaceY(c[0] + x, c[2] + z);
				if (y > best[1] && !GetGame().SurfaceIsSea(c[0] + x, c[2] + z))
					best = Vector(c[0] + x, y, c[2] + z);
			}
		}
		return best;
	}

	int LdStep(PlayerBase p, int step)
	{
		if (step % 2 == 1)
		{
			LdShotV(p, step, 0);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LdShotV, 5500, false, p, step, 1);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LdShotV, 11000, false, p, step, 2);
			return 17000;
		}
		vector cam, look, cam2, look2;
		switch (step)
		{
			case 400:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				TzClearMist();
				LdMoonNight(22, 30);
				LdWeather(0.14, 0.6, 0.2);
				if (!TzChurchDoor())
				{
					Print("[AZ_Vamp] TEST ld r1: no church door near " + TZ_CHURCH);
					return 1000;
				}
				TzFlareMan(s_TzDoorAt + s_TzDoorOut * 0.6, s_TzDoorOut);
				vector rside = Vector(s_TzDoorOut[2], 0, -s_TzDoorOut[0]);
				float offs[4] = {-2.5, 1.5, -0.5, 3.0};
				for (int w = 0; w < 4; w++)
				{
					vector wa = s_TzDoorAt + s_TzDoorOut * (12.0 + w * 5.0) + rside * offs[w];
					wa[1] = GetGame().SurfaceY(wa[0], wa[2]);
					TzElder(wa, s_TzDoorAt, w == 1);
				}
				TzPut(p, s_TzDoorAt - s_TzDoorOut * 5.0, s_TzDoorAt);
				return 6000;
			}
			case 402:
			{
				TzClearActors();
				if (s_TzDoorAt.Length() < 1 && !TzChurchDoor())
				{
					Print("[AZ_Vamp] TEST ld r6: no church door");
					return 1000;
				}
				vector ea = s_TzDoorAt + s_TzDoorOut * 0.4;
				TzEva(ea, ea + s_TzDoorOut * 5.0);

				vector rsT = Vector(s_TzDoorOut[2], 0, -s_TzDoorOut[0]);
				LdTorchAt(s_TzDoorAt + s_TzDoorOut * 0.05 + rsT * 1.1);
				LdTorchAt(s_TzDoorAt + s_TzDoorOut * 0.05 - rsT * 1.1);
				TzPut(p, s_TzDoorAt - s_TzDoorOut * 12.0, s_TzDoorAt);
				return 5000;
			}
			case 404:
			{
				TzClearActors();
				LdWeather(0.14, 0.6, 0.15);
				s_LdA = s_TzChainAt;
				s_LdD = s_TzChainDir;
				if (s_LdD.Length() < 0.1)
					s_LdD = "0 0 1";
				s_LdA[1] = GetGame().SurfaceY(s_LdA[0], s_LdA[2]);
				LdFlareWoman(s_LdA, s_LdD);
				vector rs2 = Vector(s_LdD[2], 0, -s_LdD[0]);
				float o2[3] = {-1.5, 0.8, 2.6};
				for (int k = 0; k < 3; k++)
				{
					vector va = s_LdA + s_LdD * (24.0 + k * 5.0) + rs2 * o2[k];
					va[1] = GetGame().SurfaceY(va[0], va[2]);
					TzElder(va, s_LdA, k == 2);
				}
				TzPut(p, s_LdA - s_LdD * 9.0, s_LdA);
				return 6000;
			}
			case 406:
			{
				TzClearActors();
				LdWeather(0.14, 0.6, 0.0);
				LdMoonNight(22, 30);
				LdFire();


				vector m5 = LdMoonH();
				vector tat = s_TzFireAt + m5 * 1.8;
				tat[1] = GetGame().SurfaceY(tat[0], tat[2]);
				TzPut(p, tat, tat + m5);
				TzDressVampire(p, true);
				Print("[AZ_Vamp] TEST ld r5 tester " + p.GetType());
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampFrenzyUntil = GetGame().GetTime() + 60000;
				p.m_AZVampFedUntil = 0;
				AZ_VampReveal.Update(p);
				return 6000;
			}
			case 408:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				LdMoonNight(22, 30);
				LdFire();


				s_LdD = LdMoonH() * -1;
				vector at4 = s_TzFireAt - s_LdD * 2.2;
				TzHero(at4, s_LdD, false);
				if (s_TzHero)
				{
					string fed = AZ_VampEvent.FeedOn(s_TzHero, "SurvivorF_Eva");
					Print("[AZ_Vamp] TEST ld r4: " + fed);
					array<Object> near = new array<Object>;
					GetGame().GetObjectsAtPosition(at4, 2.5, near, null);
					foreach (Object o : near)
					{
						PlayerBase pv = PlayerBase.Cast(o);
						if (pv && pv != s_TzHero && pv != p && pv.m_AZVampire)
						{
							s_TzActors.Insert(pv);
							TzDressVampire(pv, !pv.IsMale());
						}
					}
				}
				TzPut(p, s_TzFireAt - s_TzVDir * 10.0, s_TzFireAt);
				return 2200;
			}
			case 410:
			case 412:
			case 414:
			{
				TzClearActors();
				AZ_LdMoon.SetFull();
				LdWeather(0.14, 0.1, 0.0);
				TzBloodMoon(p, true);
				LdRidge(step != 412, step != 414);
				TzPut(p, s_LdA - s_LdD * 40.0, s_LdA);
				return 16000;
			}
			case 416:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				LdMoonNight(22, 30);
				LdWeather(0.14, 0.6, 0.0);
				LdFire();
				vector out7 = s_TzVDir;
				vector rs7s = Vector(out7[2], 0, -out7[0]);
				LdSurvivor("SurvivorM_Boris", s_TzFireAt - out7 * 1.6 + rs7s * 0.6, s_TzFireAt, 1);
				LdSurvivor("SurvivorF_Irena", s_TzFireAt + rs7s * 1.7, s_TzFireAt, 2);
				LdSurvivor("SurvivorM_Denis", s_TzFireAt - rs7s * 1.6 - out7 * 0.4, s_TzFireAt, 3);
				s_LdA = s_TzFireAt + out7 * 10.0;
				s_LdA[1] = GetGame().SurfaceY(s_LdA[0], s_LdA[2]);
				s_LdD = s_TzFireAt - s_LdA;
				s_LdD[1] = 0;
				s_LdD.Normalize();
				TzEva(s_LdA, s_TzFireAt);
				TzPut(p, s_LdA + out7 * 30.0, s_LdA);
				return 6000;
			}
			case 418:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				LdMoonNight(22, 30);
				LdWeather(0.14, 0.6, 0.2);
				if (s_TzDoorAt.Length() < 1 && !TzChurchDoor())
					return 1000;
				TzFlareMan(s_TzDoorAt + s_TzDoorOut * 0.6, s_TzDoorOut);
				vector rs8s = Vector(s_TzDoorOut[2], 0, -s_TzDoorOut[0]);
				float o8[3] = {-1.8, 0.6, 2.2};
				for (int e8 = 0; e8 < 3; e8++)
				{
					vector ea8 = s_TzDoorAt + s_TzDoorOut * (8.0 + e8 * 2.5) + rs8s * o8[e8];
					ea8[1] = GetGame().SurfaceY(ea8[0], ea8[2]);
					if (e8 == 1)
						TzEva(ea8, s_TzDoorAt);
					else
						TzElder(ea8, s_TzDoorAt, false);
				}
				TzPut(p, s_TzDoorAt + s_TzDoorOut * 45.0, s_TzDoorAt);
				return 6000;
			}
		}
		return 1000;
	}
}

modded class MissionGameplay
{


	protected bool m_LdOn;


	void LdClientNight(string label)
	{
		string nm = label.Substring(0, label.IndexOf("#"));
		string ltr = nm.Substring(nm.Length() - 1, 1);
		int v = 0;
		if (ltr == "b")
			v = 1;
		if (ltr == "c")
			v = 2;
		if (nm.IndexOf("tz_ld_r3") == 0)
			AZ_LdMoon.SetFull();
		else
			AZ_LdMoon.Set(v);
		World w = GetGame().GetWorld();
		Print("[AZ_Vamp] TEST ld client night " + nm + ": moon " + w.GetMoonIntensity() + " sunOrMoon " + w.GetSunOrMoon() + " fog " + GetGame().GetWeather().GetFog().GetActual() + " overcast " + GetGame().GetWeather().GetOvercast().GetActual());
	}


	override void OnEvent(EventType eventTypeId, Param params)
	{
		if (m_LdOn && eventTypeId == ChatMessageEventTypeID)
			return;
		super.OnEvent(eventTypeId, params);
	}

	override void VampHwShot(string label, vector cam, vector target)
	{
		if (label.IndexOf("tz_ld_") == 0 && label.IndexOf("#") > 0)
		{
			LdClientNight(label);
			m_LdOn = true;
			if (m_Chat)
				m_Chat.Clear();
		}
		if (label == "done")
		{
			m_LdOn = false;
			GetGame().GetWorld().SetEyeAccom(-1);
		}
		super.VampHwShot(label, cam, target);
		if (label.IndexOf("tz_ld_r5") == 0)
			m_TzHead = true;
		if (label.IndexOf("tz_ld_") == 0 && label.IndexOf("#") > 0)
			LdExposure(label);
	}



	void LdExposure(string label)
	{
		string nm = label.Substring(0, label.IndexOf("#"));
		string ltr = nm.Substring(nm.Length() - 1, 1);
		World w = GetGame().GetWorld();
		if (ltr == "b")
			w.SetEyeAccom(4.0);
		else if (ltr == "c")
			w.SetEyeAccom(1.0);
		else
			w.SetEyeAccom(-1);
		Print("[AZ_Vamp] TEST ld exposure " + nm + ": eye accom now " + w.GetEyeAccom());
	}
}

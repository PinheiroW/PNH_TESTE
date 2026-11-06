







class AZ_VampMapMark
{
	string key;
	string anchor;
	float x;
	float y;
	float z;
	string text;
	string show = "always";
}

class AZ_VampConfig
{
	int capPercent = 25;
	int capMin = 1;
	int patientZeroPer = 15;
	bool randomPatientZero = false;
	bool ritualFallback = true;
	bool elderFallback = true;
	int elderSeedTarget = 1;
	bool nightOnly = true;
	bool biteCrouched = true;	// Ceremor 11-03: a crouching human can be bitten; the victim clip (STANCEMASK_ERECT) stands them up
	float biteHoldSec = 8;
	float drinkPerSec = 100;
	float drinkMaxSec = 30;
	float finishPerSec = 37.5;
	float deathBlood = 2500;
	float bloodFloor = 3100;
	float fullBlood = 4500;
	float feedGain = 0.7;
	float bagGain = 1.0;
	float sicknessMin = 10;
	float hungryBlood = 3500;
	bool hungerNoHealOn = true;
	bool hungerSensesDullOn = true;
	int starveHunger = 8;
	float starveGraceMin = 3;
	float starveNewbornGraceMin = 10;
	int hbStarvingRangeM = 25;
	float starveHealthPerSec = 0.08;
	float starveHealthFloor = 30;
	bool riverDrinkOn = true;
	float riverDrinkPerSec = 60;
	float riverDrinkTickS = 1;
	float riverDrinkBankHeightM = 0.6;
	float riverDrinkCooldownMin = 20;
	float heartbeatMinGain = 0.30;
	float heartbeatMaxGain = 1.0;
	float vampDrainPerSec = 2.0;
	float vampDrainFloor = 3050;
	float vampRecoverPerSec = 2.0;
	float bareAtWillSec = 20;
	float bloodFadeMin = 0;
	float fedShowSec = 8;
	float biteMarkMin = 20;
	float ritualToolReturnSec = 1200;
	float bodyDamageMultiplier = 0.6;
	float stakeHitMult = 2.0;
	float nightResist = 0.6;
	float dayResist = 0.3;
	float nightHealPerSec = 0.25;
	float dayAiResist = 0.5;
	int starveShakeHunger = 6;
	float shadowRange = 18;
	float chainBiteSec = 6;
	int chainMax = 6;
	float chainWindowSec = 10;
	float shadowCooldownSec = 45;
	bool chainAuto = true;
	// AZ-2627 (Update 10): the frenzy chain's own bite floor (the single strike and the manual bite keep MIN_J_FEED_SEC),
	// the height of JumpPath's feet ray (above kerbs and loot, below a fence), and AZ-2621: under the blood moon the
	// fangs key cannot let go of a victim who is still awake
	float chainMinBiteSec = 2.5;
	float jumpFeetRayM = 0.45;
	bool moonNoLetGo = true;
	float ghoulRiseSec = 10;
	float fullHealPerSec = 1.0;
	float nightFootstepVolume = 0.45;
	float nightStaminaDrain = 0.5;
	float immunityMin = 10;
	float biteCooldownMin = 2;
	float spawnGraceMin = 0;
	float victimCooldownMin = 0;
	float victimCooldownMl = 500;
	float sunBurnPerSec = 1.2;
	// AZ-2514: live-tunable daytime shade recovery and minimum clothing burn.
	bool boltsPierce = true;
	float craftedBoltMult = 0.34;
	bool combatEvidence = true;
	bool bloodMoonDawnDelay = true;
	bool vampireGasDamage = true;
	float dayShadeHealFactor = 1.0;
	float sunClothingBurnFloor = 0.5;
	// Sun burn multipliers by weather (never 0: weather never grants immunity).
	float sunCloudLightMult = 0.4;
	float sunCloudHeavyMult = 0.15;
	float sunFogSnowMult = 0.25;
	float fireSanctuaryM = 15;
	bool bloodMoonImmediate = true;
	int bloodMoonFeeds = 6;
	// AZ-2638: opt-in population scaling; legacy fixed goal remains the default.
	bool bloodMoonScaleOn = false;
	float bloodMoonFeedsPerVampire = 4.0;
	int bloodMoonFeedsMin = 20;
	int bloodMoonFeedsMax = 60;
	bool bloodMoonAlways = false;
	float trailSec = 180;
	float trailStepM = 5;
	float trailLifeSec = 2700;
	float compelSec = 10;
	float compelWindowSec = 60;
	int compelMax = 12;
	float frenzySec = 180;
	bool alignProbe = false;
	int trailCap = 300;
	float scorchStepSec = 4;
	float scorchStepM = 2.5;
	float nightMist = 0.14;
	float frontGapMale = 0;
	bool sharedFeed = false;
	bool shadowStrike = true;
	float strikeRange = 10;
	float strikeCooldownSec = 25;	// Oliver 11-03: was 150; live config set to 25 the same day
	float strikeBiteSec = 6;
	float strikeLightM = 5;
	float kinSenseM = 15;
	float lightRepelM = 12;
	float lightBurnPerSec = 1.5;
	float lightStaminaCap = 15;
	int restSettleSec = 30;
	float restHealPerSec = 0.5;
	int dawnWarnMin = 40;
	int bloodMoonGapNights = 2;
	float fireBurnPerSec = 0.4;
	float fireWoodAtDusk = 2;
	int hunterCacheCount = 6;
	int hunterCacheRefillMin = 45;
	float holyBurnPerSec = 1.2;
	bool halloween = true;


	bool crypt = true;

	int cryptLoiterMin = 0;
	bool altarBloodBag = false;
	float pumpkinInfected = 0.05;
	int fireworksDelaySec = 600;
	float villageMistM = 45;
	float costumeInfected = 0.08;
	int decorLightMax = 4;
	bool torchLightsEnabled = true;


	bool scriptDecor = true;
	bool decorLanterns = true;
	bool decorCandles = true;
	bool decorProps = true;
	int bisectScene = 0;
	int lanternsPerDoor = 2;
	float torchDoorShare = 0.2;
	int spawnLanternsPerDoor = 3;

	float spawnZoneMinZ = 9500;
	string eventStart = "";
	string eventEnd = "";
	bool finale = true;
	bool broadcasts = true;
	string mode = "";




	bool cryptLoot = true;
	string cryptLootSarc = "SalineBagIV,BloodBagIV,TetracyclineAntibiotics,Morphine,Epinephrine,FirstAidKit>BandageDressing+BandageDressing+PainkillerTablets+CharcoalTablets,AZ_VampAmmoBox_357Silver_10Rnd,AmmoBox_762x54_20Rnd,AmmoBox_308Win_20Rnd,AmmoBox_556x45_20Rnd,AmmoBox_9x39_20Rnd";
	int cryptLootSarcPerNight = 6;
	string cryptLootGuns = "SVD|Mag_SVD_10Rnd|PSO1Optic|AK_Suppressor|AmmoBox_762x54_20Rnd,FAL|Mag_FAL_20Rnd|ACOGOptic+Fal_OeBttstck|ImprovisedSuppressor|AmmoBox_308Win_20Rnd,M4A1|Mag_STANAG_30Rnd|ACOGOptic+M4_OEBttstck+M4_RISHndgrd|M4_Suppressor|AmmoBox_556x45_20Rnd,VSS|Mag_VSS_10Rnd|PSO1Optic||AmmoBox_9x39_20Rnd,ASVAL|Mag_VAL_20Rnd|ReflexOptic||AmmoBox_9x39_20Rnd,SCARH|Mag_SCARH_20Rnd|ACOGOptic|ImprovisedSuppressor|AmmoBox_308Win_20Rnd";
	string cryptLootArmor = "PlateCarrierVest+PlateCarrierHolster,PlateCarrierVest+PlateCarrierPouches,BallisticHelmet_Black,BallisticHelmet_Green,NVGHeadstrap+NVGoggles/Battery9V";
	string cryptLootCacheChest = "WoodenCrate";
	int cryptLootCachePerNight = 1;
	string cryptLootCacheArmor = "PlateCarrierVest+PlateCarrierPouches+PlateCarrierHolster,BallisticHelmet_Black,NVGHeadstrap+NVGoggles/Battery9V";
	string cryptLootCacheMedical = "BloodBagIV,SalineBagIV,StartKitIV,TetracyclineAntibiotics,Morphine,Epinephrine,FirstAidKit>BandageDressing+BandageDressing";
	bool cryptLootHoardGun = false;
	int cryptLootHoardItemsPerChest = 1;
	string cryptLootHoard = "PlateCarrierVest+PlateCarrierPouches+PlateCarrierHolster,Mich2001Helmet+NVGoggles/Battery9V";
	int cryptLootHoardFormat = 1;
	string cryptLootHoardLegacy = "PlateCarrierVest+PlateCarrierHolster+PlateCarrierPouches,BallisticHelmet_Black,NVGHeadstrap+NVGoggles/Battery9V,Battery9V,SalineBagIV,TetracyclineAntibiotics,Morphine,Epinephrine,FirstAidKit>BandageDressing+BandageDressing";
	string cryptLootHoardChest = "SeaChest";
	float cryptLootHealthMin = 0.35;
	float cryptLootHealthMax = 0.68;
	float cryptLootRespawnMin = 40;
	int cryptLootHoardPerNight = 4;
	bool oldMapEnabled = false;
	ref array<ref AZ_VampMapMark> oldMapMarks;
	ref array<string> adminUids;
}

class AZ_VampRecord
{
	string uid;
	string name;
	bool vampire;
	float sickLeftSec;
	string sickSource;
	string changedAt;
	int feeds;
	int feedsTotal;
	int cures;
	int slain;
	float humanBase;
	bool bloodied;
	bool howToShown;
	bool shadowHintShown;
	// Update 10 (review J01/F03/F04/F05): what a relog must not reset, written at the final logout (Checkpoint) and read
	// back on connect (Restore). Remaining seconds, so a restart carries them too; old saves load these as 0
	float strikeLeftSec;
	float frenzyLeftSec;
	int downNight;
	int downCount;
	float weakLeftSec;
	float starveElapsedSec;
}

class AZ_VampStateData
{
	ref array<ref AZ_VampRecord> players;
	ref array<string> patientZero;
	bool eventActive;
	bool bloodMoonNext;
	int nightsSinceBloodMoon = 99;
	ref array<string> warned;
	bool stoppedByAdmin;
	bool bloodMoonTonight;
	int bloodMoonRevision;
	bool bloodMoonRevisionActive;
	bool bloodMoonGoalEarnedTonight;
	int nightFeeds;
	int nightVampiresPeak; // Persisted nightly high-water mark; reset at dusk.
	int nightCured;
	int nightIndex;
	bool zeroSeeded;
	float cryptBlood = 0.5;
	int cryptPortalVorkuta;
	int cryptExitNight;
	int cryptHoardFormat0;
	int cryptHoardFormat1;
}

class AZ_VampState
{


	static bool s_ProbeDay;
	static const string DIR = "$profile:AZ_Vampire";
	static const string FILE = "$profile:AZ_Vampire\\state.json";
	static const string CFG = "$profile:AZ_Vampire\\config.json";

	static ref AZ_VampStateData s_Data;
	static ref AZ_VampConfig s_Cfg;
	static ref map<string, int> s_ImmuneUntil;
	static ref map<string, int> s_BiteReadyAt;
	static ref map<string, int> s_SpawnGraceUntil;
	static ref map<string, bool> s_DummyVampire;

	static void Init()
	{
		if (!GetGame().IsServer() || s_Data)
			return;
		s_ImmuneUntil = new map<string, int>;
		s_BiteReadyAt = new map<string, int>;
		s_SpawnGraceUntil = new map<string, int>;
		s_DummyVampire = new map<string, bool>;
		if (!FileExist(DIR))
			MakeDirectory(DIR);

		s_Cfg = new AZ_VampConfig;
		bool cfgLoaded = true;
		if (FileExist(CFG))
		{
			string cfgError;
			cfgLoaded = JsonFileLoader<AZ_VampConfig>.LoadFile(CFG, s_Cfg, cfgError);
			if (!cfgLoaded)
			{
				s_Cfg = new AZ_VampConfig;
				string cfgWarning = "[AZ_Vamp] config.json failed to load: " + cfgError;
				cfgWarning += " - keeping file untouched, using defaults in memory";
				Print(cfgWarning);
			}
		}
		else
			JsonFileLoader<AZ_VampConfig>.JsonSaveFile(CFG, s_Cfg);
		if (!s_Cfg.adminUids)
			s_Cfg.adminUids = new array<string>;
		if (cfgLoaded)
			ReloadCfg(true);

		s_Data = new AZ_VampStateData;
		if (FileExist(FILE))
		{

			string err;
			if (!JsonFileLoader<AZ_VampStateData>.LoadFile(FILE, s_Data, err))
			{
				Print("[AZ_Vamp] state.json unreadable, kept as state.json.bad: " + err);
				CopyFile(FILE, FILE + ".bad");
				s_Data = new AZ_VampStateData;
			}
		}
		if (!s_Data.players)
			s_Data.players = new array<ref AZ_VampRecord>;
		if (!s_Data.patientZero)
			s_Data.patientZero = new array<string>;
		Save();
		AZ_VampEvent.Start();
		Print("[AZ_Vamp] state loaded: " + s_Data.players.Count() + " records, " + CountVampires() + " vampires");
	}



	static const int RELOAD_MS = 30000;
	static int s_CfgReloadAt;
	static string s_CfgText;

	static string ReloadCfg(bool force = false)
	{
		if (!GetGame().IsServer() || !FileExist(CFG))
			return "no config.json";
		string text = "";
		FileHandle fh = OpenFile(CFG, FileMode.READ);
		if (fh)
		{
			string line;
			while (FGets(fh, line) >= 0)
				text += line;
			CloseFile(fh);
		}
		if (!force && text == s_CfgText)
			return "config unchanged";
		AZ_VampConfig fresh = new AZ_VampConfig;
		string err;
		if (!JsonFileLoader<AZ_VampConfig>.LoadFile(CFG, fresh, err))
		{
			Print("[AZ_Vamp] config.json NOT reloaded: " + err);
			s_CfgText = text;
			return "config.json has an error, old values kept: " + err;
		}
		if (!fresh.adminUids)
			fresh.adminUids = new array<string>;
		s_Cfg = fresh;
		ApplyMode(s_Cfg);
		// Missing keys use new defaults; explicit zero remains a supported off value.
		if (!text.Contains("\"hunterCacheCount\"")) s_Cfg.hunterCacheCount = 6;
		if (!text.Contains("\"hunterCacheRefillMin\"")) s_Cfg.hunterCacheRefillMin = 45;
		s_Cfg.hunterCacheCount = AZ_VampDrop.ValidCount(s_Cfg.hunterCacheCount);
		s_Cfg.hunterCacheRefillMin = AZ_VampDrop.ValidRefillMin(s_Cfg.hunterCacheRefillMin);
		if (!text.Contains("\"bagGain\"")) s_Cfg.bagGain = 1.0;
		s_Cfg.bagGain = Math.Clamp(s_Cfg.bagGain, 0, 2);
		s_Cfg.heartbeatMaxGain = Math.Clamp(s_Cfg.heartbeatMaxGain, 0, 1);
		s_Cfg.heartbeatMinGain = Math.Clamp(s_Cfg.heartbeatMinGain, 0, s_Cfg.heartbeatMaxGain);
		s_CfgText = text;
		AZ_VampTorch.ApplyLightConfig(s_Cfg.torchLightsEnabled);
		AZ_VampHeartbeatVolume.SendConfigAll();
		Print("[AZ_Vamp] config.json reloaded");
		return "config reloaded";
	}

	static string SetTorchLights(string value)
	{
		value.ToLower();
		if (value != "on" && value != "off")
			return "usage: /vamp torchlights off|on";
		bool enabled = value == "on";
		Cfg().torchLightsEnabled = enabled;
		JsonFileLoader<AZ_VampConfig>.JsonSaveFile(CFG, s_Cfg);
		s_CfgText = "";
		AZ_VampTorch.ApplyLightConfig(enabled);
		return "torch lights " + value;
	}


	static void ApplyMode(AZ_VampConfig c)
	{
		if (c.mode == "event")
		{
			c.sicknessMin = 5;
			c.patientZeroPer = 5;
			c.bloodMoonFeeds = 3;
			c.bloodMoonGapNights = 0;
			c.finale = true;
			c.broadcasts = true;
		}
		else if (c.mode == "server")
		{
			c.sicknessMin = 20;
			c.patientZeroPer = 15;
			c.bloodMoonFeeds = 6;
			c.bloodMoonGapNights = 2;
			c.finale = false;
			c.broadcasts = false;
		}
		else
			return;
		Print("[AZ_Vamp] mode " + c.mode + ": sicknessMin " + c.sicknessMin + ", patientZeroPer " + c.patientZeroPer + ", bloodMoonFeeds " + c.bloodMoonFeeds + ", gap " + c.bloodMoonGapNights + ", finale " + c.finale + ", broadcasts " + c.broadcasts);
	}

	static AZ_VampConfig Cfg()
	{
		if (!s_Cfg)
			s_Cfg = new AZ_VampConfig;
		if (!s_Cfg.adminUids)
			s_Cfg.adminUids = new array<string>;
		return s_Cfg;
	}

	static void Save()
	{
		if (s_Data)
			JsonFileLoader<AZ_VampStateData>.JsonSaveFile(FILE, s_Data);
	}

	static int CryptHoardFormat(int side)
	{
		if (!s_Data)
			return 0;
		if (side == 0)
			return s_Data.cryptHoardFormat0;
		if (side == 1)
			return s_Data.cryptHoardFormat1;
		return 0;
	}

	static void SetCryptHoardFormat(int side, int format)
	{
		if (!s_Data || format <= 0 || CryptHoardFormat(side) == format)
			return;
		if (side == 0)
			s_Data.cryptHoardFormat0 = format;
		else if (side == 1)
			s_Data.cryptHoardFormat1 = format;
		else
			return;
		Save();
	}


	static string Uid(PlayerBase p)
	{
		if (!p)
			return "";
		if (p.GetIdentity())
			return p.GetIdentity().GetId();
		return "dummy:" + p.GetID().ToString();
	}

	static bool IsDummy(string uid)
	{
		return uid.IndexOf("dummy:") == 0;
	}

	static AZ_VampRecord Find(string uid)
	{
		if (!s_Data)
			return null;
		foreach (AZ_VampRecord r : s_Data.players)
		{
			if (r.uid == uid)
				return r;
		}
		return null;
	}

	static bool IsVampire(PlayerBase p)
	{
		string uid = Uid(p);
		if (uid == "" || !s_Data)
			return false;
		if (IsDummy(uid))
			return s_DummyVampire.Contains(uid) && s_DummyVampire.Get(uid);
		AZ_VampRecord r = Find(uid);
		return r && r.vampire;
	}

	static void SetVampire(PlayerBase p, bool vampire)
	{
		string uid = Uid(p);
		if (uid == "" || !s_Data)
			return;
		if (IsDummy(uid))
		{
			s_DummyVampire.Set(uid, vampire);
			return;
		}
		AZ_VampRecord r = Find(uid);
		if (!r)
		{
			r = new AZ_VampRecord;
			r.uid = uid;
			s_Data.players.Insert(r);
		}
		r.vampire = vampire;
		if (vampire)
		{
			r.feeds = 0;
			if (!r.howToShown && p.GetIdentity() && GetGame().IsServer())
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampAltar.HowTo, AZ_VampAltar.HOWTO_MS, false, p);
			if (GetGame().IsServer())
				p.RemoveAllAgents();
		}
		else
			r.humanBase = p.StatGet(AnalyticsManagerServer.STAT_PLAYTIME);
		if (p.GetIdentity())
			r.name = p.GetIdentity().GetName();
		int y, mo, d, h, mi, s;
		GetYearMonthDay(y, mo, d);
		GetHourMinuteSecond(h, mi, s);
		r.changedAt = string.Format("%1-%2-%3 %4:%5:%6", y, mo.ToStringLen(2), d.ToStringLen(2), h.ToStringLen(2), mi.ToStringLen(2), s.ToStringLen(2));
		Save();
	}

	static float GetSickLeft(PlayerBase p)
	{
		AZ_VampRecord r = Find(Uid(p));
		if (r)
			return r.sickLeftSec;
		return 0;
	}

	static void SetSickLeft(PlayerBase p, float sec)
	{
		string uid = Uid(p);
		if (uid == "" || !s_Data || IsDummy(uid))
			return;
		AZ_VampRecord r = Find(uid);
		if (!r)
		{
			if (sec <= 0)
				return;
			r = new AZ_VampRecord;
			r.uid = uid;
			s_Data.players.Insert(r);
		}
		if (p.GetIdentity())
			r.name = p.GetIdentity().GetName();
		r.sickLeftSec = sec;
		Save();
	}

	// final logout (and an orderly shutdown): the ability cooldowns, tonight's down count, the weakness after the long
	// down, the hunger timer and the remaining sickness go to the record by UID, so a relog cannot shed them
	static void Checkpoint(PlayerBase p, bool save = true)
	{
		if (!GetGame().IsServer() || !p || !s_Data)
			return;
		string uid = Uid(p);
		if (uid == "" || IsDummy(uid))
			return;
		AZ_VampRecord r = Find(uid);
		if (!r)
			return;		// never bitten, never a vampire: nothing to carry
		int now = GetGame().GetTime();
		if (p.m_AZVampTurning && p.m_AZVampSickEnd > 0)
			r.sickLeftSec = Math.Max(1, (p.m_AZVampSickEnd - now) / 1000.0);
		r.strikeLeftSec = 0;
		r.frenzyLeftSec = 0;
		r.weakLeftSec = 0;
		r.starveElapsedSec = 0;
		if (p.m_AZVampire)
		{
			AZ_VampShadowState st = AZ_VampShadow.s_State.Get(p);
			if (st)
			{
				r.strikeLeftSec = Math.Max(0, (st.strikeUntil - now) / 1000.0);
				r.frenzyLeftSec = Math.Max(0, (st.coolUntil - now) / 1000.0);
			}
			r.downNight = p.m_AZDownNight;
			r.downCount = p.m_AZDownCount;
			if (p.m_AZTorporWeak)
				r.weakLeftSec = Math.Max(0, (p.m_AZTorporWeakUntil - now) / 1000.0);
			if (p.m_AZStarveSince >= 0)
				r.starveElapsedSec = Math.Max(0, (now - p.m_AZStarveSince) / 1000.0);
		}
		if (save)
			Save();
	}

	// connect, after Bind: the vampire gets back what it logged out with (the sickness of a human is restored by the
	// mission's own StartTurning path)
	static void Restore(PlayerBase p)
	{
		if (!GetGame().IsServer() || !p || !p.m_AZVampire)
			return;
		AZ_VampRecord r = Find(Uid(p));
		if (!r)
			return;
		int now = GetGame().GetTime();
		if (r.downCount > 0 && r.downNight == AZ_VampProgress.s_NightNo)
		{
			p.m_AZDownNight = r.downNight;
			p.m_AZDownCount = r.downCount;
		}
		if (r.weakLeftSec > 0 && Cfg().torporOn)
		{
			p.m_AZTorporWeakUntil = now + r.weakLeftSec * 1000;
			p.m_AZTorporWeak = true;
			p.SetSynchDirty();
		}
		if (r.starveElapsedSec > 0)
			p.m_AZStarveSince = now - r.starveElapsedSec * 1000;		// StarveTick resets it if they are no longer hungry
		AZ_VampShadow.RestoreCooldowns(p, r.strikeLeftSec * 1000, r.frenzyLeftSec * 1000);
		Print("[AZ_Vamp] restored " + Uid(p) + ": strike " + r.strikeLeftSec + " s, frenzy " + r.frenzyLeftSec + " s, downs " + r.downCount + " (night " + r.downNight + "), weak " + r.weakLeftSec + " s, hungry for " + r.starveElapsedSec + " s");
	}

	static bool GetBloodied(PlayerBase p)
	{
		AZ_VampRecord r = Find(Uid(p));
		return r && r.bloodied;
	}

	static void SetBloodied(PlayerBase p, bool on)
	{
		string uid = Uid(p);
		if (uid == "" || !s_Data || IsDummy(uid))
			return;
		AZ_VampRecord r = Find(uid);
		if (!r || r.bloodied == on)
			return;
		r.bloodied = on;
		Save();
	}

	static int CountVampires()
	{
		int n = 0;
		if (!s_Data)
			return 0;
		foreach (AZ_VampRecord r : s_Data.players)
		{
			if (r.vampire)
				n++;
		}
		return n;
	}

	static void OnlinePlayers(out array<PlayerBase> list)
	{
		list = new array<PlayerBase>;
		array<Man> men = new array<Man>;
		GetGame().GetPlayers(men);
		foreach (Man m : men)
		{
			PlayerBase pb = PlayerBase.Cast(m);
			if (pb && pb.IsAlive())
				list.Insert(pb);
		}
	}

	static int CountOnlineVampires()
	{
		array<PlayerBase> list;
		OnlinePlayers(list);
		int n = 0;
		foreach (PlayerBase pb : list)
		{
			if (IsVampire(pb))
				n++;
		}
		return n;
	}


	static bool CapAllowsTurn(bool reserveElder = true)
	{
		int active = 0;
		if (reserveElder)
			active = AZ_VampAltar.ActiveReservation();
		return CountOnlineVampires() + CountOnlineTurning() + AZ_VampAltar.s_Pending.Count() + active < CapNow();
	}

	static int CountOnlineTurning()
	{
		array<PlayerBase> list;
		OnlinePlayers(list);
		int n = 0;
		foreach (PlayerBase p : list)
		{
			if (!p.m_AZVampire && (p.m_AZVampTurning || GetSickLeft(p) > 0))
				n++;
		}
		return n;
	}


	static int CapNow()
	{
		array<PlayerBase> list;
		OnlinePlayers(list);
		int cap = list.Count() * Cfg().capPercent / 100;
		if (cap < Cfg().capMin)
			cap = Cfg().capMin;
		return cap;
	}

	static bool IsImmune(PlayerBase p)
	{
		string uid = Uid(p);
		return s_ImmuneUntil && s_ImmuneUntil.Contains(uid) && GetGame().GetTime() < s_ImmuneUntil.Get(uid);
	}

	static void SetImmune(PlayerBase p)
	{
		if (s_ImmuneUntil)
			s_ImmuneUntil.Set(Uid(p), GetGame().GetTime() + Cfg().immunityMin * 60000);
	}

	static void ClearImmune(PlayerBase p)
	{
		if (s_ImmuneUntil)
			s_ImmuneUntil.Remove(Uid(p));
	}




	static bool InSpawnGrace(PlayerBase p)
	{
		if (!p || !p.GetIdentity() || !s_SpawnGraceUntil)
			return false;
		string uid = Uid(p);
		return s_SpawnGraceUntil.Contains(uid) && GetGame().GetTime() < s_SpawnGraceUntil.Get(uid);
	}


	static bool InBiteCooldown(PlayerBase p)
	{
		if (!p || !p.GetIdentity() || !s_BiteReadyAt)
			return false;
		string uid = Uid(p);
		return s_BiteReadyAt.Contains(uid) && GetGame().GetTime() < s_BiteReadyAt.Get(uid);
	}

	static bool BiteReady(PlayerBase p, bool bloodMoon = false)
	{
		return !InSpawnGrace(p) && (bloodMoon || !InBiteCooldown(p));
	}



	static void SyncBiteFlags(PlayerBase p)
	{
		if (!p)
			return;
		bool grace = InSpawnGrace(p);
		bool cool = InBiteCooldown(p);
		if (grace != p.m_AZVampGraceSync || cool != p.m_AZVampCoolSync)
		{
			p.m_AZVampGraceSync = grace;
			p.m_AZVampCoolSync = cool;
			p.SetSynchDirty();
		}
	}


	static void SetBiteCooldown(PlayerBase p)
	{
		if (s_BiteReadyAt && p && p.GetIdentity())
			s_BiteReadyAt.Set(Uid(p), GetGame().GetTime() + Cfg().victimCooldownMin * 60000);
	}


	static void SetSpawnGrace(string uid)
	{
		if (s_SpawnGraceUntil && uid != "")
			s_SpawnGraceUntil.Set(uid, GetGame().GetTime() + Cfg().spawnGraceMin * 60000);
	}

	static bool IsNightNow()
	{
		return s_ProbeDay || !Cfg().nightOnly || GetGame().GetWorld().IsNight();
	}
}

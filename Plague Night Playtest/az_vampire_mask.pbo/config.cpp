class CfgPatches
{
	class AZ_VampireMask
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Characters_Tops",
			"DZ_Characters_Masks",
			"DZ_Characters_Headgear",
			"DZ_Scripts",
			"DZ_Sounds_Effects",
			"DZ_Gear_Cooking",
			"DZ_Gear_Crafting",
			"DZ_Gear_Drinks",
			"DZ_Gear_Navigation",
			"DZ_Gear_Consumables",
			"DZ_Gear_Containers",
			"DZ_Weapons_Ammunition",
			"DZ_Weapons_Projectiles",
			"DZ_Weapons_Magazines",
			"DZ_Pistols_Magnum",
			"DZ_Pistols_DE",
			"DZ_Pistols_Derringer",
			"DZ_Weapons_Firearms_Repeater",
			"namalsk_terrain"
		};
	};
};
class CfgSoundShaders
{
	class baseCharacter_SoundShader;
	class AZ_VampHissF_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_f_hiss_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_f_hiss_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_f_hiss_3",
				1
			}
		};
		volume=1;
		range=25;
	};
	class AZ_VampFangsIn_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_fangs_in_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_fangs_in_2",
				1
			}
		};
		volume=0.80000001;
		range=15;
	};
	class AZ_VampHissM_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_m_hiss_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_m_hiss_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_m_hiss_3",
				1
			}
		};
		volume=1;
		range=25;
	};
	class AZ_VampBite_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_bite_nogasp_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_bite_nogasp_2",
				1
			}
		};
		volume=1;
		range=15;
	};
	class AZ_VampFeed_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_feed_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_feed_2",
				1
			}
		};
		volume=0.89999998;
		range=15;
	};
	class AZ_PlagueNight_Menu_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\music\plague_night_menu",
				1
			}
		};
		volume=1;
	};
	class AZ_PlagueNight_BloodMoon_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\music\plague_night_bloodmoon",
				1
			}
		};
		volume=0.69999999;
	};
	class AZ_PlagueNight_BloodMoonDrone_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\music\plague_night_bloodmoon_drone",
				1
			}
		};
		volume=0.34999999;
	};
	class AZ_VampMidnightBell_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_midnight_bell_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_midnight_bell_2",
				1
			}
		};
		volume=0.80000001;
	};
	class AZ_VampChurchBell_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_church_bell_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_church_bell_2",
				1
			}
		};
		volume=1;
		range=2000;
	};
	class AZ_VampNightScream_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_night_scream_f_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_night_scream_f_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_night_scream_m_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_night_scream_m_2",
				1
			}
		};
		volume=0.55000001;
		range=600;
	};
	class AZ_VampHeartbeat_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_heartbeat_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_heartbeat_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_heartbeat_3",
				1
			}
		};
		volume=0.94999999;
		range=80;
	};
	class AZ_VampFrenzyM_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_frenzy_aura",
				1
			}
		};
		volume=0.5;
		range=20;
	};
	class AZ_VampFrenzyF_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_frenzy_aura",
				1
			}
		};
		volume=0.5;
		range=20;
	};
	class AZ_VampSighM_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_sigh_m_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_sigh_m_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_sigh_m_3",
				1
			}
		};
		volume=1;
		range=25;
	};
	class AZ_VampSighF_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_sigh_f_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_sigh_f_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_sigh_f_3",
				1
			}
		};
		volume=1;
		range=25;
	};
	class AZ_VampHumM_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_feed_hum_m",
				1
			}
		};
		volume=0.80000001;
		range=20;
	};
	class AZ_VampHumF_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_feed_hum_f",
				1
			}
		};
		volume=0.80000001;
		range=20;
	};
	class AZ_VampCompel_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_compel_trance",
				1
			}
		};
		volume=0.69999999;
		range=15;
	};
	class AZ_CryptBed_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_bed",
				1
			}
		};
		volume=1;
	};
	class AZ_CryptBreath_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_breath_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_breath_2",
				1
			}
		};
		volume=0.80000001;
	};
	class AZ_CryptReveal_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_reveal",
				1
			}
		};
		volume=1;
	};
	class AZ_CryptBones_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_bones_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_bones_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_bones_3",
				1
			}
		};
		volume=0.89999998;
		range=20;
	};
	class AZ_CryptFar_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_3",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_4",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_5",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_6",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_7",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_8",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_9",
				1
			}
		};
		volume=0.80000001;
		range=40;
	};
	class AZ_OrbHum_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\ORB_ambient",
				1
			}
		};
		volume=0.69999999;
		range=40;
	};
	class AZ_ElderVanish_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\ELDER_vanish",
				1
			}
		};
		volume=0.34999999;
		range=18;
	};
	class AZ_ElderAppear_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\ELDER_appear",
				1
			}
		};
		volume=0.34999999;
		range=18;
	};
	class AZ_CryptBreach_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_3",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_4",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_5",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_6",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_far_7",
				1
			}
		};
		volume=0.60000002;
		range=15;
	};
	class AZ_VampVocal_laugh_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VOCAL_laugh",
				1
			}
		};
		volume=0.60000002;
		range=10;
	};
	class AZ_VampVocal_scent_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VOCAL_scent",
				1
			}
		};
		volume=0.60000002;
		range=6;
	};
	class AZ_VampVocal_sated_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VOCAL_sated",
				1
			}
		};
		volume=0.60000002;
		range=6;
	};
	class AZ_VampVocal_hiss_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VOCAL_hiss",
				1
			}
		};
		volume=0.60000002;
		range=6;
	};
	class AZ_VampLine_enough_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\LINE_enough",
				1
			}
		};
		volume=0.5;
		range=10;
	};
	class AZ_VampLine_wake_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\LINE_wake_choice",
				1
			}
		};
		volume=0.44999999;
	};
	class AZ_VampLine_drinkdry_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\LINE_drinkdry",
				1
			}
		};
		volume=0.34999999;
	};
	class AZ_VampVictimBeat_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VICTIM_beat_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VICTIM_beat_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VICTIM_beat_3",
				1
			}
		};
		volume=0.89999998;
	};
	class AZ_VampVictimBeatWeak_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VICTIM_beat_weak_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VICTIM_beat_weak_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\river\VICTIM_beat_weak_3",
				1
			}
		};
		volume=0.80000001;
	};
	class AZ_VampSwish_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\SWISH_cloth",
				1
			}
		};
		volume=0.69999999;
		range=15;
	};
	class AZ_VampStareBeat_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\STARE_beat",
				1
			}
		};
		volume=0.89999998;
	};
	class AZ_VampLine_stare_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\LINE_stare_mine",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_invite_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\LINE_invite_household",
				1
			}
		};
		volume=0.5;
	};
	class AZ_VampInvite_Pull_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\river\INVITE_pull_drone",
				1
			}
		};
		volume=0.80000001;
	};
	class AZ_VampPortal_Tone_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\PORTAL_tone",
				1
			}
		};
		volume=0.80000001;
		range=15;
	};
	class AZ_VampPortal_Far_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_come_F",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_closer_M",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_waiting_F",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_come_A",
				1
			}
		};
		volume=0.44999999;
		range=14;
	};
	class AZ_VampPortal_Mid_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_waiting_A",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_afraid_A",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_afraid_M",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_chuckle_F",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_closer_A",
				1
			}
		};
		volume=0.55000001;
		range=10;
	};
	class AZ_VampPortal_Near_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_closer_F",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_closer_M",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_come_F",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_come_M",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_come_A",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_afraid_F",
				1
			}
		};
		volume=0.60000002;
		range=6;
	};
	class AZ_VampPortal_ComeDown_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PW_comedown_all",
				1
			}
		};
		volume=0.64999998;
		range=6;
	};
	class AZ_VampPortal_Rim_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\portal\PORTAL_rim",
				1
			}
		};
		volume=0.89999998;
		range=10;
	};
	class AZ_VampBloodMoonRise_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\BLOODMOON_rise",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_CryptRadio_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_radio",
				1
			}
		};
		volume=0.80000001;
		range=7;
	};
	class AZ_CryptSlab_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_slab_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_slab_2",
				1
			}
		};
		volume=1;
		range=40;
	};
	class AZ_CryptAlarmBell_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_church_bell_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_church_bell_2",
				1
			}
		};
		volume=1;
		range=120;
	};
	class AZ_CryptCall_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_call_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_call_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\CRYPT_call_3",
				1
			}
		};
		volume=1;
		range=80;
	};
	class AZ_RitualDrone_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\RIT_drone",
				1
			}
		};
		volume=0.80000001;
	};
	class AZ_RitualPulse_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\RIT_pulse",
				1
			}
		};
		volume=0.80000001;
	};
	class AZ_RitualChoir_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\RIT_choir",
				1
			}
		};
		volume=0.80000001;
	};
	class AZ_RitualEar_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\RIT_ear_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\RIT_ear_2",
				1
			}
		};
		volume=0.89999998;
	};
	class AZ_RitualWake_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\RIT_wake_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\crypt\RIT_wake_2",
				1
			}
		};
		volume=1;
	};
	class AZ_RitualHeart_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\VAMP_heartbeat_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_heartbeat_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\VAMP_heartbeat_3",
				1
			}
		};
		volume=0.80000001;
	};
	class AZ_RitualMurmur_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\RIT_choir_b",
				1
			}
		};
		volume=0.80000001;
		range=40;
	};
	class AZ_VampLine_guide_light_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_guide_light",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_guide_kneel_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_guide_kneel",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_RitualPeak_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\crypt\RIT_peak",
				1
			}
		};
		volume=0.89999998;
	};
	class AZ_VampWhisperBreath_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_hiss_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_hiss_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_hiss_3",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_inhale_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_inhale_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_inhale_3",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_sniff_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_sniff_2",
				1
			}
		};
		volume=0.40000001;
		range=12;
	};
	class AZ_VampWhisperVoice_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_laugh_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_laugh_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_many_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_many_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_name_f_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_name_f_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_name_f_3",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_name_m_2",
				1
			}
		};
		volume=0.40000001;
		range=12;
	};
	class AZ_VampWhisperWord_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_come_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_come_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_here_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_here_2",
				1
			}
		};
		volume=0.40000001;
		range=12;
	};
	class AZ_VampWhisperShhh_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_shhh_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_shhh_2",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_shhh_3",
				1
			}
		};
		volume=0.40000001;
		range=12;
	};
	class AZ_VampLurkLaugh_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_laugh_1",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\WH_laugh_2",
				1
			}
		};
		volume=0.40000001;
		range=12;
	};
	class AZ_VampLine4_moon_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_moon_1_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_moon_1_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_moon_2_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_moon_2_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_moon_2_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_moon_3_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_moon_3_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_moon_3_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_play_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_play_1_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_play_1_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_play_2_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_play_2_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_play_2_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_play_3_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_play_3_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_play_3_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_seduce_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_seduce_1_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_seduce_1_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_seduce_2_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_seduce_2_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_seduce_2_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_seduce_3_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_seduce_3_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_seduce_3_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_seduce_4_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_seduce_4_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_seduce_4_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine4_tease_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_tease_1_f",
				1
			},
			
			{
				"azo\az_vampire_mask\sound\whisper\lines_v4\LINE_tease_1_m",
				1
			}
		};
		volume=0.34999999;
		range=12;
	};
	class AZ_VampLine_altar_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_altar_1",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_altar_2_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_altar_2",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_ante_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_ante_1",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_ante_2_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_ante_2",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_bag_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_bag_1",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_bag_2_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_bag_2",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_exit_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_exit_1",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_exit_2_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_exit_2",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_lurk_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_lurk_1",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_lurk_2_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_lurk_2",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_trap_1_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_trap_1",
				1
			}
		};
		volume=0.30000001;
	};
	class AZ_VampLine_trap_2_SoundShader
	{
		samples[]=
		{
			
			{
				"azo\az_vampire_mask\sound\whisper\lines\LINE_trap_2",
				1
			}
		};
		volume=0.30000001;
	};
};
class CfgSoundSets
{
	class baseCharacter_SoundSet;
	class AZ_VampHissF_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampHissF_SoundShader"
		};
	};
	class AZ_VampFangsIn_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampFangsIn_SoundShader"
		};
	};
	class AZ_VampHissM_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampHissM_SoundShader"
		};
	};
	class AZ_VampBite_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampBite_SoundShader"
		};
	};
	class AZ_VampFeed_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampFeed_SoundShader"
		};
	};
	class AZ_PlagueNight_Menu_SoundSet
	{
		soundShaders[]=
		{
			"AZ_PlagueNight_Menu_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_PlagueNight_BloodMoon_SoundSet
	{
		soundShaders[]=
		{
			"AZ_PlagueNight_BloodMoon_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_PlagueNight_BloodMoonDrone_SoundSet
	{
		soundShaders[]=
		{
			"AZ_PlagueNight_BloodMoonDrone_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampMidnightBell_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampMidnightBell_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampNightScream_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampNightScream_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		loop=0;
		volumeCurve="Constant1Curve";
		sound3DProcessingType="ThunderFar3DProcessingType";
	};
	class AZ_VampChurchBell_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampChurchBell_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		loop=0;
		volumeCurve="Constant1Curve";
		distanceFilter="none";
		sound3DProcessingType="ThunderFar3DProcessingType";
	};
	class AZ_VampHeartbeat_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampHeartbeat_SoundShader"
		};
		volumeCurve="LinearCurve";
		distanceFilter="none";
	};
	class AZ_VampFrenzyM_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampFrenzyM_SoundShader"
		};
		volumeCurve="LinearCurve";
	};
	class AZ_VampFrenzyF_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampFrenzyF_SoundShader"
		};
		volumeCurve="LinearCurve";
	};
	class AZ_VampSighM_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampSighM_SoundShader"
		};
	};
	class AZ_VampSighF_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampSighF_SoundShader"
		};
	};
	class AZ_VampHumM_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampHumM_SoundShader"
		};
	};
	class AZ_VampHumF_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampHumF_SoundShader"
		};
	};
	class AZ_VampCompel_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampCompel_SoundShader"
		};
	};
	class AZ_CryptBed_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptBed_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
		loop=1;
	};
	class AZ_CryptBreath_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptBreath_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_CryptReveal_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptReveal_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_CryptBones_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptBones_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_CryptFar_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptFar_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_OrbHum_SoundSet
	{
		soundShaders[]=
		{
			"AZ_OrbHum_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		loop=1;
		volumeCurve="LinearCurve";
	};
	class AZ_ElderVanish_SoundSet
	{
		soundShaders[]=
		{
			"AZ_ElderVanish_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_ElderAppear_SoundSet
	{
		soundShaders[]=
		{
			"AZ_ElderAppear_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_CryptBreach_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptBreach_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampVocal_laugh_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampVocal_laugh_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampVocal_scent_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampVocal_scent_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampVocal_sated_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampVocal_sated_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampVocal_hiss_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampVocal_hiss_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine_enough_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_enough_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine_wake_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_wake_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
		doppler=0;
	};
	class AZ_VampLine_drinkdry_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_drinkdry_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
		doppler=0;
	};
	class AZ_VampVictimBeat_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampVictimBeat_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampVictimBeatWeak_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampVictimBeatWeak_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampSwish_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampSwish_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampStareBeat_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampStareBeat_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_stare_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_stare_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_invite_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_invite_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampInvite_Pull_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampInvite_Pull_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampPortal_Tone_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampPortal_Tone_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		loop=1;
		volumeCurve="LinearCurve";
	};
	class AZ_VampPortal_Far_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampPortal_Far_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampPortal_Mid_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampPortal_Mid_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampPortal_Near_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampPortal_Near_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampPortal_ComeDown_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampPortal_ComeDown_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampPortal_Rim_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampPortal_Rim_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		loop=1;
		volumeCurve="LinearCurve";
	};
	class AZ_VampBloodMoonRise_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampBloodMoonRise_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_CryptRadio_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptRadio_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		loop=1;
		volumeCurve="LinearCurve";
	};
	class AZ_CryptSlab_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptSlab_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_CryptAlarmBell_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptAlarmBell_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=0.77999997;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_CryptCall_SoundSet
	{
		soundShaders[]=
		{
			"AZ_CryptCall_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_RitualDrone_SoundSet
	{
		soundShaders[]=
		{
			"AZ_RitualDrone_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
		doppler=0;
		loop=1;
	};
	class AZ_RitualPulse_SoundSet
	{
		soundShaders[]=
		{
			"AZ_RitualPulse_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
		doppler=0;
		loop=1;
	};
	class AZ_RitualChoir_SoundSet
	{
		soundShaders[]=
		{
			"AZ_RitualChoir_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
		doppler=0;
		loop=1;
	};
	class AZ_RitualEar_SoundSet
	{
		soundShaders[]=
		{
			"AZ_RitualEar_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_RitualWake_SoundSet
	{
		soundShaders[]=
		{
			"AZ_RitualWake_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_RitualHeart_SoundSet
	{
		soundShaders[]=
		{
			"AZ_RitualHeart_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_RitualMurmur_SoundSet
	{
		soundShaders[]=
		{
			"AZ_RitualMurmur_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		loop=1;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine_guide_light_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_guide_light_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_guide_kneel_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_guide_kneel_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_RitualPeak_SoundSet
	{
		soundShaders[]=
		{
			"AZ_RitualPeak_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
		doppler=0;
	};
	class AZ_VampWhisperBreath_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampWhisperBreath_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampWhisperVoice_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampWhisperVoice_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampWhisperWord_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampWhisperWord_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampWhisperShhh_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampWhisperShhh_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLurkLaugh_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLurkLaugh_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_moon_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_moon_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_moon_2_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_moon_2_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_moon_3_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_moon_3_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_play_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_play_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_play_2_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_play_2_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_play_3_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_play_3_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_seduce_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_seduce_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_seduce_2_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_seduce_2_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_seduce_3_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_seduce_3_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_seduce_4_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_seduce_4_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine4_tease_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine4_tease_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		volumeCurve="LinearCurve";
	};
	class AZ_VampLine_altar_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_altar_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_altar_2_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_altar_2_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_ante_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_ante_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_ante_2_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_ante_2_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_bag_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_bag_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_bag_2_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_bag_2_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_exit_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_exit_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_exit_2_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_exit_2_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_lurk_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_lurk_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_lurk_2_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_lurk_2_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_trap_1_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_trap_1_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class AZ_VampLine_trap_2_SoundSet
	{
		soundShaders[]=
		{
			"AZ_VampLine_trap_2_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
};
class CfgMods
{
	class AZ_VampireMask
	{
		dir="azo\az_vampire_mask";
		name="AZ Vampire Mask";
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		inputs="azo/az_vampire_mask/data/inputs.xml";
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"azo/az_vampire_mask/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"azo/az_vampire_mask/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"azo/az_vampire_mask/scripts/5_Mission"
				};
			};
		};
	};
};
class CfgWorlds
{
	class DefaultLighting
	{
		moonObjectColorFull[]={1,0.94999999,0.89999998,1};
		moonHaloObjectColorFull[]={0.5,0.30000001,0.30000001,0.1};
		moonsetObjectColor[]={0.80000001,0.5,0.40000001};
		moonsetHaloObjectColor[]={0.5,0.30000001,0.25};
	};
	class DefaultWorld;
	class CAWorld: DefaultWorld
	{
		moonObject="azo\az_vampire_mask\data\sky\az_vamp_moon.p3d";
	};
	class namalsk: CAWorld
	{
		spaceTexture0="azo\az_vampire_mask\data\sky\az_milkyway_left_co.paa";
		spaceTexture1="azo\az_vampire_mask\data\sky\az_milkyway_right_co.paa";
	};
};
class CfgVehicles
{
	class Clothing;
	class WoolCoat_ColorBase;
	class MiniDress_ColorBase;
	class Inventory_Base;
	class RopeLocked;
	class PumpkinHelmet;
	class Candle;
	class Torch;
	class GlassBottle;
	class Paper;
	class ChernarusMap;
	class HouseNoDestruct;
	class SeaChest;
	class WoodenCrate;
	class AZ_VampVillagePantry: WoodenCrate
	{
		scope=1;
		displayName="Village pantry";
		descriptionShort="A household food crate, tucked away before the roads emptied.";
	};
	class AZ_VampHunterChest: SeaChest
	{
		scope=1;
		displayName="Hunter's church cache";
		descriptionShort="A weathered sea chest hidden in a church by hunters who knew what walked outside.";
	};
	class AZ_VampWillBonds: RopeLocked
	{
		scope=1;
		displayName="Bound by will";
		descriptionShort="A vampire's will holds your hands together.";
		OnRestrainChange="AZ_VampWillAsh";
	};
	class AZ_VampWillAsh: Inventory_Base
	{
		scope=1;
		displayName="Fading bonds";
		model="\dz\gear\crafting\bp_rope_tied.p3d";
	};
	class AZ_VampPumpkinLantern: PumpkinHelmet
	{
		scope=2;
		displayName="Jack-o'-lantern";
		descriptionShort="A carved pumpkin with a candle inside. Someone in this village still remembers the old nights.";
	};
	class AZ_VampPumpkinHead: PumpkinHelmet
	{
		scope=2;
		displayName="Carved pumpkin (still warm)";
		descriptionShort="It was on the head of one of them. The candle inside is still burning.";
	};
	class AZ_VampCandle: Candle
	{
		scope=2;
		displayName="Candle";
		descriptionShort="Someone lit this for the dead.";
		attachments[]={};
	};
	class AZ_VampRitualCandle: Candle
	{
		scope=2;
		displayName="Altar candle";
		descriptionShort="Black wax on the stone. It wants a flame.";
		attachments[]={};
	};
	class AZ_VampHolyWater: GlassBottle
	{
		scope=2;
		displayName="Holy water";
		descriptionShort="Water from a graveyard font, blessed long ago. They say it washes out the fever of a bite.";
		varQuantityInit=500;
		varLiquidTypeInit=512;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\holywater\az_vamp_holywater_ca.paa"
		};
	};
	class AZ_VampTorch: Torch
	{
		scope=2;
		displayName="Burning torch";
		descriptionShort="It has burned all night. Nobody tends it.";
	};
	class AZ_VampWindowPlanks: HouseNoDestruct
	{
		scope=2;
		displayName="Boarded window (planks)";
		model="\DZ\structures\Proxy_BuildingParts\Barricading\door_planks_01.p3d";
	};
	class AZ_VampCobweb_01: HouseNoDestruct
	{
		scope=2;
		displayName="Cobweb (wall, ground) 1";
		model="\DZ\structures_takistan\Props\Cobwebs\Cobweb_01_wwg.p3d";
	};
	class AZ_VampCobweb_02: AZ_VampCobweb_01
	{
		displayName="Cobweb (corner) 2";
		model="\DZ\structures_takistan\Props\Cobwebs\Cobweb_02_cg.p3d";
	};
	class AZ_VampCobweb_03: AZ_VampCobweb_01
	{
		displayName="Cobweb (corner) 3";
		model="\DZ\structures_takistan\Props\Cobwebs\Cobweb_03_cg.p3d";
	};
	class AZ_VampCobweb_04: AZ_VampCobweb_01
	{
		displayName="Cobweb (wall) 4";
		model="\DZ\structures_takistan\Props\Cobwebs\Cobweb_04_ww.p3d";
	};
	class AZ_VampCobweb_05: AZ_VampCobweb_01
	{
		displayName="Cobweb (wall, ground) 5";
		model="\DZ\structures_takistan\Props\Cobwebs\Cobweb_05_wwg.p3d";
	};
	class AZ_VampCobweb_06: AZ_VampCobweb_01
	{
		displayName="Cobweb (ceiling) 6";
		model="\DZ\structures_takistan\Props\Cobwebs\Cobweb_06_wwc.p3d";
	};
	class AZ_VampCobweb_07: AZ_VampCobweb_01
	{
		displayName="Cobweb (wall, ground) 7";
		model="\DZ\structures_takistan\Props\Cobwebs\Cobweb_07_wg.p3d";
	};
	class AZ_VampSkeleton_Human: HouseNoDestruct
	{
		scope=2;
		displayName="Human skeleton";
		model="\DZ\structures_bliss\Specific\Forest\Forest_HumanSkeleton.p3d";
	};
	class AZ_VampSkeleton_Deer: AZ_VampSkeleton_Human
	{
		displayName="Deer skeleton";
		model="\DZ\structures_bliss\Specific\Forest\Forest_DeerSkeleton_Full.p3d";
	};
	class AZ_VampSkeleton_DeerDam: AZ_VampSkeleton_Human
	{
		displayName="Deer skeleton (scattered)";
		model="\DZ\structures_bliss\Specific\Forest\Forest_DeerSkeleton_Dam.p3d";
	};
	class AZ_VampBonePile: AZ_VampSkeleton_Human
	{
		displayName="Bone pile";
		model="\DZ\structures_bliss\Specific\Forest\Forest_DeerSkeleton_Pile.p3d";
	};
	class AZ_VampSkull_Deer: AZ_VampSkeleton_Human
	{
		displayName="Deer skull";
		model="\DZ\structures_bliss\Specific\Forest\Forest_DeerSkeleton_Skull.p3d";
	};
	class AZ_VampGrave_1: AZ_VampSkeleton_Human
	{
		displayName="Grave 1";
		model="\DZ\structures\Specific\Cemeteries\Cemetery_Grave1.p3d";
	};
	class AZ_VampGrave_2: AZ_VampGrave_1
	{
		displayName="Grave 2";
		model="\DZ\structures\Specific\Cemeteries\Cemetery_Grave2.p3d";
	};
	class AZ_VampGrave_3: AZ_VampGrave_1
	{
		displayName="Grave 3";
		model="\DZ\structures\Specific\Cemeteries\Cemetery_Grave3.p3d";
	};
	class AZ_VampGrave_4: AZ_VampGrave_1
	{
		displayName="Grave 4";
		model="\DZ\structures\Specific\Cemeteries\Cemetery_Grave4.p3d";
	};
	class AZ_VampGraveCross_1: AZ_VampGrave_1
	{
		displayName="Grave cross 1";
		model="\DZ\structures\Specific\Cemeteries\Cemetery_SmallCross.p3d";
	};
	class AZ_VampGraveCross_2: AZ_VampGrave_1
	{
		displayName="Grave cross 2";
		model="\DZ\structures\Specific\Cemeteries\Cemetery_SmallCross2.p3d";
	};
	class AZ_VampMassGrave_8m: AZ_VampGrave_1
	{
		displayName="Mass grave 8 m";
		model="\DZ\structures\Specific\DeadBodies\Dead_MassGrave_8m.p3d";
	};
	class AZ_VampDecor_Base: HouseNoDestruct
	{
		scope=1;
	};
	class AZ_VampDecal_Handprints: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Decal: bloody handprints (wall)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_Handprints.p3d";
	};
	class AZ_VampDecal_BloodCross: AZ_VampDecal_Handprints
	{
		displayName="Decal: blood-smeared cross (wall)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_BloodCross.p3d";
	};
	class AZ_VampDecal_Claws: AZ_VampDecal_Handprints
	{
		displayName="Decal: claw marks (wall, door)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_Claws.p3d";
	};
	class AZ_VampDecal_Graffiti: AZ_VampDecal_Handprints
	{
		displayName="Decal: 'do not open' fang graffiti (wall)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_Graffiti.p3d";
	};
	class AZ_VampDecal_BatSigil: AZ_VampDecal_Handprints
	{
		displayName="Decal: blood bat sigil (wall)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_BatSigil.p3d";
	};
	class AZ_VampDecal_Warding: AZ_VampDecal_Handprints
	{
		displayName="Decal: charcoal warding marks (wall)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_Warding.p3d";
	};
	class AZ_VampDecal_DragMark: AZ_VampDecal_Handprints
	{
		displayName="Decal: blood drag mark (ground)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_DragMark.p3d";
	};
	class AZ_VampDecal_BloodTrail: AZ_VampDecal_Handprints
	{
		displayName="Decal: blood drip trail (ground)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_BloodTrail.p3d";
	};
	class AZ_VampDecal_BloodDrops: AZ_VampDecal_Handprints
	{
		scope=1;
		displayName="Blood splash (vampire trail)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_BloodDrops.p3d";
	};
	class AZ_VampDecal_Scorch: AZ_VampDecal_Handprints
	{
		scope=1;
		displayName="Scorch mark (vampire burned here)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Decal_Scorch.p3d";
	};
	class AZ_VampPlagueNotice: AZ_VampDecal_Handprints
	{
		displayName="Plague quarantine notice (wall)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_PlagueNotice.p3d";
	};
	class AZ_VampWebLarge_Door: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Big cobweb (doorway 1.4 x 2.3 m)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_WebLarge_Door.p3d";
	};
	class AZ_VampWebLarge_Wide: AZ_VampWebLarge_Door
	{
		displayName="Big orb spiderweb (4 x 4 m, between trees)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_WebLarge_Wide.p3d";
	};
	class AZ_VampCoffin_Closed: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Coffin (closed, rope handles)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Coffin_Closed.p3d";
	};
	class AZ_VampCoffin_Cross: AZ_VampCoffin_Closed
	{
		displayName="Coffin (closed, iron cross)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Coffin_Cross.p3d";
	};
	class AZ_VampCoffin_Open: AZ_VampCoffin_Closed
	{
		displayName="Coffin (open, lid aside)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Coffin_Open.p3d";
	};
	class AZ_Crypt_Coffin_Rest: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Coffin (elder's rest)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Coffin_Rest.p3d";
	};
	class AZ_VampCoffin_False: AZ_VampCoffin_Open
	{
		scope=2;
	};
	class AZ_VampCoffin_Earth: AZ_VampCoffin_Closed
	{
		displayName="Coffin (open, filled with grave soil)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Coffin_Earth.p3d";
	};
	class AZ_VampSkull_Human: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Human skull";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Skull_Human.p3d";
	};
	class AZ_VampSkull_Pile: AZ_VampSkull_Human
	{
		displayName="Pile of human skulls";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Skull_Pile.p3d";
	};
	class AZ_VampTombstone_1: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Tombstone (rounded, epitaph)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Tombstone_1.p3d";
	};
	class AZ_VampTombstone_2: AZ_VampTombstone_1
	{
		displayName="Tombstone (broken, leaning)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Tombstone_2.p3d";
	};
	class AZ_VampTombstone_3: AZ_VampTombstone_1
	{
		displayName="Tombstone (Orthodox stone cross)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Tombstone_3.p3d";
	};
	class AZ_VampGrave_Broken: AZ_VampTombstone_1
	{
		displayName="Broken-open grave (slab pushed aside)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Grave_Broken.p3d";
	};
	class AZ_VampVotive: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Blood-red votive candle (lit)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Votive.p3d";
	};
	class AZ_VampHallVotive: AZ_VampVotive
	{
		scope=2;
	};
	class AZ_VampTrapVotive: AZ_VampHallVotive
	{
		scope=2;
	};
	class AZ_VampHunterKit: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Vampire hunter's kit (open case)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_HunterKit.p3d";
	};
	class AZ_VampQuarantineBarrier: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Quarantine barrier (sawhorse)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_QuarantineBarrier.p3d";
	};
	class AZ_VampWindowBlackout: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Boarded window (blackout)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_WindowBlackout.p3d";
	};
	class AZ_VampQuarantineTape: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Quarantine tape (5 m, two stakes)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_QuarantineTape.p3d";
	};
	class AZ_VampSkullPole: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Skull warning pole";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_SkullPole.p3d";
	};
	class AZ_VampSconce: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Iron wall sconce with candle (lit, wall-mounted)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Sconce.p3d";
	};
	class AZ_VampCandelabra: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Iron floor candelabra, 5 candles (lit)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Candelabra.p3d";
	};
	class AZ_VampLampada: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Grave lantern, red glass (lit)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Lampada.p3d";
	};
	class AZ_VampCandleCircle: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Ritual candle circle with blood (lit)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_CandleCircle.p3d";
	};
	class AZ_VampAltarCircle: AZ_VampCandleCircle
	{
		displayName="Blood altar";
	};
	class AZ_VampSkullPile: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Skull heap with melted candles (lit)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_SkullPile.p3d";
	};
	class AZ_VampRagLine: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Rag bunting on two stakes";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_RagLine.p3d";
	};
	class AZ_VampAltar_Blood: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Blood-stained stone altar";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Altar_Blood.p3d";
	};
	class AZ_VampAltarBasin: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Stone basin full of blood (lit)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Altar_Basin.p3d";
	};
	class AZ_VampChainsHanging: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Hanging chains with hooks (origin at the top, hang from a ceiling)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Chains_Hanging.p3d";
	};
	class AZ_Crypt_Walkway_Bay: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: raised walkway bay with steps to the river";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Walkway_Bay.p3d";
	};
	class AZ_Crypt_Nave_Bay: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: sunken nave floor bay";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Nave_Bay.p3d";
	};
	class AZ_Crypt_Blood_Nave: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: blood river surface, one bay";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Blood_Nave.p3d";
		hiddenSelections[]=
		{
			"river"
		};
		hiddenSelectionsMaterials[]=
		{
			"azo\az_vampire_mask\data\decor\az_decor_cryptblood_flow_00.rvmat"
		};
	};
	class AZ_Crypt_River_End: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: basin where the river ends";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_River_End.p3d";
	};
	class AZ_Crypt_Blood_RiverEnd: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: blood surface in the end basin";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Blood_RiverEnd.p3d";
		hiddenSelections[]=
		{
			"river"
		};
		hiddenSelectionsMaterials[]=
		{
			"azo\az_vampire_mask\data\decor\az_decor_cryptblood_flow_00.rvmat"
		};
	};
	class AZ_Crypt_Breach: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: torn bunker wall into the old hall";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Breach.p3d";
	};
	class AZ_Crypt_Crawl_Collapse: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: collapsed crawl at the bunker's end";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Crawl_Collapse.p3d";
	};
	class AZ_Crypt_Stand_Single: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: iron candle stand (unlit)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Stand_Single.p3d";
	};
	class AZ_Crypt_Stand_Three: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: three-arm iron candle stand (unlit)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Stand_Three.p3d";
	};
	class AZ_Crypt_Bank_Low: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: low candle bank (unlit)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Bank_Low.p3d";
	};
	class AZ_Crypt_Bank_Tall: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: tall candle bank (unlit)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Bank_Tall.p3d";
	};
	class AZ_Crypt_Rubble_Wall_A: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: rubble at the wall foot (long)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Rubble_Wall_A.p3d";
	};
	class AZ_Crypt_Rubble_Wall_B: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: rubble at the wall foot (short)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Rubble_Wall_B.p3d";
	};
	class AZ_Crypt_Blood_Debris_A: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: clots on the blood (A)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Blood_Debris_A.p3d";
	};
	class AZ_Crypt_Blood_Debris_B: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: clots on the blood (B)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Blood_Debris_B.p3d";
	};
	class AZ_Crypt_Bones_Arco: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: bones at an arcosolium";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Bones_Arco.p3d";
	};
	class AZ_Crypt_Candelabrum: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: candelabrum on a stone pedestal (unlit)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Candelabrum.p3d";
	};
	class AZ_Crypt_Pillar_A: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: briar pillar A";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Pillar_A.p3d";
	};
	class AZ_Crypt_Pillar_B: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: briar pillar B";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Pillar_B.p3d";
	};
	class AZ_Crypt_Pillar_C: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: briar pillar C";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Pillar_C.p3d";
	};
	class AZ_Crypt_Pillar_D: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: briar pillar D";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Pillar_D.p3d";
	};
	class AZ_Crypt_Pillar_Bare: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: pillar, briar broken away";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Pillar_Bare.p3d";
	};
	class AZ_Crypt_Vine_Fallen: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: fallen briar (visual)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Vine_Fallen.p3d";
	};
	class AZ_Crypt_Fresco_Ritual: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: fresco, the ritual";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Fresco_Ritual.p3d";
	};
	class AZ_Crypt_Fresco_Court: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: fresco, the countess's court";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Fresco_Court.p3d";
	};
	class AZ_Crypt_Fresco_Moon: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: fresco, a red moon over a burning village";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Fresco_Moon.p3d";
	};
	class AZ_Crypt_Apse_Band: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: apse wall band";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Apse_Band.p3d";
	};
	class AZ_Crypt_Arch_Cross: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: transverse arch over the river";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Arch_Cross.p3d";
	};
	class AZ_Crypt_Arch_Arcade: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: arcade arch along a walkway";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Arch_Arcade.p3d";
	};
	class AZ_Crypt_Arch_Arcade_Soot: AZ_Crypt_Arch_Arcade
	{
		displayName="Crypt: arcade arch, sooted over a fire bowl";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Arch_Arcade_Soot.p3d";
	};
	class AZ_Crypt_Wall_Bay: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: outer aisle and wall bay, kneeling frieze";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wall_Bay.p3d";
	};
	class AZ_Crypt_Wall_Bay_Rev: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: outer aisle and wall bay, kneeling frieze (right side, figures mirrored)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wall_Bay_Rev.p3d";
	};
	class AZ_Crypt_Wall_Bay_Arco: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: wall bay with an arcosolium (tomb niche, chest, effigy)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wall_Bay_Arco.p3d";
	};
	class AZ_Crypt_Wall_Bay_Arco_Rev: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: wall bay with an arcosolium (right side)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wall_Bay_Arco_Rev.p3d";
	};
	class AZ_Crypt_Ossuary_Screen: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: bone screen";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ossuary_Screen.p3d";
	};
	class AZ_Crypt_Ante_Niche: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: arched niche with the bat-winged crest";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Niche.p3d";
	};
	class AZ_Crypt_Wall_Bay_Door: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: wall bay with the hidden doorway";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wall_Bay_Door.p3d";
	};
	class AZ_Crypt_Wall_Bay_Door_Rev: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: wall bay with the hidden doorway (right side, figures mirrored)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wall_Bay_Door_Rev.p3d";
	};
	class AZ_Crypt_Vault_Bay: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: vault bay";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Vault_Bay.p3d";
	};
	class AZ_Crypt_Wall_Clere: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: clerestory band under the raised vault";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wall_Clere.p3d";
	};
	class AZ_Crypt_Vault_Step: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: vault step (tympanum) where the vault rises";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Vault_Step.p3d";
	};
	class AZ_Crypt_Blood_Shore: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: blood flood shore decal";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Blood_Shore.p3d";
	};
	class AZ_Crypt_Seal_Side: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: seal behind a wall bay";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Seal_Side.p3d";
	};
	class AZ_Crypt_Seal_Slab: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: seal slab over the vault / under the floor";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Seal_Slab.p3d";
	};
	class AZ_Crypt_Altar: AZ_VampAltar_Blood
	{
		scope=2;
		displayName="Crypt: carved blood altar";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Altar_Carved.p3d";
	};
	class AZ_Crypt_Altar_Rodin: AZ_VampAltar_Blood
	{
		scope=2;
		displayName="Crypt: Rodin blood altar";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Altar_Rodin.p3d";
	};
	class AZ_Crypt_Apse_Pilaster: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: apse pilaster";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Apse_Pilaster.p3d";
	};
	class AZ_Crypt_Ante_Portcullis: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: antechamber portcullis";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Portcullis.p3d";
	};
	class AZ_Crypt_Wall_Impost: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: impost moulding";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wall_Impost.p3d";
	};
	class AZ_Crypt_Wall_Back: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: end wall over the breach";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wall_Back.p3d";
	};
	class AZ_Crypt_Ante_Bay: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: antechamber bay";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Bay.p3d";
	};
	class AZ_Crypt_Ante_End: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: antechamber end wall with the gate arch";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_End.p3d";
	};
	class AZ_Crypt_Portal: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: rock-cut tomb door in the bluff";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Portal.p3d";
	};
	class AZ_Crypt_Vault: AZ_Crypt_Portal
	{
		displayName="Crypt: family vault in the churchyard";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Vault.p3d";
	};
	class AZ_Crypt_Tomb_Grille: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: tomb door grille";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Tomb_Grille.p3d";
	};
	class AZ_Crypt_Tomb_Recess: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: tomb door recess";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Tomb_Recess.p3d";
	};
	class AZ_Crypt_Ante_Leg: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: antechamber dog-leg wall";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Leg.p3d";
	};
	class AZ_Crypt_Ante_Mirror: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: veiled mirror";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Mirror.p3d";
	};
	class AZ_Crypt_Ante_Mourner: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: marble mourner";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Mourner.p3d";
	};
	class AZ_Crypt_Ledge_Mourner: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: ledge balcony mourner";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ledge_Mourner.p3d";
	};
	class AZ_Crypt_Hall_Gibbet_Remains: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: hanging gibbet with remains";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Hall_Gibbet_Remains.p3d";
	};
	class AZ_Crypt_Hall_Gibbet_Open: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: hanging gibbet, open and empty";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Hall_Gibbet_Open.p3d";
	};
	class AZ_Crypt_Ante_Ossuary: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: ossuary shelf";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Ossuary.p3d";
	};
	class AZ_Crypt_Ante_Rug: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: worn runner";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Rug.p3d";
	};
	class AZ_Crypt_Moon_Grate: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: moon grate over the altar";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Moon_Grate.p3d";
	};
	class AZ_Crypt_Countess: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: the countess, marble statue";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess.p3d";
	};
	class AZ_Crypt_Countess_Eyes: AZ_VampDecor_Base
	{
		scope=1;
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess_Eyes.p3d";
	};
	class AZ_Crypt_Countess_Drum_Eyes_1: AZ_VampDecor_Base
	{
		scope=1;
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess_Drum_Eyes_1.p3d";
	};
	class AZ_Crypt_Countess_Drum_Eyes_2: AZ_Crypt_Countess_Drum_Eyes_1
	{
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess_Drum_Eyes_2.p3d";
	};
	class AZ_Crypt_Countess_Drum_Eyes_3: AZ_Crypt_Countess_Drum_Eyes_1
	{
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess_Drum_Eyes_3.p3d";
	};
	class AZ_Crypt_Countess_Lips: AZ_Crypt_Countess_Eyes
	{
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess_Lips.p3d";
	};
	class AZ_Crypt_Countess_Chalice: AZ_Crypt_Countess_Eyes
	{
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess_Chalice.p3d";
	};
	class AZ_Crypt_Countess_Halo: AZ_Crypt_Countess_Eyes
	{
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess_Halo.p3d";
	};
	class AZ_Crypt_Countess_Drum: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: the countess's carved drum";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess_Drum.p3d";
	};
	class AZ_VampAltar_Drum: AZ_VampAltar_Blood
	{
		scope=2;
		displayName="Crypt: the countess's drum altar";
		hiddenSelections[]=
		{
			"skull_eyes"
		};
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Countess_Drum.p3d";
	};
	class AZ_Crypt_Apse_Step_F: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: Countess step (layout F)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Apse_Step_F.p3d";
	};
	class AZ_Crypt_Apse_Step_Box: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: Countess step (layout box)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Apse_Step_Box.p3d";
	};
	class AZ_Crypt_Wolf: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: guardian wolf, marble statue";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Wolf.p3d";
	};
	class AZ_Crypt_Bier: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: the feeding bier, marble statue";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Bier.p3d";
	};
	class AZ_Crypt_Ledge: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: the reveal ledge with stair and ramp";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ledge.p3d";
	};
	class AZ_Crypt_Ledge_Balustrade: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: ledge balustrade";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ledge_Balustrade.p3d";
	};
	class AZ_Crypt_Apse: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: apse with steps, dais and statue plinth";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Apse.p3d";
	};
	class AZ_Crypt_Apse_Niche: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: apse half-dome niche";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Apse_Niche.p3d";
	};
	class AZ_Crypt_Moon_Shaft: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: moon shaft";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Moon_Shaft.p3d";
	};
	class AZ_Crypt_Plug_Crawl: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: rubble plug (crawl)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Plug_Crawl.p3d";
	};
	class AZ_Crypt_Plug_Nook: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: rubble plug (gallery nook)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Plug_Nook.p3d";
	};
	class AZ_Crypt_Plug_Slab: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: rubble plug (gallery slab leg)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Plug_Slab.p3d";
	};
	class AZ_Crypt_Plug_Squeeze: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: rubble plug (tomb squeeze)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Plug_Squeeze.p3d";
	};
	class AZ_Crypt_Spring_Blood: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: blood seeping from the ledge crack";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Spring_Blood.p3d";
	};
	class AZ_Crypt_Chandelier: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: wrought-iron chandelier (lit, origin at the top of its chain)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Chandelier.p3d";
	};
	class AZ_Crypt_Chandelier_Snuffed: AZ_Crypt_Chandelier
	{
		displayName="Crypt: wrought-iron chandelier (snuffed)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Chandelier_Snuffed.p3d";
	};
	class AZ_Crypt_Chandelier_Flames: AZ_Crypt_Chandelier
	{
		displayName="Crypt: wrought-iron chandelier (burning, no light)";
	};
	class AZ_Crypt_Slab_Door: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: hidden slab door";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Slab_Door.p3d";
		class AnimationSources
		{
			class slab
			{
				source="user";
				initPhase=0;
				animPeriod=4;
			};
		};
	};
	class AZ_Crypt_Seal_Cap: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: seal cap";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Seal_Cap.p3d";
	};
	class AZ_Crypt_Outer_Shell: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: outer shell";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Outer_Shell.p3d";
	};
	class AZ_Crypt_Door_Frame: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: side-room door frame";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Door_Frame.p3d";
	};
	class AZ_Crypt_Ossuary: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: ossuary chapel";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ossuary.p3d";
	};
	class AZ_Crypt_Dig_Tomb: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: the dig team's tomb";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Dig_Tomb.p3d";
	};
	class AZ_Crypt_Chamber_Breach: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: dig chamber (breached)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Chamber_Breach.p3d";
	};
	class AZ_Crypt_Slab_Short: AZ_Crypt_Slab_Door
	{
		displayName="Crypt: gallery shortcut slab";
	};
	class AZ_Crypt_Exit_Slab: AZ_Crypt_Slab_Door
	{
		displayName="Crypt: secret exit slab";
	};
	class AZ_Crypt_Exit_Dark: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: secret exit dark";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Exit_Dark.p3d";
	};
	class AZ_Crypt_Gallery: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: Sleepers' Gallery";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Gallery.p3d";
	};
	class AZ_Crypt_Ante_Bay_Nook: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: antechamber bay (nook breach)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Bay_Nook.p3d";
	};
	class AZ_Crypt_Ante_Bay_Slab: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: antechamber bay (shortcut slab)";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Ante_Bay_Slab.p3d";
	};
	class AZ_Crypt_Trap_Causeway: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt trap: causeway";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Trap_Causeway.p3d";
	};
	class AZ_Crypt_Trap_Rubble: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt trap: rubble heap";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Trap_Rubble.p3d";
	};
	class AZ_Crypt_Trap_Boulder: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt trap: boulder";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Trap_Boulder.p3d";
		class AnimationSources
		{
			class roll
			{
				source="user";
				initPhase=0;
				animPeriod=2.5;
			};
		};
	};
	class AZ_Crypt_Trap_Alarm: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt trap: alarm plate";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Trap_Alarm.p3d";
	};
	class AZ_Crypt_Trap_Spikes: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt trap: spike plate";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Trap_Spikes.p3d";
		class AnimationSources
		{
			class spikes
			{
				source="user";
				initPhase=0;
				animPeriod=0.15000001;
			};
		};
	};
	class AZ_Crypt_Trap_Cage: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt trap: iron cage";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Trap_Cage.p3d";
		class AnimationSources
		{
			class drop
			{
				source="user";
				initPhase=0;
				animPeriod=0.44999999;
			};
			class door
			{
				source="user";
				initPhase=0;
				animPeriod=0.80000001;
			};
		};
	};
	class AZ_Crypt_Trap_Hook: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt trap: empty cage hook";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Trap_Hook.p3d";
	};
	class AZ_Crypt_Trap_Winch: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt trap: cage winch";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Trap_Winch.p3d";
	};
	class AZ_Crypt_Brazier: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: tall iron brazier";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Brazier_Tall.p3d";
	};
	class AZ_Crypt_Fire_Bowl: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: iron fire bowl";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Fire_Bowl.p3d";
	};
	class AZ_Crypt_Campfire_Dead: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: burnt-down campfire";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Campfire_Dead.p3d";
	};
	class AZ_Crypt_A1_Crack: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: crack beside the Athena-1 orb";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_A1_Crack.p3d";
	};
	class AZ_Crypt_Deep_Crack: AZ_Crypt_A1_Crack
	{
		displayName="Crypt: the deep passage down (opens after the answer)";
	};
	class AZ_Crypt_Orb_Shell: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: blazing red shell for the A1 orb";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Orb_Shell.p3d";
	};
	class AZ_Crypt_River_Portal: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: blood storm over the river";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Orb_Shell.p3d";
	};
	class AZ_Crypt_Orb_Dim: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: dim red shell for the A1 orb";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Orb_Dim.p3d";
	};
	class AZ_Crypt_Blood_Pool: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: A1 blood flood";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Blood_Pool.p3d";
	};
	class AZ_Crypt_Chamber: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crypt: rock-cut side chamber";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Chamber.p3d";
	};
	class AZ_VampGibbet: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Gibbet post with cage and remains";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Gibbet.p3d";
	};
	class AZ_VampGibbet_Hanging: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Hanging iron cage (origin at chain top, hang under a beam)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Gibbet_Hanging.p3d";
	};
	class AZ_VampShackles: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Wall shackles on chains (wall-mounted)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Shackles.p3d";
	};
	class AZ_VampBanner: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Tattered bat-sigil banner (wall-mounted)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Banner.p3d";
	};
	class AZ_VampShroudedBody: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Shrouded body";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_ShroudedBody.p3d";
	};
	class AZ_VampShroudedBody_Row: AZ_VampShroudedBody
	{
		displayName="Row of shrouded bodies (3)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_ShroudedBody_Row.p3d";
	};
	class AZ_VampStakedCorpse: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Staked vampire in an open coffin";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_StakedCorpse.p3d";
	};
	class AZ_VampLairNest: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Vampire lair nest (mattress, bones, blood)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_LairNest.p3d";
	};
	class AZ_VampDoorWard: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Door ward: crossed stakes and garlic (door-mounted)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_DoorWard.p3d";
	};
	class AZ_VampMirrorCovered: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Mirror covered with a sheet";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_MirrorCovered.p3d";
	};
	class AZ_VampSarcophagus_Ajar: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Stone sarcophagus, lid shoved aside";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Sarcophagus_Ajar.p3d";
	};
	class AZ_VampBloodChalice: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Chalices and bowl of blood on a tray";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_BloodChalice.p3d";
	};
	class AZ_VampBatColony: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Bat colony (ceiling, hangs below origin)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_BatColony.p3d";
	};
	class AZ_VampBloodBottleRack: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Rack of blood bottles";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_BloodBottleRack.p3d";
	};
	class AZ_VampSoilCrate: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Crate of grave earth";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_SoilCrate.p3d";
	};
	class AZ_Crypt_Slab_Bar: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Iron bar across the shortcut slab";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Slab_Bar.p3d";
	};
	class AZ_Crypt_Slab_Eye: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Carved closed eye (shortcut slab)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Slab_Eye.p3d";
	};
	class AZ_Crypt_Slab_Eye_Red: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Carved open eye, red iris (shortcut unlocked)";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_Slab_Eye_Red.p3d";
	};
	class AZ_VampCobwebCorner_S: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Cobweb corner 0.6 m";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_CobwebCorner_S.p3d";
	};
	class AZ_VampCobwebCorner_L: AZ_VampDecor_Base
	{
		scope=2;
		displayName="Cobweb corner 1 m";
		model="\azo\az_vampire_mask\data\decor\AZ_Decor_CobwebCorner_L.p3d";
	};
	class Box_Base;
	class AZ_VampAmmoBox_357Silver_10Rnd: Box_Base
	{
		scope=2;
		displayName="Hunter's silver .357 (box of 10)";
		descriptionShort="A black card box of ten .357 Magnum rounds, each tipped with silver from a melted chalice. Whoever packed them wrote one word on the lid: after dark.";
		model="\dz\weapons\ammunition\357_20RoundBox.p3d";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\silver\az_silver357_box_co.paa"
		};
		debug_ItemCategory=5;
		rotationFlags=17;
		weight=90;
		class Resources
		{
			class AZ_VampAmmo_357Silver
			{
				value=10;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
	class AZ_VampStake: Inventory_Base
	{
		scope=2;
		displayName="Hunter's ash stake";
		descriptionShort="Ash, cut from a churchyard tree, pared down by candlelight and hardened point-first in the fire. The grip is bound in iron from a coffin; the guard is carved as a cross so the hand cannot forget what it holds. Wood does what steel cannot: where it bites a vampire, it burns.";
		model="\azo\az_vampire_mask\data\stake\AZ_VampStake.p3d";
		animClass="Knife";
		rotationFlags=17;
		isMeleeWeapon=1;
		suicideAnim="onehanded";
		inventorySlot[]=
		{
			"Backpack_1"
		};
		weight=420;
		itemSize[]={1,4};
		absorbency=0.30000001;
		soundImpactType="wood";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=120;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"azo\az_vampire_mask\data\stake\az_stake.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"azo\az_vampire_mask\data\stake\az_stake.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"azo\az_vampire_mask\data\stake\az_stake.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"azo\az_vampire_mask\data\stake\az_stake.rvmat"
							}
						},
						
						{
							0,
							
							{
								"azo\az_vampire_mask\data\stake\az_stake.rvmat"
							}
						}
					};
				};
			};
		};
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeSharpLight_3";
				range=1.4;
			};
			class Heavy
			{
				ammo="MeleeSharpHeavy_3";
				range=1.4;
			};
			class Sprint
			{
				ammo="MeleeSharpHeavy_3";
				range=3.3;
			};
		};
	};
	class AZ_VampFangs: Inventory_Base
	{
		scope=2;
		displayName="Vampire fangs";
		descriptionShort="Two long, yellowed fangs, still sharp, pulled from a pile of ash. Proof of a kill no one will believe.";
		model="\azo\az_vampire_mask\data\trophy\AZ_VampFangs.p3d";
		weight=20;
		itemSize[]={1,1};
		rotationFlags=17;
	};
	class AZ_VampSkull_Trophy: Inventory_Base
	{
		scope=2;
		displayName="Vampire skull";
		descriptionShort="A human skull from a heap of ash, the canines too long. Still faintly warm.";
		model="\azo\az_vampire_mask\data\trophy\AZ_VampSkull_Trophy.p3d";
		weight=600;
		itemSize[]={2,2};
		rotationFlags=17;
	};
	class AZ_VampLore_Base: Paper
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
	};
	class AZ_VampLore_1: AZ_VampLore_Base
	{
		scope=2;
		displayName="Foreman's Last Page";
		descriptionShort="Ninth day below. The slab shut behind us at noon. There was no draught, but it shut. Kostya says the floor in the first room clicks. Walk only where the skull is looking. The stone ball sleeps in its cradle. Do not wake it. We opened the long box on the seventh day. By the eighth it was empty.";
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\lore\az_vamp_lore_1_ca.paa"
		};
	};
	class AZ_VampLore_2: AZ_VampLore_Base
	{
		scope=2;
		displayName="Lamp-Keeper's Note";
		descriptionShort="The lamp is the last warm thing. I keep it lit so they stay in their holes. They do not breathe, but they turn over when we talk. Misha drank the bags we brought for the wounded. He says it tastes like the river. He says she is singing. I cannot hear her. I am glad.";
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\lore\az_vamp_lore_2_ca.paa"
		};
	};
	class AZ_VampLore_3: AZ_VampLore_Base
	{
		scope=2;
		displayName="Priest's Warning";
		descriptionShort="To whoever finds this door: close it again. We walled her in with salt and prayer under the last red moon, and the prayer is worn thin. Do not wade the river. Do not kneel. Do not answer when a name you love is called from the niches. Light is the only psalm she fears. Carry it, and do not let it go out.";
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\lore\az_vamp_lore_3_ca.paa"
		};
	};
	class AZ_VampLore_4: AZ_VampLore_Base
	{
		scope=2;
		displayName="Pilgrim's Letter";
		descriptionShort="I crossed the whole length of the river on my knees, and it was warm. I lit her five candles and opened my palm over her stone, and lay down where she could see me. The others knelt where the stone is worn hollow by other knees. It did not hurt. Nothing hurts now. Only the sun. Tell my sister I will visit. At night. Ask her to leave the window open.";
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\lore\az_vamp_lore_4_ca.paa"
		};
	};
	class AZ_VampLore_5: AZ_VampLore_Base
	{
		scope=2;
		displayName="Servant's Ledger Page";
		descriptionShort="My lady sleeps standing, like her statues, and dreams of the moon. I have swept her hall for four hundred winters. I have kept the river full. When the moon turns red she will step down, and the village will remember her name. Whoever reads this: you are already late for her table.";
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\lore\az_vamp_lore_5_ca.paa"
		};
	};
	class AZ_VampLore_6: AZ_VampLore_Base
	{
		scope=2;
		displayName="A Note in a Cold Hand";
		descriptionShort="She wants more of us. We do not. Every one she calls down there is one more mouth at the same few throats. If you see candles burning in her hall, put them out. Or put out whoever lit them.";
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\lore\az_vamp_lore_6_ca.paa"
		};
	};
	class AZ_VampCryptKey: Inventory_Base
	{
		scope=2;
		displayName="Old iron crypt key";
		descriptionShort="A heavy forged key, cold to the touch and flaked with rust. It was lying before the altar, as if left for someone.";
		model="\azo\az_vampire_mask\data\crypt\AZ_Crypt_Key.p3d";
		weight=150;
		itemSize[]={1,2};
		rotationFlags=17;
	};
	class AZ_VampOldMap: ChernarusMap
	{
		scope=2;
		displayName="An old map, marked in dried blood";
		descriptionShort="Vellum gone brown, marked in something that was once red. Whoever wrote on it wanted to be followed.";
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\oldmap\az_vamp_oldmap_closed_co.paa",
			"azo\az_vampire_mask\data\oldmap\az_vamp_oldmap_ca.paa",
			"azo\az_vampire_mask\data\oldmap\az_vamp_oldmap_legend_co.paa"
		};
		selectionPaper="paper";
		world="namalsk";
		itemSize[]={1,2};
		rotationFlags=17;
	};
	class AZ_VampAshPile: Inventory_Base
	{
		scope=1;
		displayName="Pile of ash";
		descriptionShort="Grey ash and charred bits. It still smells of burnt hair.";
		model="\dz\gear\cooking\Fireplace.p3d";
		class AnimationSources
		{
			class Ashes
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class BurntWood
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class Kindling
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class Sticks
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class Wood
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class Stone
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class StonesCircle
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class Tripod
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class Oven
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class LidOn
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class LidOff
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class Inventory
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
		};
		weight=5000;
		itemSize[]={10,10};
		canBeDigged=0;
		carveNavmesh=0;
		physLayer="item_large";
	};
	class AZ_VampElder_Coat: WoolCoat_ColorBase
	{
		scope=1;
		displayName="Elder's Frock Coat";
		descriptionShort="Rotted velvet, cut for a nobleman a century dead. The lace at the throat is stiff with old blood. Elder garments ward off the sun.";
		visibilityModifier=0.85000002;
		hiddenSelectionsTextures[]=
		{
			"\azo\az_vampire_mask\data\costumes\az_vampelder_coat_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_coat_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_coat_co.paa"
		};
	};
	class AZ_VampElder_DressSick: MiniDress_ColorBase
	{
		itemsCargoSize[]={5,4};
		scope=1;
		displayName="Grave-Cold Dress";
		descriptionShort="Ash-grey and frost-stiff, as if it had lain under the snow all winter. Dried blood runs down the front. Elder garments ward off the sun.";
		hiddenSelectionsTextures[]=
		{
			"\azo\az_vampire_mask\data\costumes\az_vampelder_dresssick_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_dresssick_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_dresssick_co.paa"
		};
	};
	class AZ_VampElder_DressBeauty: MiniDress_ColorBase
	{
		itemsCargoSize[]={5,4};
		scope=1;
		displayName="Countess's Velvet Dress";
		descriptionShort="Deep red velvet under black lace, still beautiful. Only a few dark drops at the collar. Elder garments ward off the sun.";
		hiddenSelectionsTextures[]=
		{
			"\azo\az_vampire_mask\data\costumes\az_vampelder_dressbeauty_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_dressbeauty_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_dressbeauty_co.paa"
		};
	};
	class AZ_VampElder_GreatCoat: WoolCoat_ColorBase
	{
		scope=2;
		displayName="Elder's Greatcoat";
		descriptionShort="Ankle-length oxblood velvet with a shoulder cape, its hem torn to rags. The lace at the throat is stiff with old blood. Elder garments ward off the sun.";
		model="\azo\az_vampire_mask\AZ_VampElder_GreatCoat_g.p3d";
		weight=3600;
		visibilityModifier=0.85000002;
		hiddenSelections[]=
		{
			"camoGround",
			"camoMale",
			"camoFemale"
		};
		hiddenSelectionsTextures[]=
		{
			"\azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"\azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat.rvmat",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat.rvmat",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat.rvmat"
		};
		class ClothingTypes
		{
			male="\azo\az_vampire_mask\AZ_VampElder_GreatCoat_m.p3d";
			female="\azo\az_vampire_mask\AZ_VampElder_GreatCoat_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=140;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_greatcoat_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class AZ_VampElder_GraveGown: MiniDress_ColorBase
	{
		itemsCargoSize[]={5,4};
		scope=2;
		displayName="Grave Gown";
		descriptionShort="Floor-length cloth pale as snow under rags of burial gauze, frost still in the hem. Blood has dried down the bodice. Elder garments ward off the sun.";
		model="\azo\az_vampire_mask\AZ_VampElder_GraveGown_g.p3d";
		weight=2400;
		itemSize[]={4,4};
		heatIsolation=0.5;
		hiddenSelections[]=
		{
			"camoGround",
			"camoMale",
			"camoFemale"
		};
		hiddenSelectionsTextures[]=
		{
			"\azo\az_vampire_mask\data\costumes\az_vampelder_gravegown_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_gravegown_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_gravegown_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"\azo\az_vampire_mask\data\costumes\az_vampelder_gravegown.rvmat",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_gravegown.rvmat",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_gravegown.rvmat"
		};
		class ClothingTypes
		{
			male="\azo\az_vampire_mask\AZ_VampElder_GraveGown_m.p3d";
			female="\azo\az_vampire_mask\AZ_VampElder_GraveGown_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=75;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_gravegown.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_gravegown.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_gravegown_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_gravegown_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_gravegown_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class AZ_VampElder_VelvetGown: MiniDress_ColorBase
	{
		itemsCargoSize[]={5,4};
		scope=2;
		displayName="Countess's Velvet Gown";
		descriptionShort="Floor-length crimson velvet, a black lace bodice and long lace sleeves, a velvet ribbon at the throat. Elder garments ward off the sun.";
		model="\azo\az_vampire_mask\AZ_VampElder_VelvetGown_g.p3d";
		weight=2400;
		itemSize[]={4,4};
		heatIsolation=0.5;
		hiddenSelections[]=
		{
			"camoGround",
			"camoMale",
			"camoFemale",
			"personality"
		};
		hiddenSelectionsTextures[]=
		{
			"\azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown_co.paa",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"\azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown.rvmat",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown.rvmat",
			"\azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown.rvmat"
		};
		class ClothingTypes
		{
			male="\azo\az_vampire_mask\AZ_VampElder_VelvetGown_m.p3d";
			female="\azo\az_vampire_mask\AZ_VampElder_VelvetGown_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=75;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"azo\az_vampire_mask\data\costumes\az_vampelder_velvetgown_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class AZ_VampireMask_Base: Clothing
	{
		scope=0;
		descriptionShort="Local test item.";
		inventorySlot[]=
		{
			"Mask"
		};
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		weight=40;
		itemSize[]={2,2};
		noEyewear=0;
		varWetMax=0.249;
		heatIsolation=0;
		class Protection
		{
			chemical=1;
			dust_particle_breath=1;
		};
	};
	class AZ_VampireMask: AZ_VampireMask_Base
	{
		scope=2;
		displayName="Vampire (fangs retracted)";
		model="\azo\az_vampire_mask\AZ_VampireMask_base_g.p3d";
		class ClothingTypes
		{
			male="\azo\az_vampire_mask\AZ_VampireMask_base_m.p3d";
			female="\azo\az_vampire_mask\AZ_VampireMask_base_f.p3d";
		};
	};
	class AZ_VampireMask_Bared: AZ_VampireMask_Base
	{
		scope=2;
		displayName="Vampire (fangs bared, red eyes)";
		model="\azo\az_vampire_mask\AZ_VampireMask_bared_g.p3d";
		class ClothingTypes
		{
			male="\azo\az_vampire_mask\AZ_VampireMask_bared_m.p3d";
			female="\azo\az_vampire_mask\AZ_VampireMask_bared_f.p3d";
		};
	};
	class AZ_VampireMask_Fed: AZ_VampireMask_Base
	{
		scope=2;
		displayName="Vampire (fangs bared, red eyes, fed)";
		model="\azo\az_vampire_mask\AZ_VampireMask_fed_g.p3d";
		class ClothingTypes
		{
			male="\azo\az_vampire_mask\AZ_VampireMask_fed_m.p3d";
			female="\azo\az_vampire_mask\AZ_VampireMask_fed_f.p3d";
		};
	};
	class AZ_VampireMask_Frenzy: AZ_VampireMask_Fed
	{
		displayName="Vampire (fangs bared, red eyes, frenzy)";
	};
};
class cfgAmmoTypes
{
	class AType_AZ_VampBullet_357Silver
	{
		name="AZ_VampBullet_357Silver";
	};
};
class CfgAmmo
{
	class Bullet_357;
	class AZ_VampBullet_357Silver: Bullet_357
	{
		spawnPileType="AZ_VampAmmo_357Silver";
	};
};
class CfgMagazines
{
	class Ammo_357;
	class AZ_VampAmmo_357Silver: Ammo_357
	{
		scope=2;
		displayName="Silver .357 rounds";
		descriptionShort=".357 Magnum, the lead nose swapped for silver by a patient hand. Wasted on the living. A vampire hit by one cannot shrug it off; the flesh around it smokes.";
		ammo="AZ_VampBullet_357Silver";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"azo\az_vampire_mask\data\silver\az_silver357_rounds_co.paa"
		};
	};
	class Magazine_Base;
	class Mag_357Speedloader_6Rnd: Magazine_Base
	{
		ammoItems[]=
		{
			"Ammo_357",
			"AZ_VampAmmo_357Silver"
		};
	};
	class Mag_Deagle_9rnd: Magazine_Base
	{
		ammoItems[]=
		{
			"Ammo_357",
			"AZ_VampAmmo_357Silver"
		};
	};
};
class Muzzle_Base;
class CfgWeapons
{
	class Pistol_Base;
	class Rifle_Base;
	class Magnum_Base: Pistol_Base
	{
		chamberableFrom[]=
		{
			"Ammo_357",
			"Mag_357Speedloader_6Rnd",
			"AZ_VampAmmo_357Silver"
		};
		class SecondMuzzle: Muzzle_Base
		{
			chamberableFrom[]=
			{
				"Ammo_357",
				"AZ_VampAmmo_357Silver"
			};
		};
	};
	class Deagle_Base: Pistol_Base
	{
		chamberableFrom[]=
		{
			"Ammo_357",
			"AZ_VampAmmo_357Silver"
		};
	};
	class Derringer_Base: Pistol_Base
	{
		chamberableFrom[]=
		{
			"Ammo_357",
			"AZ_VampAmmo_357Silver"
		};
		class SecondMuzzle: Muzzle_Base
		{
			chamberableFrom[]=
			{
				"Ammo_357",
				"AZ_VampAmmo_357Silver"
			};
		};
	};
	class Repeater_Base: Rifle_Base
	{
		chamberableFrom[]=
		{
			"Ammo_357",
			"AZ_VampAmmo_357Silver"
		};
	};
};

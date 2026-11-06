// Generated from the installed vanilla player item profiles.
class HumansWeapons
{
 static const int MAX_KIND = 13;
 static ref array<string> Types = {"", "Crowbar", "BaseballBat", "NailedBaseballBat", "BarbedBaseballBat", "Wrench", "LugWrench", "PoliceBaton", "MeatTenderizer", "Pipe", "Machete", "CrudeMachete", "OrientalMachete", "Hammer"};
 static ref array<string> Profiles = {"Humans/anims/Human.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_2h_bat.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_2h_bat.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_2h_bat.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", "DZ/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi"};
 static ref array<string> Sounds = {"Char_Attack_unarmed_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet", "Char_Attack_fireaxe_light1_SoundSet", "Char_Attack_fireaxe_light1_SoundSet", "Char_Attack_fireaxe_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet", "Char_Attack_pipemetal_light1_SoundSet"};
 static ref array<int> Families = {0, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1};
 static ref array<float> HitTimes = {0.333333, 0.500000, 0.500000, 0.500000, 0.500000, 0.500000, 0.500000, 0.500000, 0.500000, 0.500000, 0.500000, 0.500000, 0.500000, 0.500000};
 static ref array<float> SoundTimes = {0.166667, 0.333333, 0.266667, 0.266667, 0.266667, 0.333333, 0.333333, 0.333333, 0.333333, 0.333333, 0.333333, 0.333333, 0.333333, 0.333333};
 static int ClampKind(int kind) { return Math.Clamp(kind, 0, MAX_KIND); }
 static int Family(int kind) { return Families[ClampKind(kind)]; }
}

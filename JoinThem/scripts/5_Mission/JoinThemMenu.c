class JTChoiceMenu:UIScriptedMenu
{
 override Widget Init(){layoutRoot=GetGame().GetWorkspace().CreateWidgets("JoinThem/gui/Choice.layout");return layoutRoot;}
 override void OnShow(){super.OnShow();GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);GetGame().GetUIManager().ShowUICursor(true);}
 override void OnHide(){super.OnHide();GetGame().GetMission().PlayerControlEnable(true);GetGame().GetUIManager().ShowUICursor(false);}
 override bool OnClick(Widget w,int x,int y,int button)
 {
  if(w.GetName()!="JoinButton" && w.GetName()!="RespawnButton")return super.OnClick(w,x,y,button);
  PlayerBase player=PlayerBase.Cast(GetGame().GetPlayer());
  if(!player || player.m_JTState!=1)return true;
  bool join=w.GetName()=="JoinButton";
  GetGame().RPCSingleParam(player,981301,new Param1<bool>(join),true);
  return true;
 }
}
modded class MissionBase
{
 override UIScriptedMenu CreateScriptedMenu(int id)
 {
  if(id==981300)return new JTChoiceMenu;
  return super.CreateScriptedMenu(id);
 }
}
modded class MissionGameplay
{
 float m_JTInputAt;
 Widget m_JTSkinPrompt;
 bool m_JTSkinPromptVisible;
 void JTUpdateSkinPrompt(PlayerBase player)
 {
  bool show=false;
  if(player && player.m_JTState==2 && player.IsAlive() && !player.IsUnconscious() && !GetGame().GetUIManager().GetMenu() && !GetGame().GetUIManager().IsCursorVisible())
  {
   ActionManagerClient manager=ActionManagerClient.Cast(player.GetActionManager());
   if(manager && !manager.GetRunningAction())show=manager.GetPossibleAction(JTSkinSurvivorInput)!=null;
  }
  if(show && !m_JTSkinPrompt)m_JTSkinPrompt=GetGame().GetWorkspace().CreateWidgets("JoinThem/gui/SkinPrompt.layout");
  if(m_JTSkinPrompt)m_JTSkinPrompt.Show(show);
  if(show && !m_JTSkinPromptVisible)Print("[JoinThem] Skin survivor prompt shown for available Mouse 2 action.");
  m_JTSkinPromptVisible=show;
 }
 override void OnMissionFinish()
 {
  if(m_JTSkinPrompt){m_JTSkinPrompt.Unlink();m_JTSkinPrompt=null;}
  super.OnMissionFinish();
 }
 override void OnUpdate(float timeslice)
 {
  super.OnUpdate(timeslice);
  PlayerBase player=PlayerBase.Cast(GetGame().GetPlayer());
  JTUpdateSkinPrompt(player);if(!player)return;
  if(player.m_JTState==1)
  {
   if(!JTChoiceMenu.Cast(GetGame().GetUIManager().GetMenu()))GetGame().GetUIManager().EnterScriptedMenu(981300,null);
   return;
  }
  JTChoiceMenu choice=JTChoiceMenu.Cast(GetGame().GetUIManager().GetMenu());
  if(choice)choice.Close();
  if(player.m_JTState==4 && !player.IsAlive())
  {
   if(!IsPlayerRespawning())
   {
    GetGame().GetMenuDefaultCharacterData(false).SetRandomCharacterForced(true);
    GetGame().RespawnPlayer();SetPlayerRespawning(true);Continue();
   }
   return;
  }
  if(player.m_JTState!=2 || player.IsUnconscious() || !player.IsAlive())return;
  if(player.GetHumanInventory().GetEntityInHands())return;

  if(GetGame().GetUIManager().GetMenu())return;
  float now=GetGame().GetTime()*0.001;
  if(now<m_JTInputAt)return;
  bool light=GetGame().GetInput().LocalPress("UAFire",false);
  bool heavy=false;
  if(!light && !heavy)return;
  m_JTInputAt=now+0.9;
  if(heavy)m_JTInputAt=now+1.5;
  if(player.GetCommand_Move())
  {
   int hit=0;if(heavy)hit=1;
   EntityAI target=player.JTTarget(1.9,true);
   player.StartCommand_Melee2(target,hit,0,player.GetPosition()+player.GetDirection()+"0 1 0");
  }
  GetGame().RPCSingleParam(player,981302,new Param1<bool>(heavy),true);
 }
}








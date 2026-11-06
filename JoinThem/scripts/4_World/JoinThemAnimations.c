modded class ModItemRegisterCallbacks
{
 override void RegisterEmptyHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
 {
  super.RegisterEmptyHanded(pType,pBehavior);
  pType.AddItemInHandsProfileIK("JoinThemAnimationDummy","JoinThem/anims/Joined.asi",pBehavior,"");
 }
}

// All prices, ownership, quantities and balances are resolved on the server.
class HumanTraderEconomy
{
 static ref HumanTradersConfig Config;
 static ref map<string,ref HTWallet> Wallets=new map<string,ref HTWallet>;
 static ref array<ItemBase> Sold={};
 static const string DIRECTORY="$profile:Humans";
 static const string CONFIG_PATH="$profile:Humans/HumanTraders.json";
 static const int MAX_MONEY=1000000000;
 static HumanTradersConfig Load()
 {
  if(Config)return Config;
  MakeDirectory(DIRECTORY);MakeDirectory(DIRECTORY+"/TraderWallets");
  string error;
  if(FileExist(CONFIG_PATH))
  {
   if(!JsonFileLoader<HumanTradersConfig>.LoadFile(CONFIG_PATH,Config,error) || !Config)
   {
    Print("[HumanTraders] Invalid JSON; trader disabled. "+error);
    Config=new HumanTradersConfig;Config.traderspawn=0;return Config;
   }
  }
  else
  {
   Config=new HumanTradersConfig;Config.Locations.Insert(new HTLocation);
   DefaultStock(Config);
   if(!JsonFileLoader<HumanTradersConfig>.SaveFile(CONFIG_PATH,Config,error))Print("[HumanTraders] Config save failed: "+error);
  }
  if(!Config.Items)Config.Items={};if(!Config.Locations)Config.Locations={};
  Config.RespawnMinutes=Math.Clamp(Config.RespawnMinutes,1,10080);
  Config.WanderRadius=Math.Clamp(Config.WanderRadius,0,40);Config.DefenseRadius=Math.Clamp(Config.DefenseRadius,5,80);
  Config.HearingRange=Math.Clamp(Config.HearingRange,0,20);
  Config.GreetingMinDistance=Math.Clamp(Config.GreetingMinDistance,2,10);
  Config.GreetingMaxDistance=Math.Clamp(Config.GreetingMaxDistance,Config.GreetingMinDistance+1,20);
  Config.GreetingCooldownSeconds=Math.Clamp(Config.GreetingCooldownSeconds,15,3600);
  array<string> seen={};
  for(int i=Config.Items.Count() - 1;i>=0;i--)
  {
   HTCatalogItem entry=Config.Items[i];
   string key="";if(entry){key=entry.ClassName;key.ToLower();}
   if(!entry || !ValidInventoryClass(entry.ClassName) || seen.Find(key)>=0)
   {Print("[HumanTraders] Invalid/duplicate inventory classname skipped at index "+i.ToString());Config.Items.Remove(i);continue;}
   entry.BuyPrice=Math.Clamp(entry.BuyPrice,-1,100000000);entry.SellPrice=Math.Clamp(entry.SellPrice,-1,100000000);
   if(entry.BuyPrice>=0 && entry.SellPrice>entry.BuyPrice)entry.SellPrice=entry.BuyPrice;
   seen.Insert(key);
  }
  return Config;
 }
 static string ConfigPath(string classname)
 {
  array<string> roots={"CfgVehicles","CfgMagazines","CfgWeapons"};
  foreach(string root:roots)if(GetGame().ConfigIsExisting(root+" "+classname))return root+" "+classname;
  return "";
 }
 static bool ValidInventoryClass(string classname)
 {
  if(ConfigPath(classname)=="")return false;
  return GetGame().IsKindOf(classname,"Inventory_Base") || GetGame().IsKindOf(classname,"Magazine_Base") || GetGame().IsKindOf(classname,"Weapon_Base");
 }
 static void DefaultStock(HumanTradersConfig config)
 {
  // Enumerate playable ammunition classes from the actual loaded game config.
  array<string> roots={"CfgMagazines","CfgVehicles"};
  foreach(string root:roots)
  {
   int count=GetGame().ConfigGetChildrenCount(root);
   for(int i=0;i<count;i++)
   {
    string classname;GetGame().ConfigGetChildName(root,i,classname);
    if(GetGame().ConfigGetInt(root+" "+classname+" scope")!=2 || !ValidInventoryClass(classname))continue;
    if(!GetGame().IsKindOf(classname,"Magazine_Base") && classname.IndexOf("AmmoBox_")!=0)continue;
    int buy=100;if(classname.IndexOf("Mag_")==0)buy=250;if(classname.IndexOf("AmmoBox_")==0)buy=180;
    config.Items.Insert(new HTCatalogItem(classname,buy,Math.Floor(buy*0.25)));
   }
  }
  array<string> tools={"CanOpener","Pliers","Lockpick","WeaponCleaningKit","SewingKit","LeatherSewingKit","Shovel","FieldShovel","Crowbar","FireExtinguisher","Hammer","MeatTenderizer","Wrench","LugWrench","Pipe","Screwdriver","Sickle","Hacksaw","KitchenKnife","SteakKnife","HayHook","StoneKnife","BoneKnife","FarmingHoe","FishingRod","ImprovisedFishingRod","SledgeHammer","Iceaxe","Broom","Broom_Birch","Paddle","ElectronicRepairKit","EpoxyPutty","Whetstone","HandSaw","Blowtorch","Hatchet","WoodAxe","FirefighterAxe","Pickaxe","HuntingKnife","CombatKnife","FangeKnife","Machete","OrientalMachete","KukriKnife","Cleaver","CrudeMachete","CrudeKnife","SharpWoodenStick","Flashlight","Headtorch_Black","Headtorch_Grey","Matchbox","PetrolLighter","HandDrillKit","Compass","OrienteeringCompass","Rangefinder","Binoculars","GPSReceiver","CableReel","BatteryCharger","SparkPlug","GlowPlug","CarBattery","TruckBattery","PortableGasLamp","PortableGasStove","DuctTape","CodeLock","CombinationLock","CombinationLock4"};
  foreach(string type:tools)
  {
   if(!GetGame().ConfigIsExisting("CfgVehicles "+type) || GetGame().ConfigGetInt("CfgVehicles "+type+" scope")!=2)continue;
   config.Items.Insert(new HTCatalogItem(type,150,35));
  }
 }
 static string QuantityLabel(ItemBase item)
 {
  Magazine mag=Magazine.Cast(item);
  if(mag)return mag.GetAmmoCount().ToString()+"/"+mag.GetAmmoMax().ToString()+" rounds";
  if(item.HasQuantity())return Math.Round(item.GetQuantity()).ToString()+"/"+Math.Round(item.GetQuantityMax()).ToString()+" quantity";
  return Math.Round(item.GetHealth01("","Health")*100).ToString()+"% condition";
 }
 static string PlayerId(PlayerBase player)
 {
  if(!player || !player.GetIdentity())return "";
  return player.GetIdentity().GetId();
 }
 static HTWallet Wallet(string id)
 {
  if(id=="")return null;
  HTWallet wallet;if(Wallets.Find(id,wallet))return wallet;
  string path=DIRECTORY+"/TraderWallets/"+id+".json";string error;
  if(FileExist(path))
  {
   if(!JsonFileLoader<HTWallet>.LoadFile(path,wallet,error) || !wallet || wallet.PlayerId!=id)
   {Print("[HumanTraders] Wallet invalid; trading disabled for "+id);return null;}
  }
  else {wallet=new HTWallet;wallet.PlayerId=id;}
  wallet.Money=Math.Clamp(wallet.Money,0,MAX_MONEY);Wallets.Insert(id,wallet);return wallet;
 }
 static bool Save(HTWallet wallet)
 {
  if(!wallet)return false;
  string path=DIRECTORY+"/TraderWallets/"+wallet.PlayerId+".json";
  // Stage serialization first, keeping a recovery copy of the previous balance.
  string error;if(!JsonFileLoader<HTWallet>.SaveFile(path+".tmp",wallet,error))return false;
  if(FileExist(path) && !CopyFile(path,path+".bak"))return false;
  if(!CopyFile(path+".tmp",path))return false;
  DeleteFile(path+".tmp");return true;
 }
 static HTCatalogItem Entry(string classname)
 {
  string wanted=classname;wanted.ToLower();
  foreach(HTCatalogItem entry:Load().Items){string key=entry.ClassName;key.ToLower();if(key==wanted)return entry;}
  return null;
 }
 static bool Empty(ItemBase item)
 {
  if(!item || item.IsRuined() || item.IsSetForDeletion() || item.GetInventory().AttachmentCount()>0)return false;
  CargoBase cargo=item.GetInventory().GetCargo();if(cargo && cargo.GetItemCount()>0)return false;
  Weapon_Base weapon=Weapon_Base.Cast(item);
  if(weapon)
  {
   for(int muzzle=0;muzzle<weapon.GetMuzzleCount();muzzle++)
    if(!weapon.IsChamberEmpty(muzzle) || weapon.GetInternalMagazineCartridgeCount(muzzle)>0)return false;
  }
  return true;
 }
 static int SalePrice(ItemBase item,HTCatalogItem entry)
 {
  if(!entry || entry.SellPrice<0 || !Empty(item))return -1;
  float fraction=Math.Clamp(item.GetHealth01("","Health"),0,1);
  Magazine magazine=Magazine.Cast(item);
  if(magazine && magazine.GetAmmoMax()>0)fraction=fraction*magazine.GetAmmoCount()/magazine.GetAmmoMax();
  else if(item.HasQuantity() && item.GetQuantityMax()>0)fraction=fraction*Math.Clamp(item.GetQuantity()/item.GetQuantityMax(),0,1);
  return Math.Floor(entry.SellPrice*fraction);
 }
 static string Buy(PlayerBase player,int index,HTWallet wallet)
 {
  if(!wallet)return "Your wallet could not be loaded. Contact the server administrator.";
  if(index<0 || index>=Load().Items.Count())return "This item is unavailable.";
  HTCatalogItem entry=Config.Items[index];if(entry.BuyPrice<0)return "This item is not for sale.";
  if(wallet.Money<entry.BuyPrice)return "You don't have enough money.";
  // Create directly in cargo first. A server inventory move can be queued,
  // which makes an immediate post-purchase snapshot miss the delivered item.
  ItemBase item=ItemBase.Cast(player.GetInventory().CreateInInventory(entry.ClassName));
  bool onGround=!item;
  if(onGround)item=ItemBase.Cast(GetGame().CreateObjectEx(entry.ClassName,player.GetPosition()+"0 0.1 0",ECE_CREATEPHYSICS|ECE_NOLIFETIME));
  if(!item)return "The item could not be created. No money was spent.";
  item.SetHealthMax("","Health");Magazine mag=Magazine.Cast(item);
  if(mag)
  {
   int rounds=mag.GetAmmoMax();if(entry.BuyQuantity>=0)rounds=Math.Clamp(entry.BuyQuantity,1,mag.GetAmmoMax());
   mag.ServerSetAmmoCount(rounds);
  }
  else if(item.HasQuantity())
  {
   float quantity=item.GetQuantityMax();if(entry.BuyQuantity>=0)quantity=Math.Clamp(entry.BuyQuantity,item.GetQuantityMin()+0.01,item.GetQuantityMax());
   item.SetQuantity(quantity);
  }
  wallet.Money=wallet.Money-entry.BuyPrice;
  if(!Save(wallet)){wallet.Money=wallet.Money+entry.BuyPrice;GetGame().ObjectDelete(item);return "Wallet save failed. Purchase cancelled.";}
  if(!onGround)return "Purchased "+item.GetDisplayName()+".";
  item.PlaceOnSurface();item.SetLifetime(3600);item.SetLifetimeMax(3600);
  return "Inventory full. Your purchase is on the ground at your feet.";
 }
 static string Sell(PlayerBase player,ItemBase item,HTWallet wallet,int expectedPrice=-1)
 {
  if(!wallet)return "Your wallet could not be loaded. Contact the server administrator.";
  for(int i=Sold.Count() - 1;i>=0;i--)if(!Sold[i])Sold.Remove(i);
  if(!item || item.GetHierarchyRootPlayer()!=player || Sold.Find(item)>=0)return "That item is no longer in your inventory.";
  int price=SalePrice(item,Entry(item.GetType()));
  if(price<0)return "Not accepted. Empty cargo, remove attachments and unload weapons first; ruined items cannot be sold.";
  if(expectedPrice>=0 && price!=expectedPrice)return "Sale value changed. Check the refreshed price and sell again.";
  if(price==0)return "That item's remaining condition or quantity has no sale value.";
  if(wallet.Money>MAX_MONEY-price)return "Wallet limit reached.";
  int balanceBefore=wallet.Money;
  Sold.Insert(item);wallet.Money=balanceBefore+price;
  if(!Save(wallet)){wallet.Money=wallet.Money-price;Sold.RemoveItem(item);return "Wallet save failed. Sale cancelled.";}
  string name=item.GetDisplayName();GetGame().ObjectDelete(item);
  Print("[HumanTraders] SELL "+name+" credit="+price.ToString()+" before="+balanceBefore.ToString()+" after="+wallet.Money.ToString());
  return "Sold "+name+" | +$"+price.ToString()+" | Balance: $"+balanceBefore.ToString()+" -> $"+wallet.Money.ToString();
 }
 static HTReply Snapshot(PlayerBase player,HumanTraderF_Journalist trader,string message="")
 {
  HTReply reply=new HTReply;trader.GetNetworkID(reply.TraderLow,reply.TraderHigh);reply.Token=trader.HTToken;reply.Name=Load().TraderName;reply.Message=message;
  HTWallet wallet=Wallet(PlayerId(player));if(wallet)reply.Money=wallet.Money;
  for(int i=0;i<Load().Items.Count();i++)
  {
   HTCatalogItem entry=Config.Items[i];if(entry.BuyPrice<0)continue;
   HTTradeRow row=new HTTradeRow;row.ClassName=entry.ClassName;row.Index=i;row.Price=entry.BuyPrice;
   row.Detail="Full quantity";if(entry.BuyQuantity>=0)row.Detail="Quantity "+entry.BuyQuantity.ToString();
   row.Name=GetGame().ConfigGetTextOut(ConfigPath(entry.ClassName)+" displayName");if(row.Name=="")row.Name=entry.ClassName;
   reply.Stock.Insert(row);
  }
  array<EntityAI> inventory={};player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER,inventory);
  foreach(EntityAI entity:inventory)
  {
   ItemBase item=ItemBase.Cast(entity);if(!item || item.IsSetForDeletion() || Sold.Find(item)>=0)continue;
   HTTradeRow inventoryRow=new HTTradeRow;inventoryRow.Name=item.GetDisplayName();inventoryRow.ClassName=item.GetType();item.GetNetworkID(inventoryRow.NetLow,inventoryRow.NetHigh);
   inventoryRow.Detail=QuantityLabel(item);
   inventoryRow.Price=SalePrice(item,Entry(item.GetType()));
   if(inventoryRow.Price<0)inventoryRow.Detail="Not accepted / empty contents first";
   reply.Inventory.Insert(inventoryRow);
  }
  return reply;
 }
}
modded class PlayerBase
{
 float m_HTRequestAt;
 override void OnRPC(PlayerIdentity sender,int rpc_type,ParamsReadContext ctx)
 {
  super.OnRPC(sender,rpc_type,ctx);
  if(rpc_type==HTClient.REPLY_RPC && !GetGame().IsServer())
  {
   Param1<ref HTReply> response;if(ctx.Read(response)){HTClient.Reply=response.param1;HTClient.Ready=true;}return;
  }
  if(rpc_type!=HTClient.REQUEST_RPC || !GetGame().IsServer() || !sender || !GetIdentity() || sender.GetId()!=GetIdentity().GetId())return;
  Param1<ref HTRequest> packet;if(!ctx.Read(packet) || !packet.param1)return;
  HTRequest request=packet.param1;
  float now=GetGame().GetTime()*0.001;if(request.Operation!=4 && now<m_HTRequestAt)return;m_HTRequestAt=now+0.15;
  HumanTraderF_Journalist trader=HumanTraderF_Journalist.Cast(GetGame().GetObjectByNetworkId(request.TraderLow,request.TraderHigh));
  if(trader)trader.Request(this,request);
 }
}

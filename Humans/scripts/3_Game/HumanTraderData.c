class HTLocation
{
 vector Position;
 float Orientation;
 void HTLocation(vector p="4505 0 10200",float o=0){Position=p;Orientation=o;}
}
class HTCatalogItem
{
 string ClassName;
 int BuyPrice;
 int SellPrice;
 float BuyQuantity=-1;
 void HTCatalogItem(string c="",int b=100,int s=25){ClassName=c;BuyPrice=b;SellPrice=s;}
}
class HumanTradersConfig
{
 int traderspawn=1;
 int traderrandomlocations=0;
 int RespawnMinutes=30;
 float WanderRadius=12;
 float DefenseRadius=30;
 float GreetingMinDistance=3;
 float GreetingMaxDistance=8;
 float GreetingCooldownSeconds=60;
 float HearingRange=20;
 string TraderName="Strange Trader";
 ref array<ref HTLocation> Locations={};
 ref array<ref HTCatalogItem> Items={};
}
class HTWallet
{
 int Version=1;
 string PlayerId;
 int Money;
}
class HTTradeRow
{
 string Name;
 string ClassName;
 int Price;
 int Index;
 int NetLow;
 int NetHigh;
 string Detail;
}
class HTReply
{
 int TraderLow;
 int TraderHigh;
 int Token;
 int Money;
 bool Closed;
 string Name;
 string Message;
 ref array<ref HTTradeRow> Stock={};
 ref array<ref HTTradeRow> Inventory={};
}
class HTRequest
{
 int TraderLow;
 int TraderHigh;
 int Token;
 int Sequence;
 int Operation; // 0 open, 1 buy, 2 sell, 3 refresh/heartbeat, 4 close
 int Index;
 int ItemLow;
 int ItemHigh;
 int ExpectedSalePrice=-1;
}
class HTClient
{
 static const int REQUEST_RPC=874160;
 static const int REPLY_RPC=874161;
 static const int VOICE_RPC=874162;
 static ref HTReply Reply;
 static bool Ready;
}

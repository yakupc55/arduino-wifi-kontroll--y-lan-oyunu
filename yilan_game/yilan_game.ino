#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <WiFiUDP.h>
#define PIN_SCE   D3
#define PIN_RESET D4
#define PIN_DC    D1
#define PIN_SDIN  D0
#define PIN_SCLK  D2
#define yukari 0xFF18E7
#define asagi 0xFF4AB5
#define sol 0xFF10EF
#define sag 0xFF5AA5
#define orta 0xFF38C7
#define LCD_C     LOW
#define LCD_D     HIGH
// burasý yazý için yazýda y max 6 oluyor
#define LCD_X     84
#define LCD_Y     6
#include "font.h"
#include <LiquidCrystal.h>
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// wifi connection variables
const char* ssid = "Game-wifi";
const char* password = "yilangame";
boolean wifiConnected = false;

int gelenWifiDegeri=-2;
// UDP variables
unsigned int localPort = 8888;
WiFiUDP UDP;
boolean udpConnected = false;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "basarýlý"; // a string to send back

int yemek=0;
int sayac=0;
float dengeleyici=0.0;
int t1=0,t2=0; //Creates variables t1 and t2 and gives them a value of 0
int hits=0;
int rps=0;
//byte pixels[LCD_X][LCD_Y];
unsigned long lastRefreshTime=0;
const int refreshInterval = 150;
byte gameState = 1;
byte ballSpeed = 2;
int player1WinCount = 0;
int player2WinCount = 0;
byte hitCount = 0;
 unsigned char tumalan[84][6]; // bu tüm alaný belirten bir kod
int yilanKuyruk[880];
 int yilanUzunluk=0;  
int yilanX=1;
int yilanY=4;
int yilanMaxX=40;
int yilanMaxY=22;
unsigned char hareketTipi=0; // 5 tane yönler ve stop 
bool oyunDurumu=true;
bool oyunTekrar=false;
float hiz=2.0;
bool ekle=true;
uint16_t RECV_PIN = D5;
IRrecv irrecv(RECV_PIN);
decode_results results;
bool stopDurumu=true;

void setup(){
Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
udpConnected = connectUDP();
if (udpConnected){
// initialise pins
pinMode(5,OUTPUT);
}

  irrecv.enableIRIn();
  temizle();
    LcdInitialise();
  Serial.begin(9600);
ilkDegerler(); 
}

void loop() {
  if(udpConnected){

// if there’s data available, read a packet
int packetSize = UDP.parsePacket();
if(packetSize)
{
Serial.println("");
Serial.print("Received packet of size ");
Serial.println(packetSize);
Serial.print("From ");
IPAddress remote = UDP.remoteIP();
for (int i =0; i < 4; i++)
{
Serial.print(remote[i], DEC);
if (i < 3)
{
Serial.print(".");
}
}
Serial.print(", port ");
Serial.println(UDP.remotePort());

// read the packet into packetBufffer
UDP.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
Serial.println("Contents:");
String value = packetBuffer;
Serial.println(value);
int gelenWifiDegeri=value.toInt();
if(gelenWifiDegeri>=0 and gelenWifiDegeri<5)
{

  switch (gelenWifiDegeri)
    {
      case 2: if(hareketTipi!=3)hareketTipi=2; break;
      case 3:if(hareketTipi!=2)hareketTipi=3; break;
      case 1: if(hareketTipi!=0)hareketTipi=1;break;
      case 0:if(hareketTipi!=1)hareketTipi=0; break;
      case 4: oyunTekrar=true; break;
    }
    stopDurumu=true;
}
if(gelenWifiDegeri==-1){
stopDurumu=false;
}
}
// send a reply, to the IP address and port that sent us the packet we received
UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
UDP.write(ReplyBuffer);
UDP.endPacket();
delay(10);
// turn LED on or off depending on value recieved
//digitalWrite(5,value);
}
//delay(10);


  if (irrecv.decode(&results))
  {

    /* 0: sað
     * 1: sol
     * 2: yukari
     * 3: assagi
     */
    switch (results.value)
    {
      case yukari: if(hareketTipi!=3)hareketTipi=2; break;
      case asagi:if(hareketTipi!=2)hareketTipi=3; break;
      case sol: if(hareketTipi!=0)hareketTipi=1;break;
      case sag:if(hareketTipi!=1)hareketTipi=0; break;
      case orta: oyunTekrar=true; break;
    }
  
    Serial.println(hareketTipi);
     serialPrintUint64(results.value, HEX);
       irrecv.resume();
       delay(20);
    }// kýzýlötesi ifi

 dengeleyici+=hiz;
     delay(2);
     if(dengeleyici>=100.0){
      dengeleyici=0.0;
if(oyunDurumu && stopDurumu){ 
 // temizle();
   alanTemizle();
   hareketHesapla();
int hareketimiz=kordinatTohareket(yilanX,yilanY);
yilanHareket(hareketimiz);
//yilanKuyruk[yilanUzunluk-1]=hareketimiz;
yilanKuyrukIsle();   
yemekEkle();
//kareEkle2(sayac,3);

//hesaplaVeYap();

//sayac++;
// Serial.println(yilanUzunluk);
 sahneEkle();
 sahneIsle();
 hits++;
}


else{
    temizle();
 LcdXY(0,3);
  LcdWriteString("Game Over");
  for(int i=1;i<5;i++){
    
  LcdWrite(LCD_C, 0x0C);  // LCD in inverse mode.
  delay(300);
  LcdWrite(LCD_C, 0x0D);
  delay(300);
  }
  
}
 t2 = millis();
  if(t2 >= (t1 + 1000)){
    rps = hits;
    hits = 0;
    t1=t2;
  //  Serial.println("Fps : "+ String(rps));
  }
  if(oyunTekrar){
    temizle();
ilkDegerler();
Serial.println("caliþti");
    oyunTekrar=false;
  }
    }// dengeleyici sonu


}// son kýsým


int kordinatTohareket(int x,int y){
return (y*yilanMaxX)+x;
}
void carpismaKontrol(int hareket){
for(int i=0;i<yilanUzunluk;i++){
  if(hareket==yilanKuyruk[i]){
    oyunDurumu=false; 
    break;
  }
}
}// fonksiyon sonu
void alanTemizle(){
for(int i=0;i<84;i++){

  for(int j=0;j<6;j++){
    tumalan[i][j]=0;
  }
}
}// fonksiyon sonu
void sahneIsle(){
  for(int i=0;i<84;i++){

  for(int j=0;j<6;j++){
      gotoXY (i,j);
    LcdWrite (1,tumalan[i][j]);
  }
  }
}// fonksiyon sonu
void LcdInitialise(void){
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  delay(200);
  digitalWrite(PIN_RESET, LOW);
  delay(500);
  digitalWrite(PIN_RESET, HIGH);
  LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite(LCD_C, 0xB8 );  // Set LCD Vop (Contrast).
  LcdWrite(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
  LcdWrite(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_C, 0x0C );  // LCD in normal mode.
  LcdWrite(LCD_C, 0x20 );
  LcdWrite(LCD_C, 0x80 ); //select X Address 0 of the LCD Ram
  LcdWrite(LCD_C, 0x40 ); //select Y Address 0 of the LCD Ram - Reset is not working for some reason, so I had to set these addresses
  LcdWrite(LCD_C, 0x0C );
}
void temizle(){
for(int i=0; i<504; i++) LcdWrite(LCD_D,0x00);
}

void gotoXY(int x, int y)
{
  LcdWrite( 0, 0x80 | x);  // Column.
  LcdWrite( 0, 0x40 | y);  // Row.  
}

void sahneEkle(){
  // kare þeklindeki sahnemizi ekliyoruz
for(int i=2;i<82;i++){
tumalan[i][0]+=3;
}
for(int i=2;i<82;i++){
tumalan[i][5]+=192;
}
for(int i=0;i<6;i++){
tumalan[0][i]=255;
tumalan[1][i]=255;
}
for(int i=0;i<6;i++){
tumalan[82][i]=255;
tumalan[83][i]=255;
}
  
}// fonksiyon sonu
void ilkDegerler(){
  hareketTipi=0;
 // yeniYemek(); 
  LcdWrite(LCD_C, 0x0C);
oyunDurumu=true;
yilanUzunluk=5;
  yilanKuyruk[0]=42;
  yilanKuyruk[1]=43;
  yilanKuyruk[2]=44;
  yilanKuyruk[3]=45;
  yilanKuyruk[4]=46;
  yilanX=6;
  yilanY=1;

yeniYemek();
}
void LcdWriteCharacter(char character)
{   for(int i=0; i<5; i++) LcdWrite(LCD_D,ASCII[character - 0x20][i]);
LcdWrite(LCD_D,0x00);
}
void LcdWriteString(char* characters)
{   while(*characters) LcdWriteCharacter(*characters++);
}
void LcdXY(int x, int y)
{
  LcdWrite(LCD_C,0x80 | x);  // Column.
  LcdWrite(LCD_C,0x40 | y);  // Row.
}
void LcdWrite(byte dc, byte data){
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}
void kareEkle2(int x,int y){
  x+=1;
  y+=1;
int y6 = y/4;  // bölmemizin sebebi lcdde y 6 parça olmasý 8*6 =48
int h= (y-(y6*4))%4;  // bölmememizin sebebi y ler 4 parça ayrýldýðý için
tumalan[x*2][y6]+=dondur(h);
tumalan[(x*2)+1][y6]+=dondur(h);

//Serial.println("y6 = " + String(y6)+" h= "+ String(h) + " X= " + String(x) + " Y= " +String(y));
//Serial.println("alan 1 = " + String(tumalan[x*2][y6])+"alan 2 = " + String(tumalan[(x*2)+1][y6]));
}
void yilanHareket(int hareketNoktasi){ 
  if(ekle){

yeniKuyruk(hareketNoktasi);
  }
  else{
for(int i=0;i<yilanUzunluk-1;i++){
yilanKuyruk[i]=yilanKuyruk[i+1];
}
yilanKuyruk[yilanUzunluk-1]=hareketNoktasi;
  }// else sonu
}//fonksiyon sonu


void yilanKuyrukIsle(){
  int hx=0;
  int hy=0;
  int deger=0;
for(int i=0;i<yilanUzunluk;i++){
deger=yilanKuyruk[i];
//Serial.println(" kuyruk i0 = " + String(deger));
hy=deger/yilanMaxX;
hx=deger-(hy*yilanMaxX);
//Serial.println(" hx = " + String(hx)+" hy = " + String(hy));
kareEkle2(hx,hy);
}
}// fonksiyon tipi


void yutmaKontrol(int hareketNoktasi){
  if(hareketNoktasi==yemek){
//    yeniKuyruk(hareketNoktasi);  
ekle=true;
    yeniYemek();  
    }
  
}

int hareketHesapla(){
switch(hareketTipi){
case 0: yilanX++;if(yilanX==yilanMaxX) oyunDurumu=false; break;  // saða hareket
case 1: yilanX--;if(yilanX==-1) oyunDurumu=false; break;  // sola hareket
case 2: yilanY--;if(yilanY==-1) oyunDurumu=false;break;  // yukari hareket
case 3: yilanY++;if(yilanY==yilanMaxY) oyunDurumu=false;break;  // assagi hareket
}// switch sonu
int hareketimiz2= kordinatTohareket(yilanX,yilanY);
carpismaKontrol(hareketimiz2); // çarpýþma kontrol
yutmaKontrol(hareketimiz2);
}

unsigned char dondur(unsigned char gelen){
  unsigned char s=0;
switch(gelen){
case 0:s=3; break;   // 1 + 3 = 4 ilk iki bayt hýgh deðeri
case 1:s=12; break; // 4 + 8 = 12  2 ve 3 bayt hýgh deðeri
case 2:s=48; break; // 16 + 32 = 48 4 ve 5 bayt hýgh deðeri
case 3:s=192; break; // 64 + 128 = 192 6 ve 7 bayt hýgh deðeri
}// switch sonu
return s;
}
void hesaplaVeYap(){
switch(hareketTipi){
case 0: yilanX++;if(yilanX==yilanMaxX) oyunDurumu=false; break;  // saða hareket
case 1: yilanX--;if(yilanX==-1) oyunDurumu=false; break;  // sola hareket
case 2: yilanY--;if(yilanY==-1) oyunDurumu=false;break;  // yukari hareket
case 3: yilanY++;if(yilanY==yilanMaxY) oyunDurumu=false;break;  // assagi hareket
}// switch sonu
//Serial.println(yilanUzunluk);
int hareketimiz2= kordinatTohareket(yilanX,yilanY);
carpismaKontrol(hareketimiz2); // çarpýþma kontrol
for(int i=0;i<yilanUzunluk-1;i++){
yilanKuyruk[i]=yilanKuyruk[i+1];
}
yilanKuyruk[yilanUzunluk-1]=hareketimiz2;
for(int i=0;i<yilanUzunluk;i++){
int deger=yilanKuyruk[i];
int hy=deger/yilanMaxX;
int hx=deger-(hy*yilanMaxX);
kareEkle2(hx,hy);
}
}


void yeniYemek(){
  Serial.println("yenide");
bool saglama=true;
while(saglama){
int nokta=kordinatTohareket(random(yilanMaxX),random(yilanMaxY));
for(int i=0;i<yilanUzunluk;i++){
  if(nokta==yilanKuyruk[i]){
saglama=true; 
    break;
  }
  else{
    saglama=false;
  }
}
yemek=nokta;
}//while sonu

Serial.println(yemek);

}






void yeniKuyruk(int hareketNoktasi){
yilanUzunluk++;
yilanKuyruk[yilanUzunluk-1]=hareketNoktasi;
ekle=false;
}// fonksiyon sonu


void yemekEkle(){
int hy=yemek/yilanMaxX;
int hx=yemek-(hy*yilanMaxX);
kareEkle2(hx,hy);


}
boolean connectUDP(){
boolean state = false;

Serial.println("");
Serial.println("Connecting to UDP");

if(UDP.begin(localPort) == 1){
Serial.println("Connection successful");
state = true;
}
else{
Serial.println("Connection failed");
}

return state;
}
// connect to wifi – returns true if successful or false if not
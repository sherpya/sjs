loadplugin("curl");
loadplugin("korax");

include("scripts/progress_fun.js");
verbose(1);

datfile = "/usr/lib/uvscan/scan.dat";
offset = 0x45;

localver = getdword(datfile, offset);

tempini = gettemp() + "update.ini";

unlink(tempini);

download("ftp://ftp.nai.com/CommonUpdater/update.ini", tempini);

ini = new parser();
ini.loadini(tempini);
ver = ini.getintvalue("ZIP/DATVersion");

print("Local version is: " + localver + " remote " + ver);

if (ver > localver)
   print("Update needed");
else
   print("Antivirus is uptodate");

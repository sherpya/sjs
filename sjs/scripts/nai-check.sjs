loadplugin("curl");
loadplugin("korax");

include("progress_fun.js");
verbose(1);

base = "ftp://ftp.nai.com/CommonUpdater"

datfile = "/usr/lib/uvscan/scan.dat";
offset = 0x45;

localver = getdword(datfile, offset);

tempini = gettemp() + "update.ini";

unlink(tempini);

download(base + "/update.ini", tempini);

ini = new parser();
ini.loadini(tempini);
ver = ini.getintvalue("ZIP/DATVersion");

print("Local version is: " + localver + " remote " + ver);

if (ver > localver)
   print("Update needed - " + base + ini.getvalue("ZIP/FilePath") + ini.getvalue("ZIP/FileName"));
else
   print("Antivirus is uptodate");

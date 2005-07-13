verbose(1);
loadplugin("korax");

tempini = gettemp() + "update.ini";

ini = new parser();

ini.loadini(tempini);

ver = ini.getintvalue("ZIP/DATVersion");

print("Version is " + ver);

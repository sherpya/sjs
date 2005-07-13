verbose(1);
loadplugin("korax");

tempini = gettemp() + "/" + "update.ini";

ini = new inifile(tempini);

ver = ini.getintvalue("ZIP/DATVersion");

print("Version is " + ver);

verbose(1);
loadplugin("korax");

tempini = gettemp() + "update.ini";
tempxml = gettemp() + "update.xml";

ini = new parser();

ini.loadini(tempini);

ver = ini.getintvalue("ZIP/DATVersion");

print("Version is " + ver);

ini.savexml(tempxml);

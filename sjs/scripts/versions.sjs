loadplugin("curl");
loadplugin("korax");
loadplugin("zlib");
loadplugin("registry");

//print(version.js);
//print(version.sjs);

//print(version.curl);
//print(version.korax);
//print(version.zlib);

for (i in version)
   print(i + " = " + version[i]);

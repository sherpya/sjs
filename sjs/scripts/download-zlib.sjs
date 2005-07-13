loadplugin("curl");
loadplugin("zlib");

include("scripts/progress_fun.js");

verbose(1);
mkdir("test");
res = download("http://www.zlib.net/zlib122.zip", "test/zlib122.zip");
print('Dowload res = ' + res);
verbose(0);
res = unzip("test/zlib122.zip", "test/out");
print('Unzip res = ' + res);

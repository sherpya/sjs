loadplugin("curl");

include("scripts/progress_fun.js");

verbose(1);
mkdir("test");
download("http://www.zlib.net/zlib122.zip", "test/zlib122.zip");

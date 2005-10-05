loadplugin("curl");

include("progress_fun.sjs");

verbose(1);
mkdir("test");
download("http://www.zlib.net/zlib122.zip", "test/zlib122.zip");

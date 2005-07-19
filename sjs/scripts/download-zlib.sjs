loadplugin("curl");
loadplugin("zlib");

include("scripts/progress_fun.js");

verbose(1);
mkdir("test");
res = download("http://www.zlib.net/zlib122.zip", "test/zlib122.zip");
print('Dowload res = ' + res);
verbose(0);

zipfile = new zip("test/zlib122.zip");

do
{
    info = zipfile.getfileinfo();
    print(info.filename + " - " + info.compressed_size + "/" + info.uncompressed_size);
} while (zipfile.gotonextfile());

res = zipfile.unzipto("test/out");
print('Unzip res = ' + res);

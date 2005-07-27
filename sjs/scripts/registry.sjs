loadplugin("registry");

print(version.registry);

reg = new registry();
print(reg.openkey(HKLM, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\Folder"));

while (value = reg.enumkey())
    print("Value is " + value);

reg.firstkey();
while (value = reg.enumkey())
    print("Value is " + value);

loadplugin("korax");

str = "abcdefghijklmnopqrstuvxyz";

text = '';

for (i = 0; i < 10; i++)
  text = text + str[random(0, 24)];

print("your password is " + text);

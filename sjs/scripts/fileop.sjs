f = new file("hello_from_js.txt", "w");
print(f.write('hello js!'));
f.close();

f = new file("hello_from_js.txt", "r");
print(f.read());
f.close()
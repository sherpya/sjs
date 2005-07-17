
args = scriptargs();

print("Script base: " + basepath());
for (i = 0; i < args.length; i++)
    print("Arg n." + (i+1) + ": " + args[i]);
function my_progress(dltotal, dlnow, ultotal, ulnow)
{
   perc = dlnow * 100.0 / dltotal;
   print("Perc = " + Math.round(perc) + "%");
   return 0;
}

setprogressfunction(my_progress);

chars=('a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o')
for char in "${chars[@]}";
do
  filename="test_list_";
  filename+=$char;
  outname=$filename;
  filename+=".txt";
  outname+=".out";
  touch test_lists/$filename;
  touch test_outs/$outname;
  > test_lists/$filename;
  > test_outs/$outname;
  string="";
  for i in $(seq 1 15); do
    string+="face";
    string+=$char;
    string+=$i;
    string+=".pgm "
  done;
  echo $string > test_lists/$filename;
  ./LTP test_lists/$filename &> test_outs/$outname&
done;

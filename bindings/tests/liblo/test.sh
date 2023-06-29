out="$(
oscdump 5959 & dumpjob=$#
oscsend localhost 5959 /Test_Component_1/button_out i 1   & testjob=$#
oscsend localhost 5959 /Test_Component_1/button_in i 1
kill $dumpjob
kill $testjob
)"
echo $out
out="$(echo $out | sed 's/$(\w|\d)+\.(\w|\d)+ //g')"
echo $out
[ "$out" == "/Test_Component_1/button_out i 1" ]

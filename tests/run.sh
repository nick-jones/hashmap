#/bin/bash

status=0

for f in tests/check_* ; do                                                                                                                                   
  if [ -x $f ] ; then                                                                                                                                     
    valgrind --quiet --error-exitcode=1 --tool=memcheck --leak-check=full $f
    [ "$?" -eq "0" ] || status=1 
  fi                                                                                                                                                         
done

exit $status

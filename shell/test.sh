#!/bin/bash
# It is just a script for testing shell function.

echo "The date is:"
date

echo "Let's see who is using the system:"
who

echo UID:$UID
echo HOME:$HOME

echo -e "\n*******if-then-fi********"


echo -e "\n*******para**********"
echo "\$0:$0"
echo "\$#:$#"
echo "\$@:$@"
echo "PID:$$"

enable_read_func=0
if [ $enable_read_func -eq 1 ]
then
echo -e "\n*******read**********"
echo -n "Please enter:"
read input_info
echo "Welcome $input_info"
fi


echo -e "\n*******function**********"
function func1 {
    echo "Now running func1."
}
func1


echo -e "ininin"
echo -e "info"
echo -e "input"
echo -e "unfo"
echo -e "onfo"
echo -e "anfo"






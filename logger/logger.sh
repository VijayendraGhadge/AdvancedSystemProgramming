#!/bin/bash
while getopts ":haude" opt; do
case $opt in
h)
help=1
;;
a)
author=1
;;
u)
unique=1
;;
d)
date=1
;;
e)
error=1
;;
\?)
echo "Invalid option: -$OPTARG" >&2
echo "Try './logger.sh -h' for more information."
exit 0
;;
esac
shift $(($OPTIND - 1))
done

if [[ $help -eq 1 ]] && [[ $author -eq 1 ]] ; then
echo "Author : Vijayendra Ghadge"
echo "Usage: ./logger.sh [-adehu]... [FILE]..."
echo "Excutes the file specified and redirects the output to a file."
echo "  -a           Display name of author."
echo "  -d           Include date and time in the log file name."
echo "  -e           Include error output for the command in the log file."
echo "  -h           Display the help (usage)."
echo "  -u           Give unique file name to log file."
exit 1
fi

if [[ $author -eq 1 ]] ; then
echo " Author : Vijayendra Ghadge"
exit
fi

if [[ $help -eq 1 ]] ; then
echo "Usage: ./logger.sh [-adehu]... [FILE]..."
echo "Excutes the file specified and redirects the output to a file."
echo "  -a           Display name of author."
echo "  -d           Include date and time in the log file name."
echo "  -e           Include error output for the command in the log file."
echo "  -h           Display the help (usage)."
echo "  -u           Give unique file name to log file."
exit
fi

if [[ $# -lt 1 ]] ; then
echo "No arguments provided, this command takes one argument"
echo "Usage: ./logger.sh [-adehu]... [FILE]..."
exit 1
fi

file_to_exec=$(echo "$1" | awk '{print $1}')

if [[ ! -f $file_to_exec ]]; then
echo "$file_to_exec not found"
exit 1
fi

if [[ ! -x $file_to_exec ]]; then
echo "$file_to_exec not executable or permission denied"
exit 1
fi

if [[ $date -eq 1 ]] && [[ $unique -eq 1 ]] ; then
rand=$(cat /dev/urandom | tr -cd 'a-zA-Z0-9' | head -c 10)
d=$(date +"%Y%m%dT%H%M%SZ")
name=logger.$rand.$d.log
if [[ $error -eq 1 ]] ; then
$1 >& $name
else
$1 > $name
fi
echo "Log file $name created"
exit
fi

if [[ $date -eq 1 ]] ; then
d=$(date +"%Y%m%dT%H%M%SZ")
name=logger.$d.log
if [[ $error -eq 1 ]] ; then
$1 >& $name
else
$1 > $name
fi
echo "Log file $name created"
exit
fi

if [[ $unique -eq 1 ]] ; then
rand=$(cat /dev/urandom | tr -cd 'a-zA-Z0-9' | head -c 10)
name=logger.$rand.log
if [[ $error -eq 1 ]] ; then
$1 >& $name
else
$1 > $name
fi
echo "Log file $name created"
exit
fi

if [[ $error -eq 1 ]] ; then
$1 >& logger.log
else
$1 > logger.log
fi 
echo "Log file logger.log created"

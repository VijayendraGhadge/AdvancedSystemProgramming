#!/bin/bash
#intitializing options to default values.
help=0
command=0
executable=0
status=0
file=0

function display_help {
echo "Usage: ./process.sh [-cefhs]... [PID]..."
echo "Displays specific information about a process."
echo "  -c           Display commnd used to run the process."
echo "  -e           Display the executable used to run the process."
echo "  -f           Display processes given open file."
echo "  -h           Display the help (usage)."
echo "  -s [filter param]          Display status information of the process."
exit 1
}


while getopts ":hces:f" opt; do
case $opt in
h)
help=1
;;
c)
command=1
;;
e)
executable=1
;;
s)
status=1
s_arg=$OPTARG
;;
f)
file=1
;;
:)
echo "Option -s requires an argument [filter parameter]"
;;
\?)
echo "Invalid option: -$OPTARG" >&2
echo "Try './process.sh -h' for more information."
exit 0
;;
esac
done
shift $(($OPTIND - 1))


if [[ $help -eq 1 ]] ; then
display_help
exit
fi

if [[ $# -lt 1 ]] ; then
echo "No arguments provided, this command takes one argument"
echo "Usage: ./process.sh [-adehu]... [PID]..."
exit
fi

if [[ $command -eq 1 ]] && [[ $executable -eq 1 ]] ; then
output=$(cat /proc/$1/cmdline | tr '\000' ' ')
echo "Process $1 was started as $output"
exe_path=$(ls -l /proc/$1/exe | awk '{print $11}')
echo "Process $1 was started by $exe_path"
exit
fi

if [[ $command -eq 1 ]] ; then
output=$(cat /proc/$1/cmdline | tr '\000' ' ')
echo "Process $1 was started as $output"
exit
fi

if [[ $executable -eq 1 ]] ; then
exe_path=$(ls -l /proc/$1/exe | awk '{print $11}')
echo "Process $1 was started by $exe_path"
exit
fi

if [[ $status -eq 1 ]] ; then
stat=$(cat /proc/$1/status | grep "$s_arg")
echo "$stat"
exit
fi

if [[ $file -eq 1 ]] ; then
pid=$(find /proc/*/fd -name '*' -exec ls -l {} \; | grep "$1"  | awk '{print $9}' | grep -Eo "/proc/[0-9]{1,}" | grep -Eo "[0-9]{1,}") 
echo "$pid"
exit
fi

if [[ ! -d /proc/$1 ]] ; then
echo "Process $1 does not exist."
if [[ $help -eq 1 ]]; then
display_help
fi
exit
else 
echo "Process $1 is running."
if [[ $help -eq 1 ]]; then
display_help
fi
exit
fi

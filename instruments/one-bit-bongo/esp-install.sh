echo esp-install.sh
echo $1
echo $2
[ -c "$1" -a -w "$1" ] && $2 flash || echo "No valid ESPPORT ($1), skipping install"

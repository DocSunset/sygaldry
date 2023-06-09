echo $0
[ "$1" = "$ESPPROJ" ] || { echo "Skipping this build ($1) which doesn't match ESPPROJ ($ESPPROJ)" ; exit ; }
shift 1
[ -c "$ESPPORT" -a -w "$ESPPORT" ] && $@ || echo "No valid ESPPORT ($ESPPORT), skipping install"
echo "flashed successfully"

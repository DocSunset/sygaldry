echo $0
echo $ESPPORT
echo $@
[ -c "$ESPPORT" -a -w "$ESPPORT" ] && $@ || echo "No valid ESPPORT ($ESPPORT), skipping install"

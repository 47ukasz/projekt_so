usun_ipc() {
    typ=$1
    flaga=$2

    ipcs -"$typ" | awk -v flaga="$flaga" 'NR>3 {print $2}' | while read -r id; do
        ipcrm "$flaga" "$id"
        
    done
}

usun_ipc "m" "-m" 

usun_ipc "s" "-s" 

# usun_ipc "q" "-q" 

echo "Usuwanie zasobów IPC zakończone."

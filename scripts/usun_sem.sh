SEM_IDS=$(ipcs -s | awk 'NR>3 {print $2}')

if [ -z "$SEM_IDS" ]; then
    echo "Brak zbiorów semaforów do usunięcia."
else
    echo "Usuwanie zbiorów semaforów..."
    for id in $SEM_IDS; do
        echo "Usuwanie semafora o ID: $id"
        ipcrm -s "$id"
    done
    echo "Wszystkie zbiory semaforów zostały usunięte."
fi

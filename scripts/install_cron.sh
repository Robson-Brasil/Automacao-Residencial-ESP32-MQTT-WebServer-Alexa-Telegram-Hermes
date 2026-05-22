#!/usr/bin/env bash
# install_cron.sh – instala cron a partir de arquivos dentro de ~/.hermes/cron
#
# O script lê todos os arquivos de extensão .cron dentro de $HOME/.hermes/cron/
# e garante que cada linha seja incluída na crontab do usuário. Se a linha já
# existir, ela é sobrescrita (removida antes de ser re‑inserida) para evitar
# duplicatas.

set -euo pipefail

CRON_DIR="$HOME/.hermes/cron"

# Garantir que o diretório exista
mkdir -p "$CRON_DIR"

# Salvar crontab atual em uma variável (vazia se não houver nada)
CURRENT=$(crontab -l 2>/dev/null || true)

for manifesto in "$CRON_DIR"/*.cron; do
    # Se não houver nenhum arquivo .cron, pulamos.
    [ -e "$manifesto" ] || continue
    LINE=$(cat "$manifesto")
    # Remove duplicatas (caso já exista)
    if [ -n "$CURRENT" ]; then
        NEW=$(printf "%s\n" "$CURRENT" | grep -v -F "$LINE" || true)
    else
        NEW=""
    fi
    # Adiciona a nova linha (evita linha em branco no início)
    if [ -n "$NEW" ]; then
        NEW=$(printf "%s\n%s\n" "$NEW" "$LINE")
    else
        NEW="$LINE"
    fi
    # Instala a nova crontab
    echo "$NEW" | crontab -
    CURRENT="$NEW"
    echo "🚀 Instalei cron: $LINE"
done

# Exibir a crontab final para confirmar
crontab -l

#!/bin/bash
# git-init-push.sh
# Cria repositório no GitHub (se não existir) e faz push da pasta local.
# Uso: ./git-init-push.sh /caminho/da/pasta
#
# Requer: gh autenticado

set -uo pipefail

FOLDER="$1"
GITHUB_USER="Robson-Brasil"

if [[ -z "$FOLDER" ]]; then
  echo "Uso: $0 /caminho/da/pasta"
  exit 1
fi

if [[ ! -d "$FOLDER" ]]; then
  echo "ERRO: Pasta nao encontrada: $FOLDER"
  exit 1
fi

REPO_NAME=$(basename "$FOLDER")
cd "$FOLDER" || exit 1

# 1. Inicializa git se nao existir
if [[ ! -d ".git" ]]; then
  echo "Inicializando git..."
  git init
fi

# 2. Renomeia branch para main se estiver como master
BRANCH=$(git branch --show-current)
if [[ "$BRANCH" == "master" ]]; then
  echo "Renomeando branch master -> main..."
  git branch -m master main
elif [[ -z "$BRANCH" ]]; then
  echo "Criando primeiro commit para definir branch main..."
  git checkout -b main
fi

# 3. Verifica se o repo existe no GitHub
if gh repo view "$GITHUB_USER/$REPO_NAME" &>/dev/null; then
  echo "Repositório já existe no GitHub."
else
  echo "Criando repositório no GitHub: $GITHUB_USER/$REPO_NAME..."
  gh repo create "$REPO_NAME" --public --push=false
  echo "Repositório criado!"
fi

# 4. Configura remote se nao existir
if ! git remote get-url origin &>/dev/null; then
  echo "Adicionando remote origin..."
  git remote add origin "https://github.com/$GITHUB_USER/$REPO_NAME.git"
fi

# 5. Add, commit, push
git add .
if git diff --cached --quiet; then
  echo "Nada para commitar."
else
  git commit -m "Initial commit - $REPO_NAME"
fi

echo "Fazendo push..."
git push origin main

echo "Concluido: $REPO_NAME"

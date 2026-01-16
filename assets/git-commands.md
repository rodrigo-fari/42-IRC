
# Guia Prático de Git para Trabalho em Grupo

Este guia descreve um fluxo comum de trabalho em grupo usando Git e GitHub com as seguintes branches:

- **main**: branch estável / produção  
- **dev**: branch de integração  
- **feature/***: branches individuais de desenvolvimento  

---

## 1. Configuração inicial

### git clone
Copia o repositório remoto para a sua máquina.

```bash
git clone https://github.com/org/projeto.git
```

---

## 2. Ver estado e contexto

### git status
Mostra arquivos modificados, staged e a branch atual.

```bash
git status
```

### git branch
Lista branches locais.

```bash
git branch
```

### git branch -a
Lista branches locais e remotas.

```bash
git branch -a
```

### git log --oneline --graph --all
Visualização clara do histórico e merges.

```bash
git log --oneline --graph --all
```

---

## 3. Trabalhando com branches

### git checkout
Troca de branch.

```bash
git checkout dev
```

### git checkout -b
Cria e entra em uma nova branch.

```bash
git checkout -b feature/login
```

Fluxo comum:
```
dev → feature/minha-feature
```

---

## 4. Atualizar sua branch com o remoto

### git pull
Busca mudanças do remoto e faz merge automático.

```bash
git pull origin dev
```

---

## 5. Salvando trabalho

### git add
Seleciona arquivos para commit.

```bash
git add .
```

Ou específico:

```bash
git add src/login.c
```

### git commit
Salva um checkpoint local.

```bash
git commit -m "Add login validation"
```

---

## 6. Enviar código para o remoto

### git push
Envia commits para o GitHub.

```bash
git push origin feature/login
```

Para branch nova:

```bash
git push -u origin feature/login
```

---

## 7. Atualizar sua feature com a dev

### git fetch
Busca atualizações sem alterar a branch atual.

```bash
git fetch origin
```

### git merge
Une a branch dev à sua feature.

```bash
git checkout feature/login
git merge dev
```

---

## 8. Resolver conflitos

Quando ocorrer conflito:

```text
CONFLICT (content): Merge conflict in file.c
```

Passos:
1. Abrir o arquivo
2. Resolver manualmente
3. Marcar como resolvido

```bash
git add file.c
git commit
```

---

## 9. Pull Request

Fluxo recomendado:

- feature → dev
- dev → main

---

## 10. Limpeza e manutenção

### git branch -d
Apaga branch local após merge.

```bash
git branch -d feature/login
```

### git pull --rebase
Atualiza mantendo histórico limpo.

```bash
git pull --rebase origin dev
```

---

## 11. Comandos de emergência

### git stash
Guarda alterações temporariamente.

```bash
git stash
git stash pop
```

### git reset --hard
Descarta alterações locais (destrutivo).

```bash
git reset --hard HEAD
```

---

## Fluxo resumido

```text
1. checkout dev
2. pull origin dev
3. checkout -b feature/x
4. codar
5. add + commit
6. pull origin dev
7. merge dev
8. push feature/x
9. PR → dev
```

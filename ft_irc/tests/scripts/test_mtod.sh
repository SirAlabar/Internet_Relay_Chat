#!/bin/bash

# Cores
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

IRC_PORT=6667
IRC_PASSWORD="senha123"

echo -e "${BLUE}=== Testando MOTD Command ===${NC}"

# Função para teste IRC
test_motd() {
    local test_name="$1"
    local commands=("${@:2}")
    
    echo -e "${YELLOW}[TEST] $test_name${NC}"
    
    (
        for cmd in "${commands[@]}"; do
            printf "%s\r\n" "$cmd"
            sleep 0.3
        done
        sleep 3
    ) | nc -w 5 localhost $IRC_PORT > "motd_${test_name}.log" 2>&1
    
    # Mostrar resultado
    if [ -f "motd_${test_name}.log" ]; then
        echo -e "${BLUE}Resultado:${NC}"
        cat "motd_${test_name}.log" | sed 's/^/    /'
        echo ""
    fi
}

# Iniciar servidor
echo -e "${BLUE}Iniciando servidor...${NC}"
./ircserv $IRC_PORT $IRC_PASSWORD > server_motd.log 2>&1 &
SERVER_PID=$!
sleep 3

# Verificar se servidor está rodando
if ! nc -z localhost $IRC_PORT 2>/dev/null; then
    echo -e "${RED}❌ Servidor não está respondendo${NC}"
    kill $SERVER_PID 2>/dev/null
    exit 1
fi

echo -e "${GREEN}✅ Servidor rodando${NC}"

# Teste 1: MOTD básico após autenticação
test_motd "basic" \
    "PASS $IRC_PASSWORD" \
    "NICK motdtest" \
    "USER motdtest 0 * :MOTD Test" \
    "MOTD"

# Teste 2: MOTD sem autenticação
test_motd "no_auth" \
    "MOTD"

# Teste 3: MOTD case-insensitive
test_motd "case_insensitive" \
    "PASS $IRC_PASSWORD" \
    "NICK motdtest2" \
    "USER motdtest2 0 * :MOTD Test 2" \
    "motd" \
    "Motd" \
    "MOTD"

# Teste 4: MOTD com parâmetros
test_motd "with_params" \
    "PASS $IRC_PASSWORD" \
    "NICK motdtest3" \
    "USER motdtest3 0 * :MOTD Test 3" \
    "MOTD server" \
    "MOTD localhost"

# Parar servidor
kill $SERVER_PID 2>/dev/null
sleep 2

# Analisar resultados
echo -e "${BLUE}=== ANÁLISE DOS RESULTADOS ===${NC}"

analyze_test() {
    local test_name="$1"
    local expected_codes="$2"
    
    if [ -f "motd_${test_name}.log" ]; then
        echo -e "${YELLOW}Teste: $test_name${NC}"
        
        # Verificar códigos de resposta esperados
        for code in $expected_codes; do
            if grep -q "$code" "motd_${test_name}.log"; then
                echo -e "  ${GREEN}✅ Código $code encontrado${NC}"
            else
                echo -e "  ${RED}❌ Código $code não encontrado${NC}"
            fi
        done
        
        # Mostrar mensagens relevantes
        echo -e "  ${BLUE}Respostas IRC:${NC}"
        grep -E "^:[^ ]+ [0-9]+" "motd_${test_name}.log" | sed 's/^/    /' || echo "    Nenhuma resposta IRC padrão encontrada"
        echo ""
    fi
}

# Códigos IRC esperados para MOTD:
# 375 - RPL_MOTDSTART
# 372 - RPL_MOTD  
# 376 - RPL_ENDOFMOTD
# 422 - ERR_NOMOTD (se não há MOTD)
# 451 - ERR_NOTREGISTERED (se não autenticado)

analyze_test "basic" "375 372 376 422"
analyze_test "no_auth" "451"
analyze_test "case_insensitive" "375 372 376 422"
analyze_test "with_params" "375 372 376 422"

# Limpar arquivos de teste
echo -e "${BLUE}Limpando arquivos de teste...${NC}"
rm -f motd_*.log server_motd.log

echo -e "${GREEN}Teste MOTD completo!${NC}"
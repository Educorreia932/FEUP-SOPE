# TP2: Acesso informático aos Quartos de Banho

Este segundo projeto da unidade curricular de Sistemas Operativos consiste numa aplicação do tipo cliente-servidor com a capacidade de lidar com situações de conflito no acesso a zonas partilhadas.

Neste contexto em particular, a zona partilhada será um quarto-de-banho (controlado por um processo Q), ao qual se iram dirigir pedidos de acesso de utentes/clientes (controlados por um processo U). 

Cada pedido de acesso, bem como o tratamento desses pedidos são *threads*, daí a vertente multi-thread deste projeto.

## Invocação

A execução do programa passa por dois processos diferentes, correspondentes ao cliente e ao servidor, respetivamente, e como tal, cada um deles deverá ser invocado na linha de comandos da seguinte maneira.

### Cliente

```shell
U2 <-t nsecs>  fifoname
```

### Servidor

```shell
Q2 <-t nsecs> fifoname [-l nplaces] [-n threads]
```

## Funcionamento

Para esta primeira entrega do projeto, implementámos todas as funcionalidades referentes à mesma, no enunciado. 

No início do programa, as flags de invocação são analisadas por cada um dos processos e guardadas na struct corresponde (flagsQ ou flagsU).
O canal de comunicação público entre o cliente e o servidor é criado e o processo U começa a criar as threads (send_request) responsáveis pelo envio das mensagens.
Estes threads começam por criar o fifo privado que servirá para receber a resposta do servidor. De seguida enviam o pedido no canal público e esperam pela resposta.

Entretanto no lado do servidor o fifo público é aberto e imediatamente após a receção de uma mensagem é criado um thread (handle_request).
A informação da mensagem recebida é utilizada para abrir o fifo privado e a resposta é subsequentemente enviada para este.

Posteriormente, do lado do cliente, a resposta do servidor é lida e enviada para a stdout.

O conteúdo das mensagens trocadas entre os dois processos é guardado numa struct do tipo message_t, que possui os seguintes campos:

- **i** - o número sequencial do pedido (gerado por Un)
- **pid** - identificador de sistema do processo (cliente, no caso do pedido; servidor, no caso da resposta)
- **tid** - identificador no sistema do thread cliente (cliente, no caso do pedido; servidor, no caso da
resposta)
- **dur** - duração, em milissegundos, de utilização (de um lugar) do Quarto de Banho (valor atribuído
no pedido e repetido na resposta, se se der a ocupação; se não se der, por motivo de o Quarto de
Banho estar em vias de encerrar, o servidor responde aqui com o valor -1
- **pl** – nº de lugar que eventualmente lhe será atribuído no Quarto de Banho (no pedido, este campo é
preenchido com o valor -1 e na resposta terá o valor do lugar efetivamente ocupado ou também -1,
na sequência de insucesso de ocupação, por motivo de encerramento)

Assumiu-se que a capacidade inicial da casa-de-banho seria ilimitada, recebendo pedidos de 5ms em 5ms e cada um com uma duração aleatória dentro de um intervalo definido.

## Registo

As diferentes fases de operação são registadas através de mensagens enviadas para a saída padrão (stdout) que indicam que ação foi executada a cada momento, podendo ser uma das seguintes:

- **IWANT** - cliente faz pedido inicial
- **RECVD** - servidor acusa receção de pedido
- **ENTER** - servidor diz que aceitou pedido
- **IAMIN** - cliente acusa a utilização do Quarto de Banho
- **TIMUP** - servidor diz que terminou o tempo de utilização
- **2LATE** - servidor rejeita pedido por Quarto de Banho já ter encerrado
- **CLOSD** - cliente acusa informação de que o Quarto de Banho está fechado
- **FAILD** - cliente já não consegue receber resposta do servidor
- **GAVUP** - servidor já não consegue responder a pedido porque FIFO privado do cliente fechou

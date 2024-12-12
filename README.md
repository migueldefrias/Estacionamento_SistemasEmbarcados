

# **Trabalho 1 - 2024.1 - NOTA 9.0**

## **Autores**
**Nome:** Miguel Matos Costa de Frias Barbosa &emsp; **Matrícula:** 211039635 <br>
**Nome:** Yan Luca Viana de Araújo Fontenele  &emsp; **Matrícula:**  211031889


## **Objetivo**
&emsp;&emsp;Este trabalho tem por objetivo a criação de um sistema distribuído para o controle e monitoramento de estacionamentos comerciais. Dentre os itens controlados teremos a entrada e saída de veículos, a ocupação de cada vaga individualmente, a ocupação do estacionamento como um todo e a cobrança por tempo de permanência.

## **Dependências**
&emsp;&emsp;Este trabalho foi desenvolvido usando a linguagem C.
- Bibliotecas auxiliares:
    - [BCM2835](http://www.airspayce.com/mikem/bcm2835/).

## **Instruções/Como rodar**
- Enviar a pasta Estacionamento disponibilizada nesse repositório para a placa.
```bash
scp -P porta_da_placa -r ./Estacionamento  id@ip_da_placa:~/
```
- Acessar a placa responsável pela sala que será testada
```bash
ssh id@ip_da_placa -p porta_da_placa
```
- Ter as dependências devidamente instaladas no ambiente de teste
- Por fim verifique se o Makefile está no mesmo diretório, e execute:
    ```bash
    make all
    ```
- Em seguida, execute em 4 terminais separados, primeiramente a central depois os andares em qualquer ordem:<br>

  - Para o Servidor Central:
    ```bash
    make central
    ```

   - Para o Andar Terreo:
        ```bash
        make terreo
        ```
    - Para o Primeiro Andar:
        ```bash
        make andar1
        ```
    - Para o Segundo Andar:
        ```bash
        make andar2
        ```
## Vídeo de apresentação
-  Acesse <<https://youtu.be/MopcNhOZfbI>> para ver no youtube.

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>


//ANDAR TÉRREO
#define ENDERECO_01 RPI_GPIO_P1_15                          // PINO 22 - SAÍDA
#define ENDERECO_02 RPI_V2_GPIO_P1_37                       // PINO 26 - SAÍDA
#define ENDERECO_03 RPI_V2_GPIO_P1_35                       // PINO 19 - SAÍDA
#define SENSOR_DE_VAGA RPI_GPIO_P1_12                       // PINO 18 - ENTRADA
#define SINAL_DE_LOTADO_FECHADO RPI_V2_GPIO_P1_13           // PINO 27 - SAÍDA
#define SENSOR_ABERTURA_CANCELA_ENTRADA RPI_GPIO_P1_16      // PINO 23 - ENTRADA
#define SENSOR_FECHAMENTO_CANCELA_ENTRADA RPI_GPIO_P1_18    // PINO 24 - ENTRADA
#define MOTOR_CANCELA_ENTRADA RPI_GPIO_P1_19                // PINO 10 - SAÍDA
#define SENSOR_ABERTURA_CANCELA_SAIDA RPI_GPIO_P1_22        // PINO 25 - ENTRADA
#define SENSOR_FECHAMENTO_CANCELA_SAIDA RPI_V2_GPIO_P1_32   // PINO 12 - ENTRADA
#define MOTOR_CANCELA_SAIDA RPI_GPIO_P1_11                  // PINO 17 - SAÍDA

void configuraPinos(){
    bcm2835_gpio_fsel(ENDERECO_01, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(ENDERECO_02, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(ENDERECO_03, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SENSOR_DE_VAGA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SINAL_DE_LOTADO_FECHADO, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SENSOR_ABERTURA_CANCELA_ENTRADA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SENSOR_FECHAMENTO_CANCELA_ENTRADA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(MOTOR_CANCELA_ENTRADA, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SENSOR_ABERTURA_CANCELA_SAIDA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SENSOR_FECHAMENTO_CANCELA_SAIDA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(MOTOR_CANCELA_SAIDA, BCM2835_GPIO_FSEL_OUTP);
}
typedef struct vaga
{
    struct timeval hent;    // Horário de entrada do carro na vaga
    struct timeval hsaida;  // Horário de saída do carro da vaga
    int tempo;              // Tempo de permanência do carro na vaga
    int ncarro;             // Número do carro estacionado
    int ocupado;            // ex: Vaga 1 = 1, Vaga 2 = 2, Vaga 3 = 3, Vaga 4 = 4, Vaga 5 = 5, Vaga 6 = 6, Vaga 7 = 7, Vaga 8 = 8, Vaga livre = 0
    bool boolocupado;       // 0 = vaga livre, 1 = vaga ocupada
}vaga;

typedef struct vsoma{
    int somaValores;    // Soma dos valores das vagas ocupadas (ex: 1+2+3+4+5+6+7+8 = 36)
    int somaVagas;      // Soma das vagas ocupadas (ex: 1+1+1+1+1+1+1+1 = 8)
}vsoma;

int carro = 0;
int valor;                          // Valor lido pelo sensor de vaga
bool carroAndar = false;            // 0 = carro não está andando, 1 = carro está andando
vaga *v;
vsoma x;
int k=0, j=0, carroTotal=0;
int idoso = 2, pcd = 1, normal = 5;
int anteriorSomaValores = 0;

#define tamVetorEnviar 22
#define tamVetorReceber 5

int parametros[tamVetorEnviar];
int recebe[tamVetorReceber];
int fechado = 0;

int separaIguala(){
    
    parametros[0] = pcd;
    parametros[1] = idoso;
    parametros[2] = normal;
    parametros[3] = v[0].boolocupado;
    parametros[4] = v[1].boolocupado;
    parametros[5] = v[2].boolocupado;
    parametros[6] = v[3].boolocupado;
    parametros[7] = v[4].boolocupado;
    parametros[8] = v[5].boolocupado;
    parametros[9] = v[6].boolocupado;
    parametros[10] = v[7].boolocupado;
    parametros[12] = carroTotal;
    parametros[18] = x.somaVagas;
    fechado = recebe[1];
    parametros[19] = recebe[4];    
}

//Função que lê o sensor da cancela de entrada quando um carro está entrando no estacionamento
void * sensorEntrada(){
    while(1){
        
        if(fechado==0){

        //Lê o sensor de abertura da cancela de entrada e aciona o motor da cancela para abrir
        if(HIGH == bcm2835_gpio_lev(SENSOR_ABERTURA_CANCELA_ENTRADA)){
            bcm2835_gpio_write(MOTOR_CANCELA_ENTRADA, HIGH);
           parametros[19]=1;
        }
        //Lê o sensor de saída da cancela e aciona o motor da cancela para fechar
        if(HIGH == bcm2835_gpio_lev(SENSOR_FECHAMENTO_CANCELA_ENTRADA)){
            bcm2835_gpio_write(MOTOR_CANCELA_ENTRADA, LOW);
            if(j==0){
                ++carroTotal;
                j=1;
                parametros[19]=1;
            }
        }else j=0;
        
        }
            
    }
}

//Função que lê o sensor da cancela de saída quando um carro está saindo do estacionamento
void * sensorSaida(){
    while(1){
         //Lê o sensor de abertura da cancela de saida e aciona o motor da cancela para abrir
        if(HIGH == bcm2835_gpio_lev(SENSOR_ABERTURA_CANCELA_SAIDA)){
            bcm2835_gpio_write(MOTOR_CANCELA_SAIDA, HIGH);
            
        }
        //Lê o sensor de saída da cancela de saída e aciona o motor da cancela para fechar
        if(HIGH == bcm2835_gpio_lev(SENSOR_FECHAMENTO_CANCELA_SAIDA)){
            bcm2835_gpio_write(MOTOR_CANCELA_SAIDA, LOW);
            parametros[19]=0;
        }
    }
}

//Função que verifica quais vagas estão ocupadas
void vagasOcupadas(vaga *v){
    
    x.somaValores = 0;
    x.somaVagas = 0;
    for(int i = 0; i<8; i++){
        if(v[i].ocupado != 0){
            x.somaVagas++;
        }
        x.somaValores += v[i].ocupado;
    }
}

//Função que verifica e imprime as vagas disponíveis por tipo
void * vagasDisponiveis(vaga *v){
    idoso = 2;
    pcd = 1;
    normal = 5;
    
        for(int i=3; i<8; i++){
            if(v[i].ocupado > 0)
                v[i].boolocupado = 1;
            
            else if(v[i].ocupado == 0)
                v[i].boolocupado=0;
            normal -= v[i].boolocupado;
        }
        for(int i=1; i<3; i++){
            if(v[i].ocupado > 0)
                v[i].boolocupado = 1;
            
            else if(v[i].ocupado == 0)
                v[i].boolocupado=0;

            
            idoso -= v[i].boolocupado;
        }
        
        if(v[0].ocupado > 0){
            pcd=0;
            v[0].boolocupado = 1;
        }
        else if(v[0].ocupado == 0) {
            pcd=1;
            v[0].boolocupado=0;
        }

}

//Função que verifica a mudança de estado das vagas
int mudancaEstadoVaga(vsoma *s, int anteriorSomaValores){
    return anteriorSomaValores-s->somaValores; 
}

//Função que calcula o tempo de permanência do carro na vaga
int timediff(struct timeval entrada, struct timeval saida){
    return (int)(saida.tv_sec - entrada.tv_sec);
}

//Função que calcula o valor a ser pago pelo carro que estava na vaga
void pagamento(int g, vaga *v){
    gettimeofday(&v[g-1].hsaida,0);
    v[g-1].tempo = timediff(v[g-1].hent,v[g-1].hsaida)/60;
    float x = v[g-1].tempo*0.1;
    parametros[14]=1;
    parametros[15]=v[g-1].ncarro;
    parametros[16]=v[g-1].tempo;
    parametros[17]=g;
    delay(1000);
    parametros[14]=0;
}

//Função que verifica em qual vaga o carro estacionou
void buscaCarro(int f , vaga *v){
    f *= -1;
    v[f-1].ncarro = carroTotal;
    gettimeofday(&v[f-1].hent,0);
    parametros[11] = 1;
    parametros[13] = f;

    delay(1000);
    parametros[11] = 0;
}

//Função que lê o estado das vagas do terreo
void leituraVagasTerreo(vaga *v){
    k=0;
    anteriorSomaValores = 0;        
    x.somaVagas = 0;
    x.somaValores = 0;

    while(1){
        delay(50);
        delay(50);
        vagasOcupadas(v);
        vagasDisponiveis(v);
        separaIguala();

        //Primeira vaga
        bcm2835_gpio_write(ENDERECO_01, LOW);
        bcm2835_gpio_write(ENDERECO_02, LOW);
        bcm2835_gpio_write(ENDERECO_03, LOW);
        delay(50);
        valor = bcm2835_gpio_lev(SENSOR_DE_VAGA);
        if(valor == 1)
            v[0].ocupado = 1;

        else if(valor == 0) 
            v[0].ocupado = 0;
        
        //Segunda vaga
        bcm2835_gpio_write(ENDERECO_01, HIGH);
        bcm2835_gpio_write(ENDERECO_02, LOW);
        bcm2835_gpio_write(ENDERECO_03, LOW);
        delay(50);
        valor = bcm2835_gpio_lev(SENSOR_DE_VAGA);
        if(valor == 1)
            v[1].ocupado = 2;
        else if(valor == 0) 
            v[1].ocupado = 0;

        //Terceira vaga
        bcm2835_gpio_write(ENDERECO_01, LOW);
        bcm2835_gpio_write(ENDERECO_02, HIGH);
        bcm2835_gpio_write(ENDERECO_03, LOW);
        delay(50);
        valor = bcm2835_gpio_lev(SENSOR_DE_VAGA);
        if(valor == 1)
            v[2].ocupado = 3;
        else if(valor == 0) 
            v[2].ocupado = 0;        

        //Quarta vaga
        bcm2835_gpio_write(ENDERECO_01, HIGH);
        bcm2835_gpio_write(ENDERECO_02, HIGH);
        bcm2835_gpio_write(ENDERECO_03, LOW);
        delay(50);
        valor = bcm2835_gpio_lev(SENSOR_DE_VAGA);
        if(valor == 1)
            v[3].ocupado = 4;
        else if(valor == 0) 
            v[3].ocupado = 0;        

        //Quinta vaga
        bcm2835_gpio_write(ENDERECO_01, LOW);
        bcm2835_gpio_write(ENDERECO_02, LOW);
        bcm2835_gpio_write(ENDERECO_03, HIGH);
        delay(50);
        valor = bcm2835_gpio_lev(SENSOR_DE_VAGA);
        if(valor == 1)
            v[4].ocupado = 5;
        else if(valor == 0) 
            v[4].ocupado = 0;        

        //Sexta vaga
        bcm2835_gpio_write(ENDERECO_01, HIGH);
        bcm2835_gpio_write(ENDERECO_02, LOW);
        bcm2835_gpio_write(ENDERECO_03, HIGH);
        delay(50);
        valor = bcm2835_gpio_lev(SENSOR_DE_VAGA);
        if(valor == 1)
            v[5].ocupado = 6;
        else if(valor == 0) 
            v[5].ocupado = 0;        

        //Sétima vaga
        bcm2835_gpio_write(ENDERECO_01, LOW);
        bcm2835_gpio_write(ENDERECO_02, HIGH);
        bcm2835_gpio_write(ENDERECO_03, HIGH);
        delay(50);
        valor = bcm2835_gpio_lev(SENSOR_DE_VAGA);
        if(valor == 1)
            v[6].ocupado = 7;
        else if(valor == 0) 
            v[6].ocupado = 0;
 

        //Oitava vaga
        bcm2835_gpio_write(ENDERECO_01, HIGH);
        bcm2835_gpio_write(ENDERECO_02, HIGH);
        bcm2835_gpio_write(ENDERECO_03, HIGH);
        delay(50);
        valor = bcm2835_gpio_lev(SENSOR_DE_VAGA);
        if(valor == 1)
            v[7].ocupado = 8;
        else if(valor == 0) 
            v[7].ocupado = 0;
            
        k = mudancaEstadoVaga(&x, anteriorSomaValores);
        parametros[16]=0;
        if(k>0 && k<9){
            parametros[19] = 1;
            pagamento(k, v);
        }else if(k<0 && k>-9){  
            parametros[19] = 0;
            buscaCarro(k, v);
        } 
        anteriorSomaValores = x.somaValores;
    
        if(fechado == 1) {
            bcm2835_gpio_write(SINAL_DE_LOTADO_FECHADO, HIGH);
        }
        else if(fechado == 0) {
            bcm2835_gpio_write(SINAL_DE_LOTADO_FECHADO, LOW);
        }
    }
}

void *chamaLeitura(){
    leituraVagasTerreo(v);
}

void *enviaParametros(){
    char *ip ="127.0.0.1";
    int port = 10683;
    
    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]Server socket created\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("Connected to Server\n");
    while(1){
        send (sock, parametros, tamVetorEnviar *sizeof(int) , 0);
        recv(sock, recebe, tamVetorReceber * sizeof(int), 0);
        delay(1000);
    }
    close(sock);
    printf("Disconnected from server\n");
}

int mainT(){
    //mainT
    if (!bcm2835_init())
        return 1;

    configuraPinos();
    carro = 0;

    v = calloc(8,sizeof(vaga));

    pthread_t fEntrada, fSaida, fLeituraVagas, fEnviaParametros;

    pthread_create(&fLeituraVagas, NULL, chamaLeitura, NULL);
    pthread_create(&fEnviaParametros, NULL, enviaParametros, NULL);
    pthread_create(&fEntrada,NULL,sensorEntrada,NULL);
    pthread_create(&fSaida,NULL,sensorSaida,NULL);

    pthread_join(fEntrada,NULL);
    pthread_join(fSaida,NULL);
    pthread_join(fLeituraVagas, NULL);
    pthread_join(fEnviaParametros, NULL);

    bcm2835_close();
    return 0;
}